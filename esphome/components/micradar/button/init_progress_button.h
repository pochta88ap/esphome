#pragma once

#include "esphome/components/button/button.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class InitProgressButton : public button::Button, public Parented<MicradarComponent> {
 public:
  InitProgressButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2410
}  // namespace esphome
