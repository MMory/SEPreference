//#include "external/include/json.hpp"
//#include <stdio.h>
#include <iostream>
#include "simulatorExchangeSender.hpp"

// void printbits(uint8_t byte){
//     printf("%d%d%d%d%d%d%d%d",
// 	   (byte >> 7) & 1,
// 	   (byte >> 6) & 1,
// 	   (byte >> 5) & 1,
// 	   (byte >> 4) & 1,
// 	   (byte >> 3) & 1,
// 	   (byte >> 2) & 1,
// 	   (byte >> 1) & 1,
// 	   (byte >> 0) & 1);
// }


// void valcopy(uint32_t val, uint8_t *buf, int startbit, int endbit){
//     int bitpos = startbit;
//     // Check for pointless input
//     if(startbit > endbit)
// 	return;
//     // This loop copies the bits from val to the right location in buf with bit
//     // precision.
//     do{
// 	// window of 8 bits. 
// 	uint8_t source_byte = (val >> std::max((endbit - bitpos - 7), 0)) & 0xff;
// 	uint8_t dest_byte = buf[bitpos / 8];
// 	// mask for zeroing out the old bits that are replaced, while leaving the rest unchanged.
// 	uint8_t dest_byte_mask = 0xff ^ ((1 << (8 - bitpos % 8)) - 1);
// 	// corner case: last window is smaller, so align it to byte-begin and shrink the mask
// 	if(endbit - bitpos < 8){
// 	    dest_byte_mask |= ((1 << (7 - (endbit % 8))) - 1);
// 	    source_byte = source_byte << (7 - (endbit - bitpos));
// 	}
// 	// align source to bit start offset
// 	source_byte = source_byte >> (bitpos % 8);

// 	// mask to protect old bits in buf that should not be changed.
// 	// This doesn't hurt, but reduces the damage when we did
// 	// something wrong with the source_byte.
// 	uint8_t source_byte_mask = ~dest_byte_mask;
// 	// printf("%x ", source_byte);
// 	// printbits(dest_byte_mask);
// 	// printf(" ");
// 	// printbits(source_byte_mask);
// 	// printf("\n");
// 	buf[bitpos / 8] = (dest_byte & dest_byte_mask) | (source_byte & source_byte_mask);
// 	// increment to byte boundary
// 	bitpos = (bitpos / 8 + 1) * 8;
//     } while(bitpos <= endbit);
// }

int main(int argc, char **argv){
    // nlohmann::json j = nlohmann::json::parse("{\"keyy\": [1,2,3]}");
    // std::cout << j["key"].is_null() << j["keyy"].is_null();
    // uint8_t buf[16] = { 0 };
    // valcopy(0xfffffeff, buf, 9, 38);
    // valcopy(0x00000001, buf, 47, 47);
    // for(int i = 0; i < 16; i++){
    // 	printbits(buf[i]);
    // 	printf(" ");
    // }
    // printf("\n");
    //std::string s = j["key"];
    //std::cout << s;
    if(argc < 2){
	printf("provide tdf please!\n");
	return 1;
    }
    std::string s(argv[1]);
    sepreference::SimulatorExchangeSender::init(s);
    std::string key("IndLzb1000");
    std::string key2("speed");
    // sepreference::SimulatorExchangeSender::updateValue(key, 1);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // sepreference::SimulatorExchangeSender::updateValue(key, 3);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    // sepreference::SimulatorExchangeSender::updateValue(key, 0);
    // std::this_thread::sleep_for(std::chrono::seconds(5));
    sepreference::SimulatorExchangeSender::allowSending(true);
    sepreference::SimulatorExchangeSender::allowSending(false);
    while(0){
        float val = 0;
	std::string uk;
	std::cout << "key\n";
	fflush(stdin);
	std::cin >> uk;
	std::cout << "value\n";
	std::cin >> val;
	fflush(stdout);
	sepreference::SimulatorExchangeSender::updateValue(uk, val);
	fflush(stdout);
	std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // float val = 0;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // while(val < 200){
    // 	val += 0.5;
    // 	sepreference::SimulatorExchangeSender::updateValue(key2, val);
    // 	std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // }
}
