#pragma once

#include <cstdint>

namespace halxx
{
  namespace cortex
  {
    namespace mpu
    {
#if 0
      typedef struct
{
  uint8_t                Number;            /*!< Specifies the number of the memory attributes to configure.
                                                 This parameter can be a value of @ref CORTEX_MPU_Attributes_Number   */

  uint8_t                Attributes;        /*!< Specifies the memory attributes value. Attributes This parameter
                                                 can be a combination of @ref CORTEX_MPU_Attributes                   */

} MPU_Attributes_InitTypeDef;

#endif

      enum class NVIC_PG {
	PG0 = 0x7,
	PG1 = 0x6,
	PG2 = 0x5,
	PG3 = 0x4,
	PG4 = 0x3
      };

/** @defgroup CORTEX_MPU_HFNMI_PRIVDEF_Control CORTEX MPU HFNMI and PRIVILEGED Access control
  * @{
  */
#define  MPU_HFNMI_PRIVDEF_NONE          0U /*!< Background region access not allowed, MPU disabled for Hardfaults, NMIs, and exception handlers when FAULTMASK=1 */
#define  MPU_HARDFAULT_NMI               2U /*!< Background region access not allowed, MPU enabled for Hardfaults, NMIs, and exception handlers when FAULTMASK=1 */
#define  MPU_PRIVILEGED_DEFAULT          4U /*!< Background region privileged-only access allowed, MPU disabled for Hardfaults, NMIs, and exception handlers when FAULTMASK=1 */
#define  MPU_HFNMI_PRIVDEF               6U /*!< Background region privileged-only access allowed, MPU enabled for Hardfaults, NMIs, and exception handlers when FAULTMASK=1 */
/**
  * @}
  */

      enum class region_perm {
	PRIV_RW = 0,
	ALL_RW = 1,
	PRIV_RO = 2,
	ALL_RO = 3
      };

      enum class region_num {
	R0 = 0,
	R1 = 1,
	R2 = 2,
	R3 = 3,
	R4 = 4,
	R5 = 5,
	R6 = 6,
	R7 = 7,
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
	R8 = 8,
	R9 = 9,
	R10 = 10,
	R11 = 11
#endif /* __ARM_FEATURE_CMSE */
      };

      struct region_def
      {
	bool         en;
	region_num   n;
	uintptr_t    base;
	uintptr_t    limit;
	uint8_t      attridx;
	region_perm  perm;
	bool         noexec;
	bool         shareable;
      };

      struct MPU_CSR
      {
	volatile uint32_t TYPE;                   /*!< Offset: 0x000 (R/ )  MPU Type Register */
	volatile uint32_t CTRL;                   /*!< Offset: 0x004 (R/W)  MPU Control Register */
	volatile uint32_t RNR;                    /*!< Offset: 0x008 (R/W)  MPU Region Number Register */
	volatile uint32_t RBAR;                   /*!< Offset: 0x00C (R/W)  MPU Region Base Address Register */
	volatile uint32_t RLAR;                   /*!< Offset: 0x010 (R/W)  MPU Region Limit Address Register */
	volatile uint32_t RBAR_A1;                /*!< Offset: 0x014 (R/W)  MPU Region Base Address Register Alias 1 */
	volatile uint32_t RLAR_A1;                /*!< Offset: 0x018 (R/W)  MPU Region Limit Address Register Alias 1 */
	volatile uint32_t RBAR_A2;                /*!< Offset: 0x01C (R/W)  MPU Region Base Address Register Alias 2 */
	volatile uint32_t RLAR_A2;                /*!< Offset: 0x020 (R/W)  MPU Region Limit Address Register Alias 2 */
	volatile uint32_t RBAR_A3;                /*!< Offset: 0x024 (R/W)  MPU Region Base Address Register Alias 3 */
	volatile uint32_t RLAR_A3;                /*!< Offset: 0x028 (R/W)  MPU Region Limit Address Register Alias 3 */
        uint32_t RESERVED0[1];
	volatile uint32_t MAIR[2];
      };

      static void *const SCS_BASE = (void *)0xE000E000UL;
      static void *const SCS_BASE_NS = (void *)0xE002E000UL;
      
      class MPU
      {
	static MPU_CSR *const mpu_csr;
	static MPU_CSR *const mpu_csr_ns;

	uint32_t allocated;
	
	void config_region(bool, const region_def &);
      public:
	MPU();

	uint32_t create_region(uintptr_t base, uintptr_t limit,
			       region_perm perm, bool noexec,
			       bool sharable, uint32_t attr,
			       bool en);
      };
    }
  }
}
