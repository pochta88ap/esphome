#pragma once

#include "esphome/components/number/number.h"
#include "../micradar.h"

namespace esphome::micradar {

class TrackFrequencyNumber : public number::Number, public Parented<MicradarComponent> {
 public:
  TrackFrequencyNumber() = default;

 protected:
  
  void control(float value) override;
};

}  // namespace esphome::micradar