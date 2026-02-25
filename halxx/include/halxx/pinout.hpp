#pragma once

#include <initializer_list>

namespace util
{
  template <class K, class V>
  struct entry {
    K k;
    V v;
  };

  template <class K, class V>
  struct ctmap : public std::initializer_list<entry<K, V> >
  {
    constexpr auto  find (K k) const
    {
      for (auto e : *this)  {
        if (e.k == k)
          return std::make_pair(true, e.v);
      }
      return std::make_pair (false, V{});
    }
    
    constexpr V  at (K k) const
    {  
        auto lookup = find (k);
        if (not lookup.first)  {  throw std::out_of_range ("key not found"); } 
        return lookup.second;
    }
  };

  constexpr ctmap<std::string_view, std::string_view> test2 {{
      { "A", "AA" },
      { "B", "BB" }
    }};

  static constexpr auto a = test2.at("A");

  
};

namespace pinout
{
  struct gpio_pin_id
  {
    int bank;
    int pin;
  };
  
  struct gpio_af
  {
    std::string_view function;
    std::string_view pin;
  };

  constexpr gpio_af af_none { "", "" };
  
  constexpr util::ctmap<gpio_pin_id, std::array<gpio_af, 8> > afmap {{
      {{ 0, 0 }, { gpio_af { "", "" }, gpio_af { "TIM2", "CH1" }, gpio_af { "TIM5", "CH1" }, gpio_af { "TIM8", "ETR" }, gpio_af { "TIM15", "BKIN" }, gpio_af { "SPI6", "NSS" }, gpio_af { "SPI3", "RDY" }, gpio_af { "USART2", "CTS" } }}
    }};
};
