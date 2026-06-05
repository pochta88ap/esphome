#include "track_frequency_number.h"

namespace esphome::micradar {


void TrackFrequencyNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_track_frequency();
}

}  // namespace esphome::micradar