#pragma once

#include "esphome/components/button/button.h"
#include "../micradar.h"

namespace esphome {
namespace micradar {

class TrackQueryButton : public button::Button, public Parented<MicradarComponent> {
 public:
  TrackQueryButton() = default;

 protected:
  void press_action() override;
};

}  // namespace ld2410
}  // namespace esphome
