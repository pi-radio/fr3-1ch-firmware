#pragma once

#include <cstdint>

// TO DO rename
extern "C" void handle_flash(void);

namespace flash
{
  class Flash
  {


  public:
    static void Init();

    static void WaitProgram();
    static void WaitErase();
  };

  struct ProgramSequenceError
  {
    ProgramSequenceError() {}
  };


  struct UncorrectibleECCError
  {
    uint32_t error_info;
    uint32_t addr;
    uint32_t data;

    UncorrectibleECCError(uint32_t i, uint32_t a, uint32_t d) : error_info(i), addr(a), data(d) {}
  };

  struct UnprogrammedAreaException
  {
    uint32_t addr;
    uint32_t data;

    UnprogrammedAreaException(uint32_t a, uint32_t d) : addr(a), data(d) {}
  };
  
  class HEFlash
  {
    friend void ::handle_flash(void);

    static uint32_t error_info;
    static uint32_t err_addr;
    static uint32_t err_data;
    static uint32_t cur_read;
    static bool read_fail;
    
  public:
    static constexpr uint32_t NSECTORS = 16;
    static constexpr uint32_t SECTOR_SIZE = 0x1800;
    static constexpr uint32_t BASE_ADDR = 0x09000000;

    static volatile uint16_t *ptr16(uint32_t offset);

    static bool is_programmed(uint32_t offset);
    static void erase_sector(uint32_t sector);
    static uint16_t read16(uint32_t offset);
    static void write16(uint32_t offset, uint16_t v);
  };
}
