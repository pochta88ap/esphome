#include "human_presence_function_switch.h"

namespace esphome {
namespace micradar {

void HumanPresenceFunctionSwitch::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_human_presence_function(state);
}

}  // namespace ld2410
}  // namespace esphome