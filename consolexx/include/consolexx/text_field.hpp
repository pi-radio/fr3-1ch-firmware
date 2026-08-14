#pragma once

#include <consolexx/window.hpp>

namespace consolexx
{
  class text_field : public window {
  public:
    text_field(viewport *parent, const rect &_R);

    int on_input(int c) override;
  };
}

