#ifndef _TELEGRAM_HPP_
#define _TELEGRAM_HPP_

#include <boost/asio.hpp>
#include <boost/locale.hpp>
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <rapidjson/document.h>
#include <thread>
#include <vector>

namespace sepreference {

enum class TelegramPartType {
    digital,
    indicator,
    uint4,
    uint8,
    uint16,
    uint32,
    int8,
    int16,
    int32,
    string,
    unknown
};

struct TelegramPart {
    int startbit;
    int endbit;
    TelegramPartType type;
    int factor;
    size_t len;
    int def;
    uint32_t hysteresis;
    uint32_t sent_val;
    uint32_t new_val;
    std::string name;
    std::mutex mutex;
    constexpr int size() {
        switch (type) {
        case TelegramPartType::digital:
            return 1;
        case TelegramPartType::indicator:
        case TelegramPartType::uint4:
            return 4;
        case TelegramPartType::uint8:
        case TelegramPartType::int8:
            return 8;
        case TelegramPartType::uint16:
        case TelegramPartType::int16:
            return 16;
        case TelegramPartType::uint32:
        case TelegramPartType::int32:
            return 32;
        case TelegramPartType::string:
            return len * 8;
        case TelegramPartType::unknown:
        default:
            return -1;
        }
    };
};

class Telegram {
  private:
    std::string ip;
    int port;
    std::list<std::unique_ptr<TelegramPart>> format;
    int size;
    int cycle;
    std::mutex buf_mutex;
    std::mutex comm_mutex;
    uint8_t *buf;
    std::atomic<bool> sending;
    std::atomic<bool> thread_started;
    std::condition_variable comm_condition;
    std::unique_ptr<std::thread> sendthread;

    int conv2be(int val, int size);
    void valcopy(const uint32_t val, int startbit, int endbit);
    void valcopy(const uint8_t *val, int startbit, int endbit);

  public:
    static const size_t maxBytesize = 65507;
    Telegram(const rapidjson::Value &format);
    void setSending(bool sending);
    template <typename T> void updateValue(const std::string &name, T &val) {
        for (auto &tp : format) {
            std::lock_guard<std::mutex> l(tp->mutex);
            if (tp->name == name) {
                if (val < 0 && (tp->type == TelegramPartType::uint8 ||
                                tp->type == TelegramPartType::uint16 ||
                                tp->type == TelegramPartType::uint32))
                    val = 0 - val;
                const T scaled_val = val * tp->factor;
                tp->new_val = static_cast<uint32_t>(scaled_val);
                uint32_t delta = std::max(tp->sent_val, tp->new_val) -
                                 std::min(tp->sent_val, tp->new_val);
                bool exceeded_hysteresis =
                    delta > tp->hysteresis || (scaled_val == 0 && delta > 0);
                auto x = conv2be(tp->new_val, tp->size());
                std::unique_lock<std::mutex> buf_lock(buf_mutex);
                valcopy(x, tp->startbit, tp->endbit);
                buf_lock.unlock();
                if (exceeded_hysteresis) {
                    comm_condition.notify_one();
                    tp->sent_val = tp->new_val;
                }
            }
        }
    };

    template <typename S>
    void updateStringValue(const std::string &name, std::basic_string<S> &val) {
        std::string utf8val = boost::locale::conv::utf_to_utf<char>(val);
        for (auto &tp : format) {
            std::lock_guard<std::mutex> l(tp->mutex);
            if (tp->name == name) {
                size_t actually_copied_bytes =
                    std::min(tp->len - 1, strlen(utf8val.c_str()));
                size_t actually_copied_bits = actually_copied_bytes * 8;
                int endbit = tp->startbit + actually_copied_bits - 1;
                valcopy(reinterpret_cast<const uint8_t *>(utf8val.c_str()),
                        tp->startbit, endbit);
                comm_condition.notify_one();
            }
        }
    }
    ~Telegram() {
        setSending(false);
        delete[] buf;
        buf = 0;
    }
};
} // namespace sepreference

#endif
