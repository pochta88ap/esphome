#include "human_presence_query_button.h"

namespace esphome {
namespace micradar {

void HumanPresenceQueryButton::press_action() { this->parent_->human_presence_query(); }

}  // namespace ld2410
}  // namespace esphome
