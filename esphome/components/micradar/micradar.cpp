#include "micradar.h"

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif


#include "esphome/core/application.h"
#include <cmath>


namespace esphome {
namespace micradar {

    static const char *const TAG = "micradar";

    // Data positions
    static constexpr uint8_t SHIFT_CONTROL_WORD = 0x02;
    static constexpr uint8_t SHIFT_COMMAND_WORD = 0x03;
    static constexpr uint8_t SHIFT_DATA_LENGTH_WORD = 0x04;
    static constexpr uint8_t SHIFT_DATA = 0x06;
    static constexpr uint8_t TRACK_DATA_LENGTH = 11;
    // Contfol words
    static constexpr uint8_t CTRL_SYSTEM_FUNCTIONS = 0x01;
    static constexpr uint8_t CTRL_PRODUCT_INFO = 0x02;
    static constexpr uint8_t CTRL_WORKING_STATUS = 0x05;
    static constexpr uint8_t CTRL_INSTALLATION_METHOD = 0x06;
    static constexpr uint8_t CTRL_HUMAN_PRESENCE_FUNCTION = 0x80;
    static constexpr uint8_t CTRL_TRACK_FUNCTION = 0x82;
    static constexpr uint8_t CTRL_FALL_DETECTION = 0x83;
    static constexpr uint8_t CTRL_OTA = 0x03;
    static constexpr uint8_t CTRL_SHUTDOWN = 0x04;
   

    // Command words
    // System functions
    static constexpr uint8_t CMD_HEARTBIT_PACKAGE_QUERY =0x01;
    static constexpr uint8_t CMD_MODULE_RESET = 0x02;
    // Product_info
    static constexpr uint8_t CMD_PRODUCT_MODEL_QUERY = 0xA1;
    static constexpr uint8_t CMD_PRODUCT_ID_QUERY = 0xA2;
    static constexpr uint8_t CMD_HARDWARE_MODEL_QUERY = 0xA3;
    static constexpr uint8_t CMD_FIRMWARE_VERSION_QUERY = 0xA4;
    // Working status
    static constexpr uint8_t CMD_MESSAGE_OF_INITIALIZATION_COMPLETE = 0x01;
    static constexpr uint8_t CMD_UPLOAD_OF_RADAR_FAILURE = 0x02;
    static constexpr uint8_t CMD_INITIALIZATION_PROGRESS_QUERY = 0x81;
    // Human presence function
    static constexpr uint8_t CMD_ENABLE_DISABLE_HUMAN_PRESENCE_FUNCTION = 0x00;
    static constexpr uint8_t CMD_HUMAN_PRESENCE_INFORMATION_REPORT = 0x01;
    static constexpr uint8_t CMD_MOVEMENT_INFORMATION_REPORT = 0x02;
    static constexpr uint8_t CMD_BODY_MOVEMENT_PARAMETER_REPORT = 0x03;
    static constexpr uint8_t CMD_HUMAN_PRESENCE_SWITCH_QUERY = 0x80;
    static constexpr uint8_t CMD_PRESENCE_INFORMATION_QUERY = 0x81;
    static constexpr uint8_t CMD_MOVEMENT_INFORMATION_QUERY = 0x82;
    static constexpr uint8_t CMD_BODY_MOVEMENT_PARAMETER_QUERY = 0x83;
    static constexpr uint8_t CMD_BODY_MOVEMENT = 0x83;
    // Track function
    static constexpr uint8_t CMD_TRACK_INFORMATION = 0x02;
    static constexpr uint8_t CMD_TRACK_INFORMATION_QUERY = 0x82;
    // OTA
    static constexpr uint8_t CMD_START_OTA_UPGRADE = 0x01;
    static constexpr uint8_t CMD_UPGRADE_PACKAGE_TRANSMITTION = 0x02;
    static constexpr uint8_t CMD_STOP_OTA_UPGRADE = 0x03;

    // Command values
    static constexpr uint8_t CMD_COMPLETED = 0x01;
    static constexpr uint8_t CMD_NOT_COMPLETED = 0x00;

    static constexpr uint8_t CMD_SHUTDOWN_COMPLETED = 0x04;

