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
#include "esphome/components/ld24xx/ld24xx.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#include <array>

namespace esphome {
namespace micradar {
using namespace ld24xx;
struct Target {
        uint8_t index;
        uint8_t size;
        uint8_t characteristics;
        int16_t x;
        int16_t y;
        int16_t height;
        float_t velocity;
        int16_t distance;
        uint64_t time_us;
    };


class MicradarComponent : public Component, public uart::UARTDevice {
static constexpr uint8_t MAX_LINE_LENGTH = 100;  // Max characters for serial buffer
static constexpr uint8_t MAX_TARGETS = 3;


#ifdef USE_BUTTON
  
  SUB_BUTTON(query)
  SUB_BUTTON(restart)
  SUB_BUTTON(init_progress)
  SUB_BUTTON(human_query)
  SUB_BUTTON(track_query)
 // SUB_BUTTON(track_query)
#endif

#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(product_model)
  SUB_TEXT_SENSOR(product_id)
  SUB_TEXT_SENSOR(hardware_model)
  SUB_TEXT_SENSOR(firmware_version)
  SUB_TEXT_SENSOR(human_presence_information)
  SUB_TEXT_SENSOR(movement_information)
#endif

#ifdef USE_SWITCH
  SUB_SWITCH(human_presence_function)
#endif

#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(moving_target)
  SUB_BINARY_SENSOR(still_target)
  SUB_BINARY_SENSOR(target)
#endif

#ifdef USE_SENSOR
  
  SUB_SENSOR_WITH_DEDUP(moving_target_energy, uint8_t)
  
#endif

 public:
  void setup() override;
  void dump_config() override;
  void loop() override;

  void read_all_info();
  void restart_and_read_all_info();
  void human_presence_query();
  void init_progress_query();
  void track_query();
  void set_human_presence_function( bool );
#ifdef USE_SENSOR
  void set_x_coord_sensor(uint8_t target, sensor::Sensor *s);
  void set_y_coord_sensor(uint8_t target, sensor::Sensor *s);
  void set_dist_sensor(uint8_t target, sensor::Sensor *s);
  void set_velocity_sensor(uint8_t target, sensor::Sensor *s);
  void set_angle_sensor(uint8_t target, sensor::Sensor *s);
  void set_move_energy_sensor(uint8_t target, sensor::Sensor *s);
  void set_sensors_unknown( uint8_t tgt );
#endif
 protected:
  void readline_(int readch);

  uint8_t buffer_pos_ = 0;  // where to resume processing/populating buffer
  uint8_t buffer_data_[MAX_LINE_LENGTH];
  std::string product_model_;
  std::string product_id_;
  std::string hardware_model_;
  std::string firmware_version_;
  uint8_t num_targets_;
  Target targets_[MAX_TARGETS];
  uint64_t time_;
  uint8_t human_presence_ = 0;
  
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


#ifdef USE_SENSOR
  std::array<SensorWithDedup<int16_t> , MAX_TARGETS> x_coord_sensors_{};
  std::array<SensorWithDedup<int16_t> , MAX_TARGETS> y_coord_sensors_{};
  std::array<SensorWithDedup<int16_t> , MAX_TARGETS> dist_sensors_{};
  std::array<SensorWithDedup<int16_t> , MAX_TARGETS> velocity_sensors_{};
  std::array<SensorWithDedup<float_t> , MAX_TARGETS> angle_sensors_{};
  std::array<SensorWithDedup<uint8_t>   , MAX_TARGETS> move_energy_sensors_{};
#endif
};
}
}
