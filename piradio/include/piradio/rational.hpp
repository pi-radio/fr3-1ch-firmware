#pragma once

#include <cstdint>

template <typename T>
struct rational
{
  T num;
  T den;

  rational(T n = 0, T d = 0) : num(n), den(d) {}

  rational mediant(const rational &o)
  {
    return rational(num + o.num, den + o.den);
  }

  operator double() const { return (double)num/den; }

  static rational approximate(double d, uint32_t max_denom)
  {
    rational low(0,1), high(1,1), result;
    
    while (true) {
      auto mediant = low.mediant(high);
      
      if (mediant.den > max_denom) {
        if (d - low < high - d) {
          return low;
        } else {
          return high;
        }
      }
      // We're already close enough, so we take the simplest fraction
      if (std::fabs(d - mediant) < 1.0/max_denom) {
        return mediant;
      }
      
      double n1 = d * low.den - low.num;
      double n2 = high.num - d * high.den;
      
      if (d < (double)mediant) {
        int K = std::floor(n2 / n1);
        high.num = high.num + K * low.num;
        high.den = high.den + K * low.den;

        if (std::fabs(d - high) < 1.0/max_denom) {
          return high;
        }
      } else {
        int K = std::floor(n1 / n2);
        low.num = low.num + K * high.num;
        low.den = low.den + K * high.den;
        
        if (std::fabs(d - low) < 1.0/max_denom) {
          return low;
        }
      }      
    }
  }
};