    static constexpr uint8_t CMD_ENABLE = 0x01;
    static constexpr uint8_t CMD_DISABLE = 0x00;
    static constexpr uint8_t CMD_NON_PRESENCE = 0x00;
    static constexpr uint8_t CMD_PRESENCE = 0x01;

    static constexpr uint8_t CMD_NO = 0x00;
    static constexpr uint8_t CMD_STATIC = 0x01;
    static constexpr uint8_t CMD_ACTIVE = 0x02;

    static constexpr uint8_t CMD_RECEIVED = 0x01;
    static constexpr uint8_t CMD_FAILED_TO_RECEIVE = 0x02;

    static constexpr uint8_t CMD_FIRMWARE_PACKAGE_TRANSMISSION_COMPLETED = 0x01;
    static constexpr uint8_t CMD_FIRMWARE_PACKAGE_TRANSMISSION_NOT_COMPLETED = 0x02;

    struct Uint8ToString {
        const uint8_t value;
        const char *str;
    };

    struct StringToUint8 {
        const char *str;
        const uint8_t value;
    };

    // Helper functions for lookups
    template<size_t N> uint8_t find_uint8(const StringToUint8 (&arr)[N], const char *str) {
    for (const auto &entry : arr) {
     if (strcmp(str, entry.str) == 0)
          return entry.value;
        }
        return 0xFF;  // Not found
    }

    template<size_t N> const char *find_str(const Uint8ToString (&arr)[N], uint8_t value) {
        for (const auto &entry : arr) {
            if (value == entry.value)
            return entry.str;
        }
    return "";  // Not found
    }

    enum InitializationProgress : uint8_t {
        COMPLETED = 0x01,
        NOT_COMPLETED = 0x00
    };

    enum HumanPresence : uint8_t {
        NOT_PRESENCE = 0x00,
        PRESENCE = 0x01
    };

    enum MovementSate : uint8_t {
        NO = 0x00,
        STATIC = 0x01,
        ACTIVE = 0X02
    };

    enum SwitchState : uint8_t {
        ENABLED = 0x01,
        DISABLED = 0x00
    };



    

    constexpr Uint8ToString  SWITCH_BY_UINT[] {
        {ENABLED, "Enabled"},
        {DISABLED, "Disabled"}  
    };

    constexpr Uint8ToString  MOVEMNENT_STATE_BY_UINT[] {
        {NO, "No"},
        {STATIC, "Static"},
        {ACTIVE, "Active"}    
    };

    constexpr Uint8ToString  HUMAN_PRESENCE_BY_UINT[] {
        {NOT_PRESENCE, "Not Presence"},
        {PRESENCE, "Presence"}  
    };
    
    constexpr Uint8ToString  INIT_STATE_BY_UINT[] {
        {COMPLETED, "Completed"},
        {NOT_COMPLETED, "Not Completed"}  
    };

    
    static constexpr uint8_t HEADER_TAIL_SIZE = 2;

    static constexpr uint8_t DATA_FRAME_HEADER[HEADER_TAIL_SIZE] = { 0x53, 0x59 };
    static constexpr uint8_t DATA_FRAME_TAIL[HEADER_TAIL_SIZE] = { 0x54, 0x43 };

    static inline int two_byte_to_int(char firstbyte, char secondbyte) { return (int16_t) (firstbyte << 8) + secondbyte; }
    static inline int two_byte_to_signed_int(char firstbyte, char secondbyte) { return (int16_t) ( 0x80 & firstbyte ? -1 : 1) * (((0x7f & firstbyte) << 8) + secondbyte); }
    static inline uint8_t lobyte( uint16_t word ) { return (uint8_t) 0xff & word; }
    static inline uint8_t hibyte( uint16_t word ) { return (uint8_t) ( ( 0xff00 & word )>>8 ); }

    static inline bool validate_header_footer(const uint8_t *header_tail, const uint8_t *buffer) {
        return std::memcmp(header_tail, buffer, HEADER_TAIL_SIZE) == 0;
    }

