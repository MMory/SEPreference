#ifndef _TELEGRAM_HPP_
#define _TELEGRAM_HPP_

#include <list>
#include <chrono>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <rapidjson/document.h>
#include "boost/asio.hpp"

namespace sepreference {

    typedef enum {
	digital,
	indicator,
	bcd,
	uint8,
	uint16,
	uint32,
	int8,
	int16,
	int32,
	string,
	unknown
    } TelegramPartType;

    
    struct TelegramPart {
	int size;
	int startbit;
	int endbit;
	TelegramPartType type;
	int factor;
	int def;
	int hysteresis;
	uint32_t sent_val;
	uint32_t new_val;
	std::string name;
	std::mutex mutex;
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
	void valcopy(uint32_t val, uint8_t *buf, int startbit, int endbit);
	
	void send_telegram();

    public:
	Telegram(std::string ip, int port, int cycle, const rapidjson::Value &format);
	void setSending(bool sending);
	template<typename T> void updateValue(const std::string& name, T val){
	    for(auto &tp: format){
		std::lock_guard<std::mutex> l(tp->mutex);
		if(tp->name == name){
		    if(val < 0 && (tp->type == uint8 || tp->type == uint16 || tp->type == uint32))
			val = -val;
		    const T scaled_val = val * tp->factor;
		    T delta = std::max((T)tp->sent_val, scaled_val) - std::min((T)tp->sent_val, scaled_val);
		    bool exceeded_hysteresis = tp->hysteresis == 0 || delta >= (T)tp->hysteresis || (scaled_val == 0 && delta > 0);
		    //printf("%d %d\n", delta, tp->hysteresis);
		    tp->new_val = (uint32_t)(int32_t)scaled_val;
		    auto x = conv2be(tp->new_val, tp->size);
		    std::unique_lock<std::mutex> buf_lock(buf_mutex);
		    valcopy(x, buf, tp->startbit, tp->endbit);
		    buf_lock.unlock();
		    if(exceeded_hysteresis){
			comm_condition.notify_one();
			tp->sent_val = tp->new_val;
		    }
		}
	    }
	};
	~Telegram(){
	    setSending(false);
	    delete[] buf;
	    buf = 0;
	}
    };
}

#endif
