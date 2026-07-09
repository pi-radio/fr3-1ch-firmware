#include <stm32h5/flash.hpp>

extern "C" {
#include <stm32h573xx.h>
#include <stm32h5xx_hal.h>
}

#include <halxx/fault.hpp>
#include <cassert>

using namespace flash;

static constexpr uint32_t config_base = 0x09000000;



void Flash::Init()
{
  FLASH_OBProgramInitTypeDef pOBInit = {0};

  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }
  
  pOBInit.OptionType = OPTIONBYTE_EDATA;
  pOBInit.Banks = FLASH_BANK_BOTH;
  pOBInit.EDATASize = 8;
  
  if (HAL_FLASHEx_OBProgram(&pOBInit) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_FLASH_OB_Launch() != HAL_OK) {
    Error_Handler();
  }
  
  if (HAL_FLASH_OB_Lock() != HAL_OK)
  {
    Error_Handler();
  }
  
  FLASH_NS->WRP1R_PRG = 0xC0000000;
  FLASH_NS->WRP2R_PRG = 0xC0000000;

  FLASH_NS->NSCR |= FLASH_CR_PG;
}

void Flash::WaitErase()
{
  if (FLASH_NS->NSSR & FLASH_FLAG_WBNE) {
    return;
  }

  while (FLASH_NS->NSSR & FLASH_FLAG_BSY);

  if (!(FLASH_NS->NSSR & FLASH_FLAG_EOP))
  {
    // EH?
    int v = 0;
    v++;
  }

  // Check other flags for multiple set flags


  if (FLASH_NS->NSSR & FLASH_FLAG_PGSERR) {
    FLASH_NS->NSCCR = FLASH_CCR_CLR_PGSERR;
    throw ProgramSequenceError();
  }

}

void Flash::WaitProgram()
{
  if (FLASH_NS->NSSR & FLASH_FLAG_WBNE) {
    return;
  }

  while (FLASH_NS->NSSR & FLASH_FLAG_BSY);

  if (!(FLASH_NS->NSSR & FLASH_FLAG_EOP))
  {
    // EH?
    int v = 0;
    v++;
  }

  // Check other flags for multiple set flags


  if (FLASH_NS->NSSR & FLASH_FLAG_PGSERR) {
    FLASH_NS->NSCCR = FLASH_CCR_CLR_PGSERR;
    throw ProgramSequenceError();
  }

}

uint32_t HEFlash::error_info = -1;
uint32_t HEFlash::err_addr = -1;
uint32_t HEFlash::err_data = -1;
uint32_t HEFlash::cur_read = -1;
bool HEFlash::read_fail = false;

void HEFlash::erase_sector(uint32_t sector)
{
  uint32_t bank = sector > NSECTORS/2;

  if (FLASH_NS->NSSR & FLASH_FLAG_PGSERR) {
      FLASH_NS->NSCCR = FLASH_CCR_CLR_PGSERR;
  }

  while ((FLASH_NS->NSSR & (FLASH_FLAG_BSY | FLASH_FLAG_DBNE) != 0));

  sector = (sector & 0x7) + 120;

  if (!bank) {
    FLASH_NS->NSCR &= ~FLASH_CR_BKSEL;
  } else {
    FLASH_NS->NSCR |= FLASH_CR_BKSEL;
  }

  FLASH_NS->NSCR &= ~(FLASH_CR_SNB | FLASH_CR_PG);

  FLASH_NS->NSCR |= (FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);

  while ((FLASH_NS->NSSR & (FLASH_FLAG_BSY | FLASH_FLAG_WBNE | FLASH_FLAG_DBNE)) != 0);

  if (FLASH_NS->NSSR & FLASH_FLAG_PGSERR) {
    FLASH_NS->NSCCR = FLASH_CCR_CLR_PGSERR;
    throw ProgramSequenceError();
  }

  FLASH_NS->NSCR |= FLASH_CR_PG;
}

volatile uint16_t *HEFlash::ptr16(uint32_t offset)
{
  return (__IO uint16_t *)(config_base + 2 * offset);
}

uint16_t HEFlash::read16(uint32_t offset)
{
  uint16_t retval;
  
  /* Program a halfword word (16 bits) */
  retval = *ptr16(offset);

  if (read_fail) {
    read_fail = false;
    // TODO -- correlate the addr to the offset
    throw UncorrectibleECCError(error_info, err_addr, err_data);
  }

  return retval;
}


void HEFlash::write16(uint32_t offset, uint16_t v)
{
  /* Program a halfword word (16 bits) */
  *ptr16(offset) = v;

  Flash::WaitProgram();


  try {
    uint16_t test = read16(offset);

    assert(test == v);
  } catch(UncorrectibleECCError e) {
    int v = 1;
    v++;
  }
}

extern "C" void handle_flash(void)
{
  HEFlash::error_info = FLASH_NS->ECCDETR;
  uint32_t error_data = FLASH_NS->ECCDR;
  
  SET_BIT(FLASH_NS->ECCDETR, FLASH_ECCR_ECCD);

  if (HEFlash::error_info & (1 << 21)) {
    HEFlash::err_addr = HEFlash::error_info & 0xFFFF;
    HEFlash::err_data = error_data & 0xFFFF;
    HEFlash::read_fail = true;
  }
  
  // Invalidate instruction cache
  SET_BIT(ICACHE->CR, ICACHE_CR_CACHEINV);
}
