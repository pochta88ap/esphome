#pragma once

#include "esphome/components/number/number.h"
#include "../micradar.h"

namespace esphome::micradar {

class GateThresholdNumber : public number::Number, public Parented<MicradarComponent> {
 public:
  TrackFrequencyNumber(float_t frequency);

 protected:
  float_t frequency_;
  void control(float value) override;
};

}  // namespace esphome::micradar