#include "aok_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.aok";

static const uint32_t PREAMBLE_HIGH_US = 460;
static const uint32_t PREAMBLE_LOW_US = 440;
static const uint32_t HEADER_LONG_US = 5200;
static const uint32_t HEADER_SHORT_US = 500;
static const uint32_t HEADER_HIGH_US = 5000;
static const uint32_t HEADER_LOW_US = 1500;
static const uint32_t BIT_ZERO_HIGH_US = 400;
static const uint32_t BIT_ZERO_LOW_US = 500;
static const uint32_t BIT_ONE_HIGH_US = 730;
static const uint32_t BIT_ONE_LOW_US = 170;

void AokProtocol::encode(RemoteTransmitData *dst, const AokData &data) {
  dst->set_carrier_frequency(0);
  dst->reserve(2 + 40 * 2u);

  dst->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint32_t mask = 1UL << (23); mask != 0; mask >>= 1) {
    if (data.id & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }

  for (uint32_t mask = 1UL << (7); mask != 0; mask >>= 1) {
    if (data.channel & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }

  for (uint32_t mask = 1UL << (3); mask != 0; mask >>= 1) {
    if (data.button & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }

  for (uint32_t mask = 1UL << (3); mask != 0; mask >>= 1) {
    if (data.check & mask) {
      dst->item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
    } else {
      dst->item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
    }
  }
}
optional<AokData> AokProtocol::decode(RemoteReceiveData src) {
  AokData out{
      .id = 0,
      .channel = 0,
      .button = 0,
      .check = 0,
  };
  ESP_LOGI(TAG, "Startaok preamble");
  for (uint8_t i = 0; i < 5; i++) {
    if (!src.expect_item(PREAMBLE_HIGH_US, PREAMBLE_LOW_US)) {
      return{};
    }
  }
   ESP_LOGI(TAG, "Received aok preamble");
  if (!src.expect_item(HEADER_LONG_US, HEADER_SHORT_US))
    return {};
ESP_LOGI(TAG, "Received aok header");
  for (uint8_t i = 0; i < 32; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.id = (out.id << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.id = (out.id << 1) | 0;
    } else {
      return {};
    }
  }

  for (uint8_t i = 0; i < 16; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.channel = (out.channel << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.channel = (out.channel << 1) | 0;
    } else {
      return {};
    }
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.button = (out.button << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.button = (out.button << 1) | 0;
    } else {
      return {};
    }
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (src.expect_item(BIT_ONE_HIGH_US, BIT_ONE_LOW_US)) {
      out.check = (out.check << 1) | 1;
    } else if (src.expect_item(BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US)) {
      out.check = (out.check << 1) | 0;
    } else {
      return {};
    }
  }
  // Last bit is not received properly but can be decoded
  if (src.expect_mark(BIT_ONE_HIGH_US)) {
    out.check = (out.check << 1) | 1;
  } else if (src.expect_mark(BIT_ZERO_HIGH_US)) {
    out.check = (out.check << 1) | 0;
  } else {
    return {};
  }

  return out;
}
void AokProtocol::dump(const AokData &data) {
  ESP_LOGI(TAG, "Received Aok: id=0x%08" PRIX32 ", channel=%d, button=%d, check=%d", data.id, data.channel,
           data.button, data.check);
}

}  // namespace remote_base
}  // namespace esphome
