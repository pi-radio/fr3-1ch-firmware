/*
 * ltc2668.h
 *
 *  Created on: Feb 12, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CORE_LTC2668_H_
#define APPLICATION_USER_CORE_LTC2668_H_

#ifdef __cplusplus

class LTC2668
{
  uint16_t V2code(double V) { return (uint16_t)(65536.0*(V + 2.5)/5); }

public:
  LTC2668();

  void initialize();

  void setV(int n, double V);
};

extern LTC2668 ltc2668;

extern "C" void ltc2668_program(void);

#else

void ltc2668_program(void);

#endif

#endif /* APPLICATION_USER_CORE_LTC2668_H_ */
