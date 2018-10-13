#ifndef TELEGRAM_HPP
#define TELEGRAM_HPP 

#include <list>
#include <vector>

namespace sepreference {

    typedef enum {
	indicator,
	digital,
	analog,
	timer
    } TelegramPartType;

    
    struct TelegramPart {
	int size;
	TelegramPartType type;
	int factor;
	int def;
    };
    
    struct Telegram {
	int port;
	std::string ip;
	std::list<std::unique_ptr<TelegramPart>> format;
    };

}

#endif
