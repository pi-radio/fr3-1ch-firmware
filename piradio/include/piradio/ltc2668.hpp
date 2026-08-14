#pragma once

class LTC2668
{
  uint16_t V2code(double V) { return (uint16_t)(65536.0*(V + 2.5)/5); }

public:
  LTC2668();

  void initialize();

  void setV(int n, double V);
};
