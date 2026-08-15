#include <cassert>
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

  config_header hdr(0);

  if (page_seq.size() == 0) {
    uint32_t page = free_pages.front();
    free_pages.pop_front();

    // Write the first page

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
    uint32_t read_pos = page->read16(1);

    while(true) {
      uint16_t tag;

      try {
        tag = page->read16(read_pos);
      } catch(UncorrectibleECCError e) {
        page->append_point = read_pos;
        break;
      }

      if (addrs.contains(tag)) {
        uint32_t page_no = addrs[tag].first;

        pages[page_no].valid_tags.erase(tag);
        pages[page_no].free_space += values[tag]->l;
      }

      addrs.insert_or_assign(tag, std::make_pair(page->N, read_pos));

      auto p = registry.read(tag, page->N, read_pos);

      page->free_space -= p->l;

      values[tag] = p;

      read_pos += p->l;


    }
  }

}

uint32_t _config::allocate_page()
{
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

  if (free_pages.size() == 0) {
    // We should reclaim pages -- we start from the oldest
    // We should also make *damn* sure we don't need to allocate another page,
    // however, at this point the total data size of the config data is quite
    // small
    for (auto entry = page_seq.begin(); entry != --page_seq.end(); ++entry) {
      while(entry->second->valid_tags.size()) {
        std::cout << "Copying forward config tag " << entry->first << std::endl;

        _do_save(*values[*entry->second->valid_tags.begin()]);
      }

      erase_page(entry->second->N);
    }
  }

  return page;
}

void _config::_do_save(const uint16_t *data, uint16_t length)
{
  uint16_t tag = *data;
  page *append_page = (*(--page_seq.end())).second;

  assert(append_page->has_room(length));

  if (addrs.contains(tag)) {
    // FIX ME -- Invalidate the current doohickey
    auto loc = addrs[tag];

    pages[loc.first].free_space += values[tag]->l;
  }

  auto pos = append_page->append_point;

  addrs.insert_or_assign(tag, std::make_pair(append_page->N, pos));

  for (uint16_t i = 0; i < length; i++) {
    append_page->append(*data++);
  }

  auto p = registry.read(tag, append_page->N, pos);

  append_page->free_space -= p->l;

  values[tag] = p;
}

void _config::save(const uint16_t *data, uint16_t length)
{
  page *append_page = (*(--page_seq.end())).second;

  if (!append_page->has_room(length)) {
    // Allocate new page
    allocate_page();

    append_page = (*(--page_seq.end())).second;
  }

  _do_save(data, length);
}

void _config::erase_page(uint32_t page_no)
{
  page &p = pages[page_no];

  p.append_point = 0;
  p.free_space = 0;
  p.serial = 0xFFFFFFFF;

  page_seq.erase(p.serial);

  std::cout << "Erasing config page " << page_no << std::endl;

  HEFlash::erase_sector(page_no);
}

void _config::erase()
{
  page_seq.clear();
  values.clear();
  addrs.clear();

  for (uint32_t i = 0; i < HEFlash::NSECTORS; i++)
  {
    erase_page(i);
  }
}

uint16_t _config::read_word(uint32_t offset)
{
  return *((uint16_t *)flash::HEFlash::BASE_ADDR + offset);
}


