#include <json.hpp>
#include "telegramDescriber.hpp"
#include "telegram.hpp"

namespace sepreference {
    
    void foo(){
	nlohmann::json j;
	j["pi"] = 3.14159;
	Telegram t;
    }

    int conv2be(uint32_t val){
	union {
	    uint16_t u16;
	    uint8_t u8[2];
	} u;
	u.u16 = 0x0001;

	if(u.u8[0]){
	    val =
	    	((val << 24) & 0xff000000) |
	    	((val <<  8) & 0xff0000  ) |
	    	((val >>  8) & 0xff00    ) |
	    	((val >> 24) & 0xff      );
	}
	return val;        
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
	default:
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
		tp->startbit = bitpos;
		tp->size = getTelegramPartSize(tp->type);
		tp->endbit = bitpos + tp->size - 1;
		tp->name = json_tp["name"];
		bitpos += tp->size;
		t->format.push_back(std::move(tp));
	    }
	    // Now we know the length of the overall telegram and are able to allocate the buffer.
	    // Length should be dividable by 8, if it's not, we allocate an extra byte.
	    const int alloclength = bitpos / 8 + ((bitpos % 8) > 0);
	    t->buf = new uint8_t[alloclength];
	    telegrams.push_back(std::move(t));
	}
    }
    void valcopy(uint32_t val, uint8_t *buf, int startbit, int endbit){
	for(int bitpos = startbit; bitpos < endbit + 1; bitpos += std::min(8, endbit - bitpos)){
	    uint8_t source_byte = (val >> (((endbit - bitpos) / 8) * 8)) & 0xff;
	    uint8_t dest_byte = buf[bitpos / 8];
	    uint8_t dest_byte_mask = 0xff ^ ((1 << bitpos % 8) - 1);
	    if(endbit - bitpos < 8){
		dest_byte_mask |= ((1 << (8 - (endbit % 8))) - 1);
		source_byte = source_byte << (8 - (bitpos % 8));
	    }
	    buf[bitpos / 8] = (dest_byte & dest_byte_mask) | source_byte;
	}
    }
    void TelegramDescriber::updateValue(const std::string& name, uint32_t val){
	val = conv2be(val);
	for(auto &t: telegrams){
	    for(auto &tp: t->format){
		if(tp->name == name){
		    valcopy(val, t->buf, tp->startbit, tp->endbit);
		}
	    }
	}
    }
}
