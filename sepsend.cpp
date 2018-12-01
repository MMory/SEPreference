#include "sepsend.hpp"

namespace sepreference {
    void sepsend(){

    }

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
}