    void MicradarComponent::dump_config() {
#ifdef USE_BUTTON
        ESP_LOGCONFIG(TAG, "Buttons:");
        
        LOG_BUTTON("  ", "Query", this->query_button_);
        LOG_BUTTON("  ", "Restart", this->restart_button_);
#endif

#ifdef USE_TEXT_SENSOR
  ESP_LOGCONFIG(TAG, "Text Sensors:");
  LOG_TEXT_SENSOR("  ", "product Model", this->product_model_text_sensor_);
  LOG_TEXT_SENSOR("  ", "Product Id", this->product_id_text_sensor_);
  LOG_TEXT_SENSOR("  ", "Hardware Model", this->hardware_model_text_sensor_);
  LOG_TEXT_SENSOR("  ", "Firmware Version", this->firmware_version_text_sensor_);
#endif

#ifdef USE_BINARY_SENSOR
  ESP_LOGCONFIG(TAG, "Binary Sensors:");
  LOG_BINARY_SENSOR("  ", "Target", this->target_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "MovingTarget", this->moving_target_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "StillTarget", this->still_target_binary_sensor_);
#endif

    }

    void MicradarComponent::setup() { 
        ESP_LOGCONFIG(TAG, "Running setup");
        this->read_all_info();
     }

    void MicradarComponent::read_all_info(){
        this->issue_product_model_query_();
        this->issue_product_id_query_();
        this->issue_hardware_model_query_();
        this->issue_firmware_version_query_();
        this->issue_human_presence_switch_query_();
        this->human_presence_query();
        
    }

    void MicradarComponent::restart_and_read_all_info(){
        this->issue_module_reset_();
        this->read_all_info();
    }

    void MicradarComponent::human_presence_query(){
        this->issue_presence_information_query_();
        this->issue_movement_information_query_();
        this->issue_body_movement_parameter_query_();
    }

     void MicradarComponent::init_progress_query(){
        this->issue_initialization_progress_query_();
     }

     void MicradarComponent::track_query(){
        this->issue_track_information_query_();
     }

     void MicradarComponent::set_human_presence_function( bool state){
        this->issue_enable_human_presence_function_( state? 1:0 );
     }


    void MicradarComponent::loop() {
       // Read all available bytes in batches to reduce UART call overhead.
  size_t avail = this->available();
  uint8_t buf[MAX_LINE_LENGTH];
  while (avail > 0) {
    size_t to_read = std::min(avail, sizeof(buf));
    if (!this->read_array(buf, to_read)) {
      break;
    }
    avail -= to_read;

    for (size_t i = 0; i < to_read; i++) {
      this->readline_(buf[i]);
    }
  }
    }

    void MicradarComponent::readline_(int readch ) {
        if (readch < 0) {
           
            return;  // No data available
        }
        if (this->buffer_pos_ < MAX_LINE_LENGTH - 1) {
            this->buffer_data_[this->buffer_pos_++] = readch;
            this->buffer_data_[this->buffer_pos_] = 0;
        } else {
        // We should never get here, but just in case...
            ESP_LOGW(TAG, "Max command length exceeded; ignoring");
            this->buffer_pos_ = 0;
        }
        if (this->buffer_pos_ < HEADER_TAIL_SIZE) {
            return;  // Not enough data to process yet
        }
        if (micradar::validate_header_footer(DATA_FRAME_TAIL, &this->buffer_data_[this->buffer_pos_ - HEADER_TAIL_SIZE])) {
            uint8_t sum = this->checkDigit_(this->buffer_data_, this->buffer_pos_- HEADER_TAIL_SIZE - 1 );
            if( sum != this->buffer_data_[this->buffer_pos_ - HEADER_TAIL_SIZE -1 ] ){
                ESP_LOGW(TAG, "error receiving data block; ignoring");
                this->buffer_pos_= 0;
                return;
            }
             ESP_LOGD(TAG, "Handling Data: %s", format_hex_pretty(this->buffer_data_, this->buffer_pos_).c_str());
            this->handle_data_();
            this->buffer_pos_ = 0;  // Reset position index for next message
        }

    }
    uint8_t MicradarComponent::checkDigit_( uint8_t *buf, uint16_t len ){
        uint8_t i;
        uint16_t sum(0);
        for (i=0; i< len ; i++ ){
            sum+= buf[i];
        }
        return (uint8_t) 0xff & sum; 
    }

