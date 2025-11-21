#pragma once

#include "esphome/components/switch/switch.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class HumanPresenceFunctionSwitch : public switch_::Switch, public Parented<MicradarComponent> {
 public:
  HumanPresenceFunctionSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace ld2410
}  // namespace esphome