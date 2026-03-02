


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
