#include "track_query_button.h"

namespace esphome {
namespace micradar {

void TrackQueryButton::press_action() { this->parent_->track_query(); }

}  // namespace ld2410
}  // namespace esphome
