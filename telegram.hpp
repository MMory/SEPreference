#ifndef _TELEGRAM_HPP_
#define _TELEGRAM_HPP_

#include <list>
#include <vector>
#include <thread>
#include "sepsend.hpp"

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
    };
    
    class Telegram {
    private:
	std::string ip;
	int port;
	std::list<std::unique_ptr<TelegramPart>> format;
	int size;
	int cycle;
	uint8_t *buf;
	bool send_pending;
	std::unique_ptr<std::thread> sendthread;
	int conv2be(uint32_t val);
	void valcopy(uint32_t val, uint8_t *buf, int startbit, int endbit);
	
	void send_telegram(){
	    sepsend();
	};
	//static void thread_func();
    public:
	Telegram(std::string ip, int port, int cycle, nlohmann::json &format);
	template<typename T> void updateValue(const std::string& name, T val){
	    for(auto &tp: format){
		if(tp->name == name){
		    const T scaled_val = val * tp->factor;
		    T delta = std::max((T)tp->sent_val, scaled_val) - std::min((T)tp->sent_val, scaled_val);
		    bool exceeded_hysteresis = delta > tp->hysteresis;
		    tp->new_val = (uint32_t)(int32_t)scaled_val;
		    valcopy(conv2be(tp->new_val), buf, tp->startbit, tp->endbit);
		    if(exceeded_hysteresis){
			if(cycle > 0)
			    send_pending = true;
			else {
			    sendthread = std::unique_ptr<std::thread>(new std::thread([this]() -> void {this->send_telegram();}));
			    sendthread->detach();
			}
		    }
		}
	    }
	};
	~Telegram(){
	    delete buf;
	    buf = 0;
	}
    };
}

#endif
