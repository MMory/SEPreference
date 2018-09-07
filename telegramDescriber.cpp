#include <json.hpp>
#include "telegramDescriber.hpp"
#include "telegram.hpp"

namespace sepreference {
    
    void foo(){
	nlohmann::json j;
	j["pi"] = 3.14159;
	Telegram t;
    }

    TelegramPartType getTelegramPartType(std::string typestr){
	if(typestr == "Digital")
	    return digital;
	if(typestr == "Indicator")
	    return indicator;
	if(typestr == "BCD")
	    return bcd;
	if(typestr == "UInt8")
	    return uint8;
	if(typestr == "UInt16")
	    return uint16;
	if(typestr == "UInt32")
	    return uint32;
	if(typestr == "Int8")
	    return int8;
	if(typestr == "Int16")
	    return int16;
	if(typestr == "Int32")
	    return int32;
	if(typestr == "String")
	    return string;
	return unknown;
    }

    const int getTelegramPartSize(const TelegramPartType t){
	switch(t){
	case digital:
	    return 1;
	case indicator:
	case bcd:
	    return 4;
	case uint8:
	case int8:
	    return 8;
	case uint16:
	case int16:
	    return 16;
	case uint32:
	case int32:
	    return 32;
	case string:
	case unknown:
	    return -1;
	}
    }
    
    TelegramDescriber::TelegramDescriber(nlohmann::json &describer){
	for(auto &json_t: describer["telegrams"]){
	    int bitpos = 0;
	    std::unique_ptr<Telegram> t(new Telegram());
	    t->ip = json_t["ip"];
	    t->port = json_t["port"];
	    for(auto &json_tp: json_t["format"]){
		std::unique_ptr<TelegramPart> tp(new TelegramPart());
		tp->type = getTelegramPartType(json_tp["type"]);
		if(json_tp["factor"].is_number())
		    tp->factor = json_tp["factor"];
		else
		    tp->factor = 1;
		if(json_tp["default"].is_number())
		    tp->def = json_tp["default"];
		else
		    tp->def = 0;
		tp->size = getTelegramPartSize(tp->type);
		t->format[json_tp["name"]] = std::move(tp);
	    }
	    telegrams.push_back(std::move(t));
	}
    }
}
