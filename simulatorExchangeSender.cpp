#include "simulatorExchangeSender.hpp"
#include "telegramDescriber.hpp"
#include <fstream>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

static sepreference::DescriberValidationResult
validateDescriber(rapidjson::Document &describer) {
    if (!describer.HasMember("telegrams"))
        return sepreference::DescriberValidationResult::
            DESCRIBER_NO_TOPLEVEL_TELEGRAMS;
    auto &json_telegrams = describer["telegrams"];
    if (!json_telegrams.IsArray())
        return sepreference::DescriberValidationResult::
            DESCRIBER_TOPLEVEL_TELEGRAMS_NOT_ARRAY;
    for (auto &json_telegram : json_telegrams.GetArray()) {
        size_t bitsize = 0;
        if (!json_telegram.HasMember("IP"))
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_NO_IP;
        if (!json_telegram["IP"].IsString())
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_IP_NOT_STRING;
        if (!json_telegram.HasMember("port"))
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_NO_PORT;
        if (!json_telegram["port"].IsInt())
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_PORT_NOT_INT;
        if (json_telegram.HasMember("cycle") && !json_telegram["cycle"].IsInt())
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_CYCLE_NOT_INT;
        if (!json_telegram.HasMember("format"))
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_NO_FORMAT;
        auto &format = json_telegram["format"];
        if (!format.IsArray())
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_FORMAT_NOT_ARRAY;
        for (auto &json_tp : format.GetArray()) {
            if (!json_tp.HasMember("name"))
                return sepreference::DescriberValidationResult::
                    DESCRIBER_TELEGRAMPART_NO_NAME;
            if (!json_tp["name"].IsString())
                return sepreference::DescriberValidationResult::
                    DESCRIBER_TELEGRAMPART_NAME_NOT_STRING;
            if (!json_tp.HasMember("type"))
                return sepreference::DescriberValidationResult::
                    DESCRIBER_TELEGRAMPART_NO_TYPE;
            auto &json_tp_type = json_tp["type"];
            if (!json_tp_type.IsString())
                return sepreference::DescriberValidationResult::
                    DESCRIBER_TELEGRAMPART_TYPE_NOT_STRING;
            const sepreference::TelegramPartType partType =
                sepreference::getTelegramPartType(json_tp_type.GetString());
            if (partType == sepreference::TelegramPartType::unknown)
                return sepreference::DescriberValidationResult::
                    DESCRIBER_TELEGRAMPART_TYPE_UNKNOWN;
            struct sepreference::TelegramPart tp;
            if (partType == sepreference::TelegramPartType::string) {
                if (!json_tp.HasMember("length"))
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_STRING_NO_LENGTH;
                if (!json_tp["length"].IsInt())
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_STRING_LENGTH_NOT_INT;
                if (json_tp.HasMember("factor"))
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_FACTOR;
                if (json_tp.HasMember("default"))
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_DEFAULT;
                if (json_tp.HasMember("hysteresis"))
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_HYSTERESIS;
                tp.len = json_tp["length"].GetInt();
            } else {
                if (json_tp.HasMember("factor") && !json_tp["factor"].IsInt())
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_FACTOR_NOT_INT;
                if (json_tp.HasMember("default") && !json_tp["default"].IsInt())
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_DEFAULT_NOT_INT;
                if (json_tp.HasMember("hysteresis") &&
                    !json_tp["hysteresis"].IsInt())
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_HYSTERESIS_NOT_INT;
                if (json_tp.HasMember("length"))
                    return sepreference::DescriberValidationResult::
                        DESCRIBER_TELEGRAMPART_NONSTRING_ILLEGAL_LENGTH;
            }
            tp.type = partType;
            bitsize += tp.size();
        }
        if (bitsize > sepreference::Telegram::maxBytesize * 8)
            return sepreference::DescriberValidationResult::
                DESCRIBER_TELEGRAM_TOO_LONG;
    }
    return sepreference::DescriberValidationResult::DESCRIBER_VALID;
}

