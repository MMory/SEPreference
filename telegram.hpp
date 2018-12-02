#ifndef _TELEGRAM_HPP_
#define _TELEGRAM_HPP_

#include <list>
#include <chrono>
#include <vector>
#include <thread>
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

	boost::asio::io_service io_service;
	boost::asio::ip::udp::socket socket;
	boost::asio::ip::udp::endpoint remote_endpoint;
	
	int conv2be(int val, int size);
	void valcopy(uint32_t val, uint8_t *buf, int startbit, int endbit);
	
	void send_telegram();

    public:
	Telegram(std::string ip, int port, int cycle, const rapidjson::Value &format);
	void init_socket();
	void close_socket();
	template<typename T> void updateValue(const std::string& name, T val){
	    for(auto &tp: format){
		if(tp->name == name){
		    const T scaled_val = val * tp->factor;
		    T delta = std::max((T)tp->sent_val, scaled_val) - std::min((T)tp->sent_val, scaled_val);
		    bool exceeded_hysteresis = tp->hysteresis == 0 || delta >= tp->hysteresis || scaled_val == 0;
		    //printf("%d %d\n", delta, tp->hysteresis);
		    tp->new_val = (uint32_t)(int32_t)scaled_val;
		    auto x = conv2be(tp->new_val, tp->size);
		    valcopy(x, buf, tp->startbit, tp->endbit);
		    if(exceeded_hysteresis){
			if(cycle > 0)
			    send_pending = true;
			else {
			    //send_telegram();
			    sendthread = std::unique_ptr<std::thread>(new std::thread([this]() -> void {this->send_telegram();}));
			    sendthread->detach();
			    tp->sent_val = tp->new_val;
			}
		    }
		}
	    }
	};
	~Telegram(){
	    close_socket();
	    delete buf;
	    buf = 0;
	}
    };
}

#endif
