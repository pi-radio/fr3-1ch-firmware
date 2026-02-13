/*
 * armexceptions.h
 *
 *  Created on: Feb 12, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CORE_ARMEXCEPTIONS_H_
#define APPLICATION_USER_CORE_ARMEXCEPTIONS_H_

#include <string.h>

struct state_context_base {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R12;
  uint32_t LR;
  uint32_t PC;
  uint32_t xPSR;
} __attribute__ ((packed));

struct state_context_fp {
  uint32_t reserved;
  uint32_t FPSCR;
  uint32_t S15;
  uint32_t S14;
  uint32_t S13;
  uint32_t S12;
  uint32_t S11;
  uint32_t S10;
  uint32_t S9;
  uint32_t S8;
  uint32_t S7;
  uint32_t S6;
  uint32_t S5;
  uint32_t S4;
  uint32_t S3;
  uint32_t S2;
  uint32_t S1;
  uint32_t S0;
} __attribute__ ((packed));

#define EXCEPTION_HARD_FAULT    0xAD01FFCB
#define EXCEPTION_DEBUG         0xDEADBEEF
#define EXCEPTION_NMI           0xFEEDF00D
#define EXCEPTION_MEM_MANAGE    0xCAFECAFE
#define EXCEPTION_BUS_FAULT     0x99998888
#define EXCEPTION_USAGE_FAULT   0x45888277
#define EXCEPTION_ERROR_HANDLER 0xF00DA551

typedef struct exception_info {
  uint32_t exception_type;
  uint32_t CFSR;
  uint32_t HFSR;
  uint32_t SHCSR;
  uint32_t VTOR;
  uint32_t excSP;
  uint32_t excLR;
  uint32_t SP;
  uint32_t SP1;
  uint32_t CONTROL;
  uint32_t excbase;
  struct state_context_base base_ctx;
  struct state_context_fp fp_ctx;
} exception_info_t;

exception_info_t *get_exception_info();
void clear_exception(void);


#endif /* APPLICATION_USER_CORE_ARMEXCEPTIONS_H_ */
