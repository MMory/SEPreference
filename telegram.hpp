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
    };
    
    struct Telegram {
	int port;
	std::string ip;
	std::map<std::string, std::unique_ptr<TelegramPart>> format;
	int size;
	std::shared_ptr<uint8_t[]> buf;
    };

}

#endif
