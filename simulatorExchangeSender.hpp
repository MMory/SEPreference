#ifndef _SIMULATOREXCHANGESENDER_HPP_
#define _SIMULATOREXCHANGESENDER_HPP_

#include <map>
#include <memory>
#include <string>
#include <thread>

namespace sepreference {
enum class SimulatorExchangeSenderState {
    STATE_OFF,
    STATE_INITIALISED,
    STATE_SENDING
};

enum class DescriberValidationResult {
    DESCRIBER_VALID,
    DESCRIBER_NOT_PROVIDED,
    DESCRIBER_CANNOT_READ_FILE,
    DESCRIBER_INVALID_JSON,
    DESCRIBER_NO_TOPLEVEL_TELEGRAMS,
    DESCRIBER_TOPLEVEL_TELEGRAMS_NOT_ARRAY,
    DESCRIBER_TELEGRAM_NO_IP,
    DESCRIBER_TELEGRAM_IP_NOT_STRING,
    DESCRIBER_TELEGRAM_NO_PORT,
    DESCRIBER_TELEGRAM_PORT_NOT_INT,
    DESCRIBER_TELEGRAM_CYCLE_NOT_INT,
    DESCRIBER_TELEGRAM_NO_FORMAT,
    DESCRIBER_TELEGRAM_FORMAT_NOT_ARRAY,
    DESCRIBER_TELEGRAM_TOO_LONG,
    DESCRIBER_TELEGRAMPART_NO_NAME,
    DESCRIBER_TELEGRAMPART_NAME_NOT_STRING,
    DESCRIBER_TELEGRAMPART_NO_TYPE,
    DESCRIBER_TELEGRAMPART_TYPE_NOT_STRING,
    DESCRIBER_TELEGRAMPART_TYPE_UNKNOWN,
    DESCRIBER_TELEGRAMPART_FACTOR_NOT_INT,
    DESCRIBER_TELEGRAMPART_DEFAULT_NOT_INT,
    DESCRIBER_TELEGRAMPART_HYSTERESIS_NOT_INT,
    DESCRIBER_TELEGRAMPART_STRING_NO_LENGTH,
    DESCRIBER_TELEGRAMPART_STRING_LENGTH_NOT_INT,
    DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_FACTOR,
    DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_DEFAULT,
    DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_HYSTERESIS,
    DESCRIBER_TELEGRAMPART_NONSTRING_ILLEGAL_LENGTH
};

enum class TelegramPartType;

TelegramPartType getTelegramPartType(const std::string &typestr);

class TelegramDescriber;

class SimulatorExchangeSender {
  private:
    static SimulatorExchangeSenderState state;
    static std::unique_ptr<TelegramDescriber> describer;
    static DescriberValidationResult validationResult;

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
    static void updateValue(const std::string &name, std::wstring &val);
    static const std::string getErrorMsg();
}; // namespace sepreference

} // namespace sepreference

#endif
