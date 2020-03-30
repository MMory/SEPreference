#include "telegramDescriber.hpp"
#include "telegram.hpp"

namespace sepreference {

TelegramDescriber::TelegramDescriber(rapidjson::Document &describer) {
    const rapidjson::Value &json_telegrams = describer["telegrams"];
    for (auto &json_t : json_telegrams.GetArray()) {
        telegrams.push_back(
            std::unique_ptr<Telegram>(new Telegram(json_t)));
    }
}
void TelegramDescriber::setSending(bool sending) {
    for (auto &t : telegrams) {
        t->setSending(sending);
    }
}
} // namespace sepreference
