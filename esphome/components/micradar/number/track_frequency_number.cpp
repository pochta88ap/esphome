#include "track_frequency_number.h"

namespace esphome::micradar {

TrackFrequencyNumber::TrackFrequencyNumber(float_t frequency) : frequency_(frequency) {}

void TrackFrequencyNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_track_frequency(this->frequency_);
}

}  // namespace esphome::micradar