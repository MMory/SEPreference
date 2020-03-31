#ifndef _TELEGRAMDESCRIBER_HPP_
#define _TELEGRAMDESCRIBER_HPP_

#include <rapidjson/document.h>

#include "telegram.hpp"

namespace sepreference {
class TelegramDescriber {
  private:
    std::vector<std::unique_ptr<Telegram>> telegrams;

  public:
    TelegramDescriber(rapidjson::Document &describer);
    void setSending(bool sending);
    template <typename T> void updateValue(const std::string &name, T &val) {
        for (auto &t : telegrams) {
            t->updateValue<T>(name, val);
        }
    }
    template <typename S>
    void updateStringValue(const std::string &name, std::basic_string<S> &val) {
        for (auto &t : telegrams) {
            t->updateStringValue<S>(name, val);
        }
    }
};
} // namespace sepreference

#endif
