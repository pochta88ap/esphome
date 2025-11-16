#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
//#include "esphome/components/ld24xx/ld24xx.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#include <array>

namespace esphome {
namespace micradar {

class MicradarComponent : public Component, public uart::UARTDevice {
static constexpr uint8_t MAX_LINE_LENGTH = 46;  // Max characters for serial buffer
 public:
  void setup() override;
  void dump_config() override;
  void loop() override;
 protected:
  void readline_(int readch);

  uint8_t buffer_pos_ = 0;  // where to resume processing/populating buffer
  uint8_t buffer_data_[MAX_LINE_LENGTH];
  uint8_t issue_data_[MAX_LINE_LENGTH];
  uint8_t checkDigit_( uint8_t *buf, uint16_t len);
  void issue_data_( uint8_t control, uint8_t command, uint8_t *bytes, uint16_t len );
  void handle_data_();


};
}
}