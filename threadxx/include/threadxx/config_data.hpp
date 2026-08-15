#pragma once

#include <cstdint>
#include <deque>
#include <set>
#include <map>
#include <format>
#include <memory>

#include <stm32h5/flash.hpp>

#include <threadxx/dbgstream.hpp>

namespace TXX
{
  namespace config_data
  {
    static constexpr uint32_t CUR_VERSION_UINT32 = 0x00010000U;

    struct config_version
    {
      uint8_t maj;
      uint8_t min;
      uint8_t rel;

      constexpr config_version() : maj((CUR_VERSION_UINT32 >> 24) & 0xFF),
          min((CUR_VERSION_UINT32 >> 16) & 0xFF),
          rel((CUR_VERSION_UINT32 >> 8) & 0xFF)
      {

      }


      config_version(uint32_t v) : maj((v >> 24) & 0xFF),
          min((v >> 16) & 0xFF),
          rel((v >> 8) & 0xFF)
      {

      }

      operator uint32_t() const {
        return (maj << 24) | (min << 16) | (rel << 8) | 0;
      }

      config_version &operator=(uint32_t v) {
        maj = (CUR_VERSION_UINT32 >> 24) & 0xFF;
        min = (CUR_VERSION_UINT32 >> 16) & 0xFF;
        rel = (CUR_VERSION_UINT32 >> 8) & 0xFF;
        return *this;
      }

      bool operator==(const config_version &o) const {
        return (maj == o.maj) &&
               (min == o.min) &&
               (rel == o.rel);
      }

      bool operator <(const config_version &o) const {
        if (maj < o.maj)
          return true;

        if (maj > o.maj)
          return false;

        if (min < o.min)
          return true;

        if (min > o.min)
          return false;

        if (rel < o.rel)
          return true;

        return false;
      }
    };

    static constexpr config_version CUR_VERSION;
    
    struct InvalidTagException
    {
      uint16_t found_tag;

      InvalidTagException(uint16_t t) : found_tag(t) {}
    };

    struct InvalidLengthException
    {
      uint16_t found_len;

      InvalidLengthException(uint16_t l) : found_len(l) {}
    };


    struct tlv_base
    {
      uint16_t t;
      uint16_t l;

      tlv_base(uint16_t _t, uint16_t _l) : t(_t), l(_l) {}
    };

    class TLVRegistry
    {
      std::map<uint16_t, std::shared_ptr<tlv_base> (*)(uint32_t, uint32_t)> read_map;
    public:

      template <typename Tv>
      void register_tlv()
      {
        read_map[Tv::TAG] = &Tv::read;
      }

      std::shared_ptr<tlv_base> read(uint16_t tag, uint32_t page, uint32_t offset) {
        return read_map[tag](page, offset);
      }
    };

    extern TLVRegistry &registry;



    extern struct _uid U_ID;

    struct page
    {
      uint32_t N;
      uint32_t serial;
      config_version version;
      uint32_t append_point;
      //uint32_t read_pos;
      uint32_t free_space;

      std::set<uint16_t> valid_tags;

      page() : N(0xFFFFFFFF),  serial(0xFFFFFFFF),  version(CUR_VERSION), append_point(0), free_space(flash::HEFlash::SECTOR_SIZE) {}

      uint32_t area_offset(uint32_t off) { return  + off; }

      uint32_t base() const { return N * flash::HEFlash::SECTOR_SIZE / 2; }

      uint16_t read16(uint32_t off) {
        uint32_t addr = base() + off;

        //dbg::dbgout << std::format("Reading at {:08x}", (uint32_t)flash::HEFlash::ptr16(addr)) << std::endl;

        return flash::HEFlash::read16(addr);
      }
      void write16(uint32_t off, uint16_t v) {
        uint32_t addr = base() + off;

        //dbg::dbgout << std::format("Writing {:04x} at {:08x}", v, (uint32_t)flash::HEFlash::ptr16(addr)) << std::endl;

        flash::HEFlash::write16(addr, v);
      }

      void append(uint16_t v) { write16(append_point++, v); }

      bool has_room(uint16_t l) { return flash::HEFlash::SECTOR_SIZE - append_point > l; }
    };
    
    class _config
    {
    public:
      static constexpr uint32_t NPAGES = flash::HEFlash::NSECTORS;

    private:
      uint32_t next_serial;
      page pages[NPAGES];
      

      std::deque<uint32_t> free_pages;

      std::map<uint32_t, page *> page_seq;
      std::map<uint16_t, std::pair<uint32_t, uint32_t> > addrs;
      std::map<uint16_t, std::shared_ptr<tlv_base> > values;

      void scan_headers();

      template <typename Tv>
      void _do_save(const Tv &e) {
        _do_save((uint16_t *)&e, e.l);
      }

      void _do_save(const uint16_t *data, uint16_t length);

      void save(const uint16_t *data, uint16_t length);

      void erase_page(uint32_t page);

    public:
      template <typename Tv>
      std::shared_ptr<Tv> get()
      {
        return std::reinterpret_pointer_cast<Tv>(values[Tv::TAG]);
      }


      template <typename Tv>
      std::shared_ptr<Tv> read(uint32_t page_no, uint32_t offset)
      {
        std::shared_ptr<Tv> retval = std::make_shared<Tv>();

        uint16_t tag = pages[page_no].read16(offset++);
        uint16_t len = pages[page_no].read16(offset++);

        if (tag != Tv::TAG) {
          // raise an exception
          throw InvalidTagException(tag);
        }

        if (len < 2) {
          throw InvalidLengthException(len);
        }

        if (2 * len > sizeof(Tv)) {
          throw InvalidLengthException(len);
        }

        uint16_t *pdata = ((uint16_t *)retval.get()) + 2;

        for (int i = 0; i < len - 2; i++) {
          *pdata++ = pages[page_no].read16(offset++);
        }

        return retval;
      }

      template <typename Tv>
      void write(uint32_t page_no, const Tv &tv)
      {
        uint16_t *pdata = (uint16_t *)&tv;

        for (int i = 0; i < tv.l; i++) {
          pages[page_no].append(*pdata++);
        }
      }
      
      uint32_t allocate_page();


      template <typename Tv>
      void save(const Tv &e) {
        save((uint16_t *)&e, e.l);
      }


      _config();

      void load();
      void erase();
      uint16_t read_word(uint32_t);
    };

    extern _config &config;

    extern void initialize_config();

    template <class Tv>
    struct tlv : public tlv_base {

      tlv() : tlv_base(Tv::TAG, sizeof(Tv)/2) {}

      static std::shared_ptr<tlv_base> read(uint32_t page, uint32_t offset) {
        return  config.read<Tv>(page, offset);
      }
    } __attribute__((packed));

    struct config_header : public tlv<config_header> {
      static constexpr uint16_t TAG = 0xB002;

      uint32_t version;
      uint32_t page_serial;

      config_header() {}
      config_header(uint32_t s) : version(CUR_VERSION), page_serial(s) {}
    } __attribute__((packed));


  };


}

class ConfigRepo
{
};
