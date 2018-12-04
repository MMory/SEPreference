#include <rapidjson/document.h>
#include "simulatorExchangeSender.hpp"
#include "telegram.hpp"

void printbits(uint8_t byte){
    printf("%d%d%d%d%d%d%d%d ",
	   (byte >> 7) & 1,
	   (byte >> 6) & 1,
	   (byte >> 5) & 1,
	   (byte >> 4) & 1,
	   (byte >> 3) & 1,
	   (byte >> 2) & 1,
	   (byte >> 1) & 1,
	   (byte >> 0) & 1);
}

namespace sepreference {
    TelegramPartType getTelegramPartType(std::string typestr){
	if(typestr == "digital")
	    return digital;
	if(typestr == "indicator")
	    return indicator;
	if(typestr == "bcd")
	    return bcd;
	if(typestr == "uint8")
	    return uint8;
	if(typestr == "uint16")
	    return uint16;
	if(typestr == "uint32")
	    return uint32;
	if(typestr == "int8")
	    return int8;
	if(typestr == "int16")
	    return int16;
	if(typestr == "int32")
	    return int32;
	if(typestr == "string")
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

    int Telegram::conv2be(int val, int size){
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
	    val = val >> (32 - 8 * ((size + 7) / 8));
	}
	return val;        
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

    
    Telegram::Telegram(std::string ip, int port, int cycle, const rapidjson::Value &format){
	this->ip = ip;
	this->port = port;
	this->cycle = cycle;
	this->sending.store(false);
	this->thread_started.store(false);
	int bitpos = 0;
	for(auto& json_tp: format["format"].GetArray()){
	    std::unique_ptr<TelegramPart> tp(new TelegramPart());
	    tp->type = getTelegramPartType(json_tp["type"].GetString());
	    if(json_tp.HasMember("factor") && json_tp["factor"].IsInt())
		tp->factor = json_tp["factor"].GetInt();
	    else
		tp->factor = 1;
	    if(json_tp.HasMember("default") && json_tp["default"].IsInt())
		tp->def = json_tp["default"].GetInt();
	    else
		tp->def = 0;
	    tp->startbit = bitpos;
	    tp->size = getTelegramPartSize(tp->type);
	    tp->endbit = bitpos + tp->size - 1;
	    tp->name = json_tp["name"].GetString();
	    if(json_tp.HasMember("hysteresis") && json_tp["hysteresis"].IsInt())
	       tp->hysteresis = json_tp["hysteresis"].GetInt();
	    else
		tp->hysteresis = 0;
	    bitpos += tp->size;
	    this->format.push_back(std::move(tp));
	}
	// Now we know the length of the overall telegram and are able to allocate the buffer.
	// Length should be dividable by 8, if it's not, we allocate an extra byte.
	const int alloclength = bitpos / 8 + ((bitpos % 8) > 0);
	buf = new uint8_t[alloclength]();
	size = alloclength;
    }

    void Telegram::setSending(bool sending){
	if(sending){
	    if(this->sending.load())
		return;
	    this->sending.store(true);
	    sendthread = std::unique_ptr<std::thread>(new std::thread([this]() -> void {
			boost::asio::io_service io_service;
			boost::asio::ip::udp::socket socket(io_service);
			boost::asio::ip::udp::endpoint remote_endpoint;
			socket.open(boost::asio::ip::udp::v4());
			remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(this->ip), this->port);
			while(this->sending.load()){
			    boost::system::error_code err;
			    std::unique_lock<std::mutex> buf_lock(buf_mutex);
			    socket.send_to(boost::asio::buffer(buf, size), remote_endpoint, 0, err);
			    buf_lock.unlock();
			    if(!this->thread_started.load()){
				this->comm_condition.notify_one();
				this->thread_started.store(true);
			    }
			    std::unique_lock<std::mutex> comm_lock(comm_mutex);
			    this->comm_condition.wait(comm_lock);
			}
		    }));
	    std::unique_lock<std::mutex> comm_lock(comm_mutex);
	    comm_condition.wait(comm_lock);
	} else {
	    if(!this->sending.load())
		return;
	    this->sending.store(false);
	    comm_condition.notify_one();
	    sendthread->join();
	    thread_started.store(false);
	}
    }
}
