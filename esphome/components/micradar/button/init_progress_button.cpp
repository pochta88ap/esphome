#include "init_progress_button.h"

namespace esphome {
namespace micradar {

void InitProgressButton::press_action() { this->parent_->init_progress_query(); }

}  // namespace ld2410
}  // namespace esphome
