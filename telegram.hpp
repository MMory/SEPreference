#ifndef TELEGRAM_HPP
#define TELEGRAM_HPP 

#include <list>
#include <vector>

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
	std::string name;
    };
    
    struct Telegram {
	int port;
	std::string ip;
	std::list<std::unique_ptr<TelegramPart>> format;
	int size;
	uint8_t *buf;

	~Telegram(){
	    delete buf;
	}
    };

}

#endif
