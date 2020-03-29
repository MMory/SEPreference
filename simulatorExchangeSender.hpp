#ifndef _SIMULATOREXCHANGESENDER_HPP_
#define _SIMULATOREXCHANGESENDER_HPP_

#include <map>
#include <memory>
#include <thread>

namespace sepreference {
typedef enum {
    STATE_OFF,
    STATE_INITIALISED,
    STATE_SENDING
} SimulatorExchangeSenderState;

class TelegramDescriber;

class SimulatorExchangeSender {
  private:
    static SimulatorExchangeSenderState state;
    static std::map<std::string, int> attributes;
    static std::unique_ptr<TelegramDescriber> describer;

  public:
    static bool init(std::string &filename);
    static bool allowSending(bool allowed);
    static void updateValue(const std::string &name, float val);
    static void updateValue(const std::string &name, uint32_t val);
    static void updateValue(const std::string &name, uint16_t val);
    static void updateValue(const std::string &name, uint8_t val);
    static void updateValue(const std::string &name, int32_t val);
    static void updateValue(const std::string &name, int16_t val);
    static void updateValue(const std::string &name, int8_t val);
};

} // namespace sepreference

#endif
