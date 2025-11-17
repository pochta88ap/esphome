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


#ifdef USE_BUTTON
  
  SUB_BUTTON(query)
  SUB_BUTTON(restart)
#endif

#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(product_model)
  SUB_TEXT_SENSOR(product_id)
  SUB_TEXT_SENSOR(hardware_model)
  SUB_TEXT_SENSOR(firmware_version)
#endif

 public:
  void setup() override;
  void dump_config() override;
  void loop() override;

  void read_all_info();
  void restart_and_read_all_info();

 protected:
  void readline_(int readch);

  uint8_t buffer_pos_ = 0;  // where to resume processing/populating buffer
  uint8_t buffer_data_[MAX_LINE_LENGTH];
  std::string product_model_;
  std::string product_id_;
  std::string hardware_model_;
  std::string firmware_version_;
  
  uint8_t checkDigit_( uint8_t *buf, uint16_t len);
  void issue_data_( uint8_t control, uint8_t command, uint8_t *bytes, uint16_t len );
  void handle_data_();
  
  //issue command functions
  void issue_heartbit_package_query_();
  void issue_module_reset_();
  void issue_product_model_query_();
  void issue_product_id_query_();
  void issue_hardware_model_query_();
  void issue_firmware_version_query_();
  void issue_initialization_progress_query_();
  void issue_enable_human_presence_function_( uint8_t value );
  void issue_human_presence_switch_query_();
  void issue_presence_information_query_();
  void issue_movement_information_query_();
  void issue_body_movement_parameter_query_();
  void issue_track_information_query_();
  void issue_start_OTA_upgrade_( uint32_t firmware_package_size );
  void issue_upgrade_package_transmission_( uint8_t *packet, uint32_t len );
  void issue_stop_OTA_upgrade_( uint8_t value );



};
}
}