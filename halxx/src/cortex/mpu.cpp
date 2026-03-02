#include <cortex/mpu.hpp>

#include <cortex/core.hpp>

using namespace halxx::cortex::mpu;

MPU_CSR *const MPU::mpu_csr = (MPU_CSR *)(SCS_BASE + 0x0D90);
MPU_CSR *const MPU::mpu_csr_ns = (MPU_CSR *)(SCS_BASE_NS + 0x0D90);

MPU::MPU()
{
}

void MPU::config_region(bool nonsec, const region_def &rd)
{
  MPU_CSR *const csr = nonsec ? mpu_csr_ns : mpu_csr;
  
  __DMB();

  csr->RNR = (uint32_t)rd.n;

  /* Disable the Region */
  csr->RLAR &= ~MPU_RLAR_EN_Msk;

  csr->RBAR = (((uint32_t)rd.base                  & 0xFFFFFFE0UL)  |
	       ((uint32_t)rd.shareable             << MPU_RBAR_SH_Pos)  |
	       ((uint32_t)rd.perm                  << MPU_RBAR_AP_Pos)  |
	       ((uint32_t)rd.noexec                << MPU_RBAR_XN_Pos));

  csr->RLAR = (((uint32_t)rd.limit                 & 0xFFFFFFE0UL) |
	       ((uint32_t)rd.attridx               << MPU_RLAR_AttrIndx_Pos) |
	       ((rd.en ? 1 : 0)                    << MPU_RLAR_EN_Pos));
  
}
