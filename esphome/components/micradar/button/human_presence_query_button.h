#pragma once

#include "esphome/components/button/button.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class HumanPresenceQueryButton : public button::Button, public Parented<MicradarComponent> {
 public:
  HumanPresenceQueryButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2410
}  // namespace esphome
