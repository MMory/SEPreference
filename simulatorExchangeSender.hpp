#ifndef SIMULATOREXCHANGESENDER_HPP
#define SIMULATOREXCHANGESENDER_HPP

#include <map>
#include <json.hpp>
#include "telegramDescriber.hpp"

namespace sepreference {

    class SimulatorExchangeSender {
    private:
	std::map<std::string, int> attributes;
	TelegramDescriber describer;
    };
    
}

#endif
