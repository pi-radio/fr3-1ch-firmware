#pragma once

#include <cstdint>

namespace TXX
{
  namespace config_data
  {
    static constexpr uint16_t CUR_VERSION = 0x0001;
    static constexpr uint16_t TAG_CONFIG_HEADER = 0xB002;
    static constexpr uintptr_t config_addr = 0x09000000;
    
    template <int type_code, class Tv>
    struct tlv {
      uint16_t t;
      uint16_t l;
      Tv       v;

      tlv() : t(type_code), l(sizeof(Tv) + 4) {}
      tlv(Tv &_v) : t(type_code), l(sizeof(Tv) + 4), v(_v) {}
    } __attribute__((packed));

    struct config_header {
      uint32_t version;
    };
    
    struct config_serial {
      uint32_t  serno[16];
    };


    extern struct _uid U_ID;

    class _config
    {
    public:
      _config();

      void erase();
      uint16_t read_word(uint32_t);
    };

    extern _config config;

#if 0    

extern void init_config_data(void);
extern uint16_t config_get_flash_code(void);
extern uint16_t config_get_package_code(void);

extern void erase_config_data(void);

static inline uint32_t config_get_flash_size(void) {
  return config_get_flash_code() * 1024UL;
}

extern uint16_t config_read_word(uint32_t offset);

extern int is_fr3_1ch_board(void);

#endif
    
  };
}

class ConfigRepo
{
};
