#pragma once

#include "esphome/core/component.h"
#include "remote_base.h"

#include <cinttypes>

namespace esphome {
namespace remote_base {

struct AokData {
  uint32_t id;
  uint16_t channel;
  uint8_t button;
  uint8_t check;

  bool operator==(const AokData &rhs) const {
    return id == rhs.id && channel == rhs.channel && button == rhs.button && check == rhs.check;
  }
};

class AokProtocol : public RemoteProtocol<AokData> {
 public:
  void encode(RemoteTransmitData *dst, const AokData &data) override;
  optional<AokData> decode(RemoteReceiveData src) override;
  void dump(const AokData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Aok)

template<typename... Ts> class AokAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint32_t, id)
  TEMPLATABLE_VALUE(uint16_t, channel)
  TEMPLATABLE_VALUE(uint8_t, button)
  TEMPLATABLE_VALUE(uint8_t, check)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    AokData data{};
    data.id = this->id_.value(x...);
    data.channel = this->channel_.value(x...);
    data.button = this->button_.value(x...);
    data.check = this->check_.value(x...);
    AokProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
