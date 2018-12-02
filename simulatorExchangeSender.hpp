#ifndef _SIMULATOREXCHANGESENDER_HPP_
#define _SIMULATOREXCHANGESENDER_HPP_

#include <memory>
#include <map>
#include <thread>
#include <json.hpp>
#include "telegramDescriber.hpp"

namespace sepreference {
    typedef enum {
	STATE_OFF,
	STATE_INITIALISED,
	STATE_SENDING
    } SimulatorExchangeSenderState;
    
    class SimulatorExchangeSender {
    private:
	static SimulatorExchangeSenderState state;
	static std::map<std::string, int> attributes;
	static std::unique_ptr<TelegramDescriber> describer;
    public:
	static void init(std::string &filename);
	static bool allowSending(bool allowed);
	static SimulatorExchangeSenderState getState();
	static void updateValue(const std::string& name, float val);
	static void updateValue(const std::string& name, uint32_t val);
	static void updateValue(const std::string& name, uint16_t val);
	static void updateValue(const std::string& name, uint8_t val);
	static void updateValue(const std::string& name, int32_t val);
	static void updateValue(const std::string& name, int16_t val);
	static void updateValue(const std::string& name, int8_t val);
    };
    
}

#endif
