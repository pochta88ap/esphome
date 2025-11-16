#include "micradar.h"


#include "esphome/core/application.h"


namespace esphome {
namespace micradar {

    static const char *const TAG = "micradar";

    // Data positions
    static constexpr uint8_t SHIFT_CONTROL_WORD = 0x02;
    static constexpr uint8_t SHIFT_COMMAND_WORD = 0x03;
    static constexpr uint8_t SHIFT_DATA_LENGTH_WORD = 0x04;
    static constexpr uint8_t SHIFT_DATA = 0x05;
    // Contfol words
    static constexpr uint8_t CTRL_SYSTEM_FUNCTIONS = 0x01;
    static constexpr uint8_t CTRL_PRODUCT_INFO = 0x02;
    static constexpr uint8_t CTRL_WORKING_STATUS = 0x05;
    static constexpr uint8_t CTRL_INSTALLATION_METHOD = 0x06;
    static constexpr uint8_t CTRL_HUMAN_PRESENCE_FUNCTION = 0x80;
    static constexpr uint8_t CTRL_TRACK_FUNCTION = 0x82;
    static constexpr uint8_t CTRL_FALL_DETECTION = 0x83;
    static constexpr uint8_t CTRL_OTA = 0x03;
   

    // Command words
    // System functions
    static constexpr uint8_t CMD_HEARTBIT_PACKAGE =0x01;
    static constexpr uint8_t CMD_MODULE_RESET = 0x02;
    // Product_info
    static constexpr uint8_t CMD_PRODUCT_MODEL_QUERY = 0xA1;
    static constexpr uint8_t CMD_PRODUCT_ID_QUERY = 0xA2;
    static constexpr uint8_t CMD_HARDWARE_MODEL_ID = 0xA3;
    static constexpr uint8_t CMD_FIRMWARE_VERSION_ID = 0xA4;
    // Working status
    static constexpr uint8_t CMD_MESSAGE_OF_INITIALIZATION_COMPLETE = 0x01;
    static constexpr uint8_t CMD_UPLOAD_OF_RADAR_FAILURE = 0x02;
    static constexpr uint8_t CMD_INITIALIZATION_PROGRESS_QUERY = 0x81;
    // Human presence function
    static constexpr uint8_t CMD_ENABLE_DISABLE_HUMAN = 0x00;
    static constexpr uint8_t CMD_HUMAN_PRESENCE_INFORMATION_REPORT = 0x01;
    static constexpr uint8_t CMD_MOVEMENT_INFORMATION_REPORT = 0x02;
    static constexpr uint8_t CMD_BODY_MOVEMENT_PARAMETER_REPORT = 0x03;
    static constexpr uint8_t CMD_HUMAN_PRESENCE_SWITCH_QUERY = 0x80;
    static constexpr uint8_t CMD_PRESENCE_INFORMATION_QUERY = 0x81;
    static constexpr uint8_t CMD_MOVEMENT_INFORMATION_QUERY = 0x82;
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


    static constexpr uint8_t HEADER_TAIL_SIZE = 2;

    static constexpr uint8_t DATA_FRAME_HEADER[HEADER_TAIL_SIZE] = { 0x53, 0x59 };
    static constexpr uint8_t DATA_FRAME_TAIL[HEADER_TAIL_SIZE] = { 0x54, 0x43 };

    static inline int two_byte_to_int(char firstbyte, char secondbyte) { return (int16_t) (secondbyte << 8) + firstbyte; }
    static inline uint8_t lobyte( uint16_t word ) { (uint8_t) 0xff & word; }
    static inline uint8_t hibyte( uint16_t word ) { (uint8_t) ( ( 0xff00 & word )>>8 ); }

    static inline bool validate_header_footer(const uint8_t *header_tail, const uint8_t *buffer) {
        return std::memcmp(header_tail, buffer, HEADER_TAIL_SIZE) == 0;
    }

    void MicradarComponent::dump_config() {
    
    }

    void MicradarComponent::setup() {

    }

    void MicradarComponent::loop() {
        while (this->available()) {
            this->readline_(this->read());
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
            uint8_t sum = checkDigit_(this->buffer_data_, this->buffer_pos_- HEADER_TAIL_SIZE - 1 );
            if( sum != this->buffer_data_[this->buffer_pos_ - HEADER_TAIL_SIZE -1 ] ){
                ESP_LOGW(TAG, "error receiving data block; ignoring");
                this->buffer_pos_= 0;
                return;
            }
             ESP_LOGV(TAG, "Handling Data: %s sum: %d ", format_hex_pretty(this->buffer_data_, this->buffer_pos_).c_str(), sum);
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
        ESP_LOGV(TAG, "Sending CONTROL %02X COMMAND %02X", control, command);
        uint16_t check = DATA_FRAME_HEADER[0] + DATA_FRAME_HEADER[1] + control + command + hibyte( len )+ lobyte( len ) ;
        for( int i=0; i< len; i++) check += bytes[i];
        this->write_array( DATA_FRAME_HEADER, sizeof( DATA_FRAME_HEADER ) );
        uint8_t ctrl_bytes[4] = { control, command, hibyte( len ), lobyte ( len ) };
        this->write_array( ctrl_bytes, sizeof( ctrl_bytes ));
        this->write_array( bytes, len  );
        this->write_array( &check, sizeof( check ));
        this->write_array( DATA_FRAME_TAIL, sizeof( DATA_FRAME_TAIL ) );
    }
    
    void MicradarComponent::handle_data_(){

        uint8_t controlWord = this->buffer_data[SHIFT_COMTROL_WORD];
        uint8_t commandWord = this->buffer_data[SHIFT_COMMAND_WORD];
        uint8_t dataLength  = this->buffer_data[SHIFT_DATA_LENGTH_WORD];
        switch( controlWord ){
            case CTRL_SYSTEM_FUNCTIONS:
                break;
            case CTRL_PRODUCT_INFO:
                break;
            case CTRL_WORKING_STATUS:
                break;
            case CTRL_INSTALLATION_METHOD:
                break;
            case CTRL_HUMAN_PRESENCE_FUNCTION:
                break;
        case CTRL_TRACK_FUNCTION:
            break;
        case CTRL_FALL_DETECTION:
            break;
        case CTRL_OTA:
            break;
        default:
            break;
        }
        
    }
}




}