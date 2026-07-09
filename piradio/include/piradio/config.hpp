#pragma once

#include <memory>
#include <cstring>
#include <cassert>

#include <threadxx/config_data.hpp>

namespace piradio
{
  namespace config
  {
    struct board_model : public TXX::config_data::tlv<board_model>  {
      static constexpr uint16_t TAG = 0x1002;

      uint16_t revision;
      uint16_t length;
      uint8_t  model[32];

      board_model() : revision(-1), length(0) {

      }

      board_model(const std::string &s, uint16_t rev) : revision(rev) {
        assert(s.size() <= 32);

        std::memset(model, 0, sizeof(model));
        length = s.size();
        std::memcpy(model, s.c_str(), length);
      }
    };

    struct board_serial : public TXX::config_data::tlv<board_serial>  {
      static constexpr uint16_t TAG = 0x1003;
      uint8_t  serno[32];
    };

    extern std::vector<std::string> board_models;
  };
};