namespace sepreference {

SimulatorExchangeSenderState SimulatorExchangeSender::state =
    SimulatorExchangeSenderState::STATE_OFF;
std::unique_ptr<TelegramDescriber> SimulatorExchangeSender::describer =
    std::unique_ptr<TelegramDescriber>(nullptr);
DescriberValidationResult SimulatorExchangeSender::validationResult =
    DescriberValidationResult::DESCRIBER_NOT_PROVIDED;
bool SimulatorExchangeSender::init(std::string &filename) {
    if (state == SimulatorExchangeSenderState::STATE_OFF) {
        rapidjson::Document j;
        std::ifstream i(filename);
        if (!i.fail()) {
            rapidjson::IStreamWrapper isw(i);
            j.ParseStream(isw);
            if (j.GetParseError() !=
                rapidjson::ParseErrorCode::kParseErrorNone) {
                validationResult =
                    DescriberValidationResult::DESCRIBER_INVALID_JSON;
                return false;
            }
            validationResult = validateDescriber(j);
            if (validationResult !=
                DescriberValidationResult::DESCRIBER_VALID)
                return false;
            describer =
                std::unique_ptr<TelegramDescriber>(new TelegramDescriber(j));
            state = SimulatorExchangeSenderState::STATE_INITIALISED;
            return true;
        } else {
            validationResult =
                DescriberValidationResult::DESCRIBER_CANNOT_READ_FILE;
        }
    }
    return false;
}

bool SimulatorExchangeSender::allowSending(bool allowed) {
    if (allowed) {
        switch (state) {
        case SimulatorExchangeSenderState::STATE_INITIALISED:
            state = SimulatorExchangeSenderState::STATE_SENDING;
            describer->setSending(true);
        case SimulatorExchangeSenderState::STATE_SENDING:
            return true;
        default:
            return false;
        }
    } else {
        switch (state) {
        case SimulatorExchangeSenderState::STATE_SENDING:
            state = SimulatorExchangeSenderState::STATE_INITIALISED;
            describer->setSending(false);
        case SimulatorExchangeSenderState::STATE_INITIALISED:
            return true;
        default:
            return false;
        }
    }
}

void SimulatorExchangeSender::updateValue(const std::string &name, float val) {
    if (state != SimulatorExchangeSenderState::STATE_OFF)
        describer->updateValue<float>(name, val);
}

void SimulatorExchangeSender::updateValue(const std::string &name,
                                          int16_t val) {
    if (state != SimulatorExchangeSenderState::STATE_OFF) {
        auto uval = static_cast<uint16_t>(val);
        describer->updateValue<uint16_t>(name, uval);
    }
}

void SimulatorExchangeSender::updateValue(const std::string &name,
                                          int32_t val) {
    if (state != SimulatorExchangeSenderState::STATE_OFF) {
        auto uval = static_cast<uint32_t>(val);
        describer->updateValue<uint32_t>(name, uval);
    }
}

const std::string SimulatorExchangeSender::getErrorMsg() {
    switch (validationResult) {
    case DescriberValidationResult::DESCRIBER_VALID:
        return "Valid telegram describer.";
    case DescriberValidationResult::DESCRIBER_NOT_PROVIDED:
        return "No describer provided yet.";
    case DescriberValidationResult::DESCRIBER_CANNOT_READ_FILE:
        return "Cannot read provided file.";
    case DescriberValidationResult::DESCRIBER_INVALID_JSON:
        return "Provided file is no valid json.";
    case DescriberValidationResult::DESCRIBER_NO_TOPLEVEL_TELEGRAMS:
        return "No single top-level element \"telegrams\".";
    case DescriberValidationResult::DESCRIBER_TOPLEVEL_TELEGRAMS_NOT_ARRAY:
        return "Top-level element \"telegrams\" is not an array.";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_NO_IP:
        return "Telegram does not have mandatory element \"IP\".";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_IP_NOT_STRING:
        return "Telegram element \"IP\" is not a string.";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_NO_PORT:
        return "Telegram does not have mandatory element \"port\".";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_PORT_NOT_INT:
        return "Telegram element \"port\" is not an int.";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_CYCLE_NOT_INT:
        return "Telegram element \"port\" is not an int.";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_NO_FORMAT:
        return "Telegram does not have mandatory element \"port\".";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_FORMAT_NOT_ARRAY:
        return "Telegram element \"format\" is not an array.";
    case DescriberValidationResult::DESCRIBER_TELEGRAM_TOO_LONG:
        return "Telegram is too long.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_NO_NAME:
        return "Telegram part does not have mandatory element \"name\".";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_NAME_NOT_STRING:
        return "Telegram part element \"name\" is not a string.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_NO_TYPE:
        return "Telegram part does not have mandatory element \"type\".";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_TYPE_NOT_STRING:
        return "Telegram part element \"type\" is not a string.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_TYPE_UNKNOWN:
        return "Telegram part element \"type\" value is unknown.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_FACTOR_NOT_INT:
        return "Telegram part element \"factor\" is not an int.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_DEFAULT_NOT_INT:
        return "Telegram part element \"default\" is not an int.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_HYSTERESIS_NOT_INT:
        return "Telegram part element \"hysteresis\" is not an int.";
    case DescriberValidationResult::DESCRIBER_TELEGRAMPART_STRING_NO_LENGTH:
        return "Telegram part type is string, but mandatory element \"length\" is not present.";
    case DescriberValidationResult::
        DESCRIBER_TELEGRAMPART_STRING_LENGTH_NOT_INT:
        return "Telegram part element \"length\" is not an int.";
    case DescriberValidationResult::
        DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_FACTOR:
        return "Telegram part type is string, for that, the element \"factor\" is illegal.";
    case DescriberValidationResult::
        DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_DEFAULT:
        return "Telegram part type is string, for that, the element \"default\" is illegal.";
    case DescriberValidationResult::
        DESCRIBER_TELEGRAMPART_STRING_ILLEGAL_HYSTERESIS:
        return "Telegram part type is string, for that, the element \"hysteresis\" is illegal.";
    case DescriberValidationResult::
        DESCRIBER_TELEGRAMPART_NONSTRING_ILLEGAL_LENGTH:
        return "Telegram part type is not string, for that, the element \"length\" is illegal.";
    default:
      return "Unknown error.";
    }
}

} // namespace sepreference
