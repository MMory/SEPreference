#include <memory>
#include <fstream>
#include <json.hpp>
#include "simulatorExchangeSender.hpp"

namespace sepreference {
    SimulatorExchangeSenderState SimulatorExchangeSender::state = STATE_OFF;
    std::unique_ptr<TelegramDescriber> SimulatorExchangeSender::describer = std::unique_ptr<TelegramDescriber>(nullptr);
    void SimulatorExchangeSender::init(std::string &filename){
	if(state == STATE_OFF){
	    nlohmann::json j;
	    std::ifstream i(filename);
	    i >> j;
	    describer = std::unique_ptr<TelegramDescriber>(new TelegramDescriber(j));
	    describer->init_sockets();
	    state = STATE_INITIALISED;
	}
    }

    SimulatorExchangeSenderState SimulatorExchangeSender::getState(){
    	return state;
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, float val){
	describer->updateValue<float>(name, val);
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, int16_t val){
	describer->updateValue<uint16_t>(name, val);
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, int32_t val){
	describer->updateValue<uint32_t>(name, val);
    }
}
