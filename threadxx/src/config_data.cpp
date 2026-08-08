#include <format>
#include <map>

#include <threadxx/config_data.hpp>
#include <threadxx/dbgstream.hpp>

using namespace TXX::config_data;
using namespace flash;

static int _counter;


static typename std::aligned_storage<sizeof (_config), alignof (_config)>::type
config_buf; // memory for the stream object
_config& TXX::config_data::config = reinterpret_cast<_config &> (config_buf);


static typename std::aligned_storage<sizeof (TLVRegistry), alignof (TLVRegistry)>::type
registry_buf; // memory for the stream object
TLVRegistry& TXX::config_data::registry = reinterpret_cast<TLVRegistry &> (registry_buf);

void TXX::config_data::initialize_config()
{
  using namespace std;
  if (_counter++ == 0) {
    new (&config) _config ();
    new (&registry) TLVRegistry();
  }
}

_config::_config() {
  for (uint32_t i = 0; i < NPAGES; i++) {
    pages[i].N = i;
  }

  next_serial = 0;
}

void _config::scan_headers() {
  for (uint32_t i = 0; i < HEFlash::NSECTORS; i++) {
    std::shared_ptr<config_header> hdr;

    try {
      hdr = read<config_header>(i, 0);
    } catch(UncorrectibleECCError e) {
      if (e.data != 0xFFFF)
        dbg::dbgout << std::format("Uncorrectible ECC: offset: {:08x} info: {:08x} addr: {:04x} data: {:04x}",
            i * HEFlash::SECTOR_SIZE / 2, e.error_info, e.addr, e.data) << std::endl;

      free_pages.push_back(i);
      continue;
    } catch(InvalidLengthException e) {
      dbg::dbgout << std::format("Invalid config header length {} on page {}", e.found_len, i) << std::endl;
      continue;
    }

    dbg::dbgout << "Found data on page " << i << std::endl;

    pages[i].N = i;
    pages[i].serial = hdr->page_serial;
    pages[i].version = hdr->version;
    pages[i].read_pos = hdr->l;
    pages[i].free_space -= sizeof(hdr);

    page_seq[pages[i].serial] = &pages[i];

    if (pages[i].version > CUR_VERSION)
    {

      std::cout << std::format("WARNING: Firmware downgrade detected (my config ver: {}.{}.{} in flash: {}.{}.{}",
          CUR_VERSION.maj, CUR_VERSION.min, CUR_VERSION.rel, pages[i].version.maj, pages[i].version.min, pages[i].version.rel) << std::endl;
    }

    dbg::dbgout << std::format("Found config page {} on page {}", pages[i].N, i) << std::endl;
  }
};

void _config::load() {
  scan_headers();

  if (page_seq.size() == 0) {
    uint32_t page = free_pages.front();
    free_pages.pop_front();

    // Write the first page
    config_header hdr(0);


    pages[page].serial = 0;

    try {
      write(page, hdr);
    } catch(ProgramSequenceError e) {
      dbg::dbgout << std::format("ERROR: Unable to write header to sector {}", 0) << std::endl;
      return;
    }

    page_seq[page] = &pages[page];

    scan_headers();

    if (page_seq.size() == 0) {
      dbg::dbgout << "ERROR: Unable to initialize config area." << std::endl;
    }

    dbg::dbgout << "Initialized config area.  Please provision device." << std::endl;

  }

  for (const auto & [n, page] : page_seq) {
    while(true) {
      uint16_t tag;

      try {
        tag = page->read16(page->read_pos);
      } catch(UncorrectibleECCError e) {
        page->append_point = page->read_pos;
        break;
      }

      if (addrs.contains(tag)) {
        uint32_t page_no = addrs[tag].first;

        pages[page_no].free_space += values[tag]->l;
      }

      addrs.insert_or_assign(tag, std::make_pair(page->N, page->read_pos));

      auto p = registry.read(tag, page->N, page->read_pos);

      page->free_space -= p->l;

      values.erase(tag);
      values.emplace(tag, p);

      page->read_pos += p->l;


    }
  }

}

uint32_t _config::allocate_page()
{
  if (free_pages.size() != 0) {
    uint32_t serial = (*(--page_seq.end())).first + 1;

    uint32_t page = free_pages.front();
    free_pages.pop_front();

    config_header hdr(serial);

    try {
      write(page, hdr);
    } catch(ProgramSequenceError e) {
      dbg::dbgout << std::format("ERROR: Unable to write header to sector {}", 0) << std::endl;
      return -1;
    }

    page_seq[serial] = &pages[page];

    return page;
  }

}

void _config::save(const uint16_t *data, uint16_t length)
{
  page *append_page = (*(--page_seq.end())).second;

  if (!append_page->has_room(length)) {
    // Allocate new page
    allocate_page();

    append_page = (*(--page_seq.end())).second;
  }

  for (uint16_t i = 0; i < length; i++) {
    append_page->append(*data++);
  }
}

void _config::erase()
{
#if 0
  uint32_t bank;
  uint32_t error;
  FLASH_EraseInitTypeDef erase;

  for (bank = 0; bank < 2; bank++) {
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Banks = bank;
    erase.Sector = FLASH_SECTOR_NB - FLASH_EDATA_SECTOR_NB;
    erase.NbSectors = FLASH_EDATA_SECTOR_NB;

    if (HAL_FLASHEx_Erase(&erase, &error) != HAL_OK) {
      Error_Handler();
    }
  }
#endif
}

uint16_t _config::read_word(uint32_t offset)
{
  return *((uint16_t *)flash::HEFlash::BASE_ADDR + offset);
}


