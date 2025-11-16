#include "query_button.h"

namespace esphome {
namespace micradar {

void QueryButton::press_action() { this->parent_->read_all_info(); }

}  // namespace ld2410
}  // namespace esphome
