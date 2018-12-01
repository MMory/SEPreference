#include <json.hpp>
#include "simulatorExchangeSender.hpp"
#include "telegram.hpp"

namespace sepreference {
    void thread_func(){
	while(SimulatorExchangeSender::getState() == STATE_SENDING){

	}
    }

    int Telegram::conv2be(uint32_t val){
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

    void Telegram::valcopy(uint32_t val, uint8_t *buf, int startbit, int endbit){
	int bitpos = startbit;
	// Check for pointless input
	if(startbit > endbit)
	    return;
	// This loop copies the bits from val to the right location in buf with bit
	// precision.
	do{
	    // window of 8 bits.
	    uint8_t source_byte = (val >> std::max((endbit - bitpos - 7), 0)) & 0xff;
	    uint8_t dest_byte = buf[bitpos / 8];
	    // mask for zeroing out the old bits that are replaced, while leaving the rest unchanged.
	    uint8_t dest_byte_mask = 0xff ^ ((1 << (8 - bitpos % 8)) - 1);
	    // corner case: last window is smaller, so align it to byte-begin and shrink the mask
	    if(endbit - bitpos < 8){
		dest_byte_mask |= ((1 << (7 - (endbit % 8))) - 1);
		source_byte = source_byte << (7 - (endbit - bitpos));
	    }
	    // align source to bit start offset
	    source_byte = source_byte >> (bitpos % 8);

	    // mask to protect old bits in buf that should not be changed.
	    // This doesn't hurt, but reduces the damage when we did
	    // something wrong with the source_byte.
	    uint8_t source_byte_mask = ~dest_byte_mask;
	    buf[bitpos / 8] = (dest_byte & dest_byte_mask) | (source_byte & source_byte_mask);
	    // increment to byte boundary
	    bitpos = (bitpos / 8 + 1) * 8;
	} while(bitpos <= endbit);
    }

    
    Telegram::Telegram(std::string ip, int port, int cycle, nlohmann::json &format){
	send_pending = false;
	this->ip = ip;
	this->port = port;
	this->cycle = cycle;
	int bitpos = 0;
	for(auto &json_tp: format){
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
	    tp->hysteresis = json_tp["hysteresis"].is_number() ? (int)json_tp["hysteresis"] : 0;
	    bitpos += tp->size;
	    this->format.push_back(std::move(tp));
	}
	// Now we know the length of the overall telegram and are able to allocate the buffer.
	// Length should be dividable by 8, if it's not, we allocate an extra byte.
	const int alloclength = bitpos / 8 + ((bitpos % 8) > 0);
	buf = new uint8_t[alloclength];
	size = alloclength;
	if(cycle > 0){
	    sendthread = std::unique_ptr<std::thread>(new std::thread(thread_func));
	}
    }

}