    void MicradarComponent::issue_data_( uint8_t control, uint8_t command, uint8_t *bytes, uint16_t len ){
        ESP_LOGD(TAG, "Sending CONTROL %02X COMMAND %02X", control, command);
        uint16_t sum = DATA_FRAME_HEADER[0] + DATA_FRAME_HEADER[1] + control + command + hibyte( len )+ lobyte( len ) ;
        for( int i=0; i< len; i++) sum += bytes[i];
        uint8_t check = lobyte( sum );
        this->write_array( DATA_FRAME_HEADER, sizeof( DATA_FRAME_HEADER ) );
        uint8_t ctrl_bytes[4] = { control, command, hibyte( len ), lobyte ( len ) };
        this->write_array( ctrl_bytes, sizeof( ctrl_bytes ));
        this->write_array( bytes, len  );
        this->write_array( &check, sizeof( check ));
        this->write_array( DATA_FRAME_TAIL, sizeof( DATA_FRAME_TAIL ) );
    }
    
    void MicradarComponent::handle_data_(){

        uint8_t controlWord = this->buffer_data_[SHIFT_CONTROL_WORD];
        uint8_t commandWord = this->buffer_data_[SHIFT_COMMAND_WORD];
        uint16_t dataLength  = two_byte_to_int( this->buffer_data_[SHIFT_DATA_LENGTH_WORD], this->buffer_data_[SHIFT_DATA_LENGTH_WORD+1]);
        const char *init_state;
        const char *human_presence;
        const char *movement_info;
    //    ESP_LOGD(TAG, "control word %02X command %02X", controlWord, commandWord);
        switch( controlWord ){
            case CTRL_SYSTEM_FUNCTIONS:
                switch( commandWord ){
                    case CMD_HEARTBIT_PACKAGE_QUERY:
                        break;
                    case CMD_MODULE_RESET:
                        break;
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_PRODUCT_INFO:
                switch( commandWord ){
                    case CMD_PRODUCT_MODEL_QUERY:
#ifdef USE_TEXT_SENSOR
                        if (this->product_model_text_sensor_ != nullptr) {
                            product_model_ = ((char*)(this->buffer_data_+6));
                            this->product_model_text_sensor_->publish_state(product_model_);
                            ESP_LOGD(TAG, "product model: %s", product_model_.c_str());
                        }
#endif
                        break;
                    case CMD_PRODUCT_ID_QUERY:
#ifdef USE_TEXT_SENSOR
                        if (this->product_id_text_sensor_ != nullptr) {
                            product_id_ = ((char*)(this->buffer_data_+6));
                            this->product_id_text_sensor_->publish_state(product_id_);
                            ESP_LOGD(TAG, "product id: %s", product_id_.c_str());
                        }
#endif
                        break;
                    case CMD_HARDWARE_MODEL_QUERY:
#ifdef USE_TEXT_SENSOR
                        if (this->hardware_model_text_sensor_ != nullptr) {
                            hardware_model_ = ((char*)(this->buffer_data_+SHIFT_DATA));
                            this->hardware_model_text_sensor_->publish_state(hardware_model_);
                            ESP_LOGD(TAG, "hardware model: %s", hardware_model_.c_str());
                        }
#endif
                        break;
                    case CMD_FIRMWARE_VERSION_QUERY:
#ifdef USE_TEXT_SENSOR
                        if (this->firmware_version_text_sensor_ != nullptr) {
                            firmware_version_ = ((char*)(this->buffer_data_+SHIFT_DATA));
                            this->firmware_version_text_sensor_->publish_state(firmware_version_);
                            ESP_LOGD(TAG, "firmware_version: %s", firmware_version_.c_str());
                        }
#endif
                        break;
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_WORKING_STATUS:
                switch( commandWord ){
                    case CMD_MESSAGE_OF_INITIALIZATION_COMPLETE:
                        init_state = find_str( INIT_STATE_BY_UINT, this->buffer_data_[SHIFT_DATA]);
                        ESP_LOGD(TAG, "Initialisation progress: %s", init_state );
                        break;
                    case CMD_UPLOAD_OF_RADAR_FAILURE:
                        break;
                    case CMD_INITIALIZATION_PROGRESS_QUERY:
                        break;
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_INSTALLATION_METHOD:
                switch( commandWord ){
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_HUMAN_PRESENCE_FUNCTION:
                switch( commandWord ){
                    case CMD_ENABLE_DISABLE_HUMAN_PRESENCE_FUNCTION:
                        break;
                    case CMD_HUMAN_PRESENCE_INFORMATION_REPORT:
                    case CMD_PRESENCE_INFORMATION_QUERY:
                        if (this->target_binary_sensor_ != nullptr) {
                            this->target_binary_sensor_->publish_state(this->buffer_data_[SHIFT_DATA] != 0);
                            if( this->buffer_data_[SHIFT_DATA] == 0 )
                            for( uint8_t pos =0; pos < MAX_TARGETS; pos++){
                                set_sensors_unknown( pos);
                            }
                        }
                        human_presence = find_str( HUMAN_PRESENCE_BY_UINT, this->buffer_data_[SHIFT_DATA]);
                        ESP_LOGD(TAG, "Human presence: %s", human_presence );
                        break;
                    case CMD_MOVEMENT_INFORMATION_REPORT:
                    case CMD_MOVEMENT_INFORMATION_QUERY:
                        if (this->moving_target_binary_sensor_ != nullptr) {
                            this->moving_target_binary_sensor_->publish_state(this->buffer_data_[SHIFT_DATA] == 0x02);
                        }
                        if (this->still_target_binary_sensor_ != nullptr) {
                            this->still_target_binary_sensor_->publish_state(this->buffer_data_[SHIFT_DATA] == 0x01);
                        }

                        movement_info = find_str( MOVEMNENT_STATE_BY_UINT, this->buffer_data_[SHIFT_DATA]);
                        ESP_LOGD(TAG, "Movement Info: %s", movement_info );
                        break;
                    case CMD_BODY_MOVEMENT_PARAMETER_REPORT:
                    case CMD_BODY_MOVEMENT_PARAMETER_QUERY:
#ifdef USE_SENSOR
                            SAFE_PUBLISH_SENSOR(this->moving_target_energy_sensor_, this->buffer_data_[SHIFT_DATA]);
#endif
                        ESP_LOGD(TAG, "Movement parameter: %d", this->buffer_data_[SHIFT_DATA] );
                        break;
                    case CMD_HUMAN_PRESENCE_SWITCH_QUERY:
                        if( this->human_presence_function_switch_ != nullptr) {
                             this->human_presence_function_switch_->publish_state(this->buffer_data_[SHIFT_DATA] != 0 );
                        }
                        break;
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_TRACK_FUNCTION:
                switch( commandWord ){
                    case CMD_TRACK_INFORMATION:
                    //    break;
                    case CMD_TRACK_INFORMATION_QUERY:
                    {
                        uint16_t px, py, d;
                        float angle = 0;
                        num_targets_ = dataLength/TRACK_DATA_LENGTH;
                        ESP_LOGI(TAG, "targets: %d ", num_targets_ );
                        for( int pos = 0; pos < MAX_TARGETS; pos++ ){
                            if( pos >=num_targets_) {
                               set_sensors_unknown(pos);
                                continue;
                            }
                            targets_[pos].index = buffer_data_[6 + pos * TRACK_DATA_LENGTH];
                            targets_[pos].size = buffer_data_[7 + pos * TRACK_DATA_LENGTH];
                            targets_[pos].characteristics = buffer_data_[8 + pos * TRACK_DATA_LENGTH];
                            px = targets_[pos].x;
                            py = targets_[pos].y;
                            targets_[pos].x = two_byte_to_signed_int(buffer_data_[9 + pos * TRACK_DATA_LENGTH], 
                                buffer_data_[10 + pos *TRACK_DATA_LENGTH]);
                            targets_[pos].y = two_byte_to_signed_int(buffer_data_[11 + pos * TRACK_DATA_LENGTH], 
                                buffer_data_[12 + pos * TRACK_DATA_LENGTH]);
                            d = sqrtf( targets_[pos].x * targets_[pos].x + targets_[pos].y * targets_[pos].y );
                             
                            targets_[pos].height = two_byte_to_signed_int(buffer_data_[13 + pos *TRACK_DATA_LENGTH],
                                buffer_data_[14 + pos * TRACK_DATA_LENGTH]);
                            if(d ==  targets_[pos].distance)
                                targets_[pos].velocity = 0;
                            else
                                if( d< targets_[pos].distance )
                                    targets_[pos].velocity = 1;
                            else
                                targets_[pos].velocity = -1;
                            angle = atan2f(static_cast<float>(-px), static_cast<float>(py)) * (180.0f / std::numbers::pi_v<float>);
                            //two_byte_to_signed_int(buffer_data_[15 + pos *TRACK_DATA_LENGTH], 
                            //    buffer_data_[16 + pos * TRACK_DATA_LENGTH]);
                            targets_[pos].distance = d;
                            SAFE_PUBLISH_SENSOR(this->x_coord_sensors_[targets_[pos].index-1], targets_[pos].x);
                            SAFE_PUBLISH_SENSOR(this->y_coord_sensors_[targets_[pos].index-1], targets_[pos].y);
                            SAFE_PUBLISH_SENSOR(this->dist_sensors_[targets_[pos].index-1], targets_[pos].distance);
                            SAFE_PUBLISH_SENSOR(this->move_energy_sensors_[targets_[pos].index-1], targets_[pos].size); 
                            SAFE_PUBLISH_SENSOR(this->angle_sensors_[targets_[pos].index-1], angle); 
                            ESP_LOGD(TAG, "Tracking info: Index: %d size: %d characteristics: %d x: %d \n y: %d height: %d velocity: %d distance: %d angle: %f", 
                                    targets_[pos].index, targets_[pos].size, targets_[pos].characteristics, 
                                    targets_[pos].x, targets_[pos].y, targets_[pos].height, targets_[pos].velocity, targets_[pos].distance, angle);
                        }
                    }/*
                        for( int pos = num_targets_; pos < MAX_TARGETS; pos++ ){
                            targets_[pos].index = pos + 1;
                            targets_[pos].size = 0;
                            targets_[pos].characteristics = 0;
                            targets_[pos].x = 0;
                            targets_[pos].y = 0;
                            targets_[pos].height = 0;
                            targets_[pos].velocity = 0;
                            if ((this->x_coord_sensors_[pos]) != nullptr) { (this->x_coord_sensors_[pos])->publish_state_if_not_dup(targets_[pos].x); }
                            if ((this->y_coord_sensors_[pos]) != nullptr) { (this->y_coord_sensors_[pos])->publish_state_if_not_dup(targets_[pos].y); }
                            if ((this->move_energy_sensors_[pos]) != nullptr) { (this->move_energy_sensors_[pos])->publish_state_if_not_dup(targets_[pos].size); }
                           
                        }*/
                    
                        break;
                    case CMD_INITIALIZATION_PROGRESS_QUERY:
                        break;
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_FALL_DETECTION:
                switch( commandWord ){
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_OTA:
                switch( commandWord ){
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            case CTRL_SHUTDOWN:
                switch( commandWord ){
                    case CMD_SHUTDOWN_COMPLETED:
                        ESP_LOGD(TAG, "shutdown completed");
                        break;
                    default:
                        ESP_LOGW(TAG, "control word %02X unknown command %02X", controlWord, commandWord);
                        break;
                }
                break;
            default:
                ESP_LOGW(TAG, "unknown control word %02X", controlWord);
                break;
        }        
    }
    void MicradarComponent::issue_heartbit_package_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_SYSTEM_FUNCTIONS, CMD_HEARTBIT_PACKAGE_QUERY, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_module_reset_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_SYSTEM_FUNCTIONS, CMD_MODULE_RESET, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_product_model_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_PRODUCT_INFO, CMD_PRODUCT_MODEL_QUERY, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_product_id_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_PRODUCT_INFO, CMD_PRODUCT_ID_QUERY, &buf, sizeof( buf) );
    }
    
    void MicradarComponent::issue_hardware_model_query_(){
         uint8_t buf = 0;
        this->issue_data_( CTRL_PRODUCT_INFO, CMD_HARDWARE_MODEL_QUERY, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_firmware_version_query_(){
         uint8_t buf = 0;
        this->issue_data_( CTRL_PRODUCT_INFO, CMD_FIRMWARE_VERSION_QUERY, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_initialization_progress_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_WORKING_STATUS, CMD_INITIALIZATION_PROGRESS_QUERY, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_enable_human_presence_function_( uint8_t state ){
         
        this->issue_data_( CTRL_HUMAN_PRESENCE_FUNCTION, CMD_ENABLE_DISABLE_HUMAN_PRESENCE_FUNCTION, &state, 1 );
    }

    void MicradarComponent::issue_human_presence_switch_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_HUMAN_PRESENCE_FUNCTION, CMD_HUMAN_PRESENCE_SWITCH_QUERY, &buf, sizeof( buf) );
    }

    void MicradarComponent::issue_presence_information_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_HUMAN_PRESENCE_FUNCTION, CMD_PRESENCE_INFORMATION_QUERY, &buf, sizeof( buf) );
    }
    void MicradarComponent::issue_movement_information_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_HUMAN_PRESENCE_FUNCTION, CMD_MOVEMENT_INFORMATION_QUERY, &buf, sizeof( buf) );
    }
    void MicradarComponent::issue_body_movement_parameter_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_HUMAN_PRESENCE_FUNCTION, CMD_BODY_MOVEMENT_PARAMETER_QUERY, &buf, sizeof( buf) );
    }
    void MicradarComponent::issue_track_information_query_(){
        uint8_t buf = 0;
        this->issue_data_( CTRL_TRACK_FUNCTION, CMD_TRACK_INFORMATION_QUERY, &buf, sizeof( buf) );
    }
   
    void MicradarComponent::issue_stop_OTA_upgrade_( uint8_t value ){
        this->issue_data_( CTRL_OTA, CMD_STOP_OTA_UPGRADE, &value, sizeof( value) );
    }

#ifdef USE_SENSOR
// These could leak memory, but they are only set once prior to 'setup()' and should never be used again.
    void MicradarComponent::set_move_energy_sensor(uint8_t target, sensor::Sensor *s) {
  this->move_energy_sensors_[target].set_sensor(s);
}

void MicradarComponent::set_x_coord_sensor(uint8_t target, sensor::Sensor *s) {
  this->x_coord_sensors_[target].set_sensor(s);
}
void MicradarComponent::set_y_coord_sensor(uint8_t target, sensor::Sensor *s) {
  this->y_coord_sensors_[target].set_sensor(s);
}
void MicradarComponent::set_dist_sensor(uint8_t target, sensor::Sensor *s) {
  this->dist_sensors_[target].set_sensor(s);
}
void MicradarComponent::set_velocity_sensor(uint8_t target, sensor::Sensor *s) {
  this->velocity_sensors_[target].set_sensor(s);
}
void MicradarComponent::set_angle_sensor(uint8_t target, sensor::Sensor *s) {
  this->angle_sensors_[target].set_sensor(s);
}
void MicradarComponent::set_sensors_unknown( uint8_t tgt ){
     SAFE_PUBLISH_SENSOR_UNKNOWN(this->x_coord_sensors_[pos]);
                                SAFE_PUBLISH_SENSOR_UNKNOWN(this->y_coord_sensors_[tgt]);
                                SAFE_PUBLISH_SENSOR_UNKNOWN(this->dist_sensors_[tgt]);
                                SAFE_PUBLISH_SENSOR_UNKNOWN(this->move_energy_sensors_[tgt]); 
                                SAFE_PUBLISH_SENSOR_UNKNOWN(this->angle_sensors_[tgt]);
                                SAFE_PUBLISH_SENSOR_UNKNOWN(this->velocity_sensors_[tgt]); 
}
#endif
}




}