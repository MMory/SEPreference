#include <memory>
#include "simulatorExchangeSender.hpp"

namespace sepreference {
    SimulatorExchangeSenderState SimulatorExchangeSender::state = STATE_OFF;
    std::unique_ptr<TelegramDescriber> SimulatorExchangeSender::describer = std::unique_ptr<TelegramDescriber>(nullptr);
    void SimulatorExchangeSender::init(){

    }

    SimulatorExchangeSenderState SimulatorExchangeSender::getState(){
    	return state;
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, float val){
	describer->updateValue<float>(name, val);
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, int16_t val){
	describer->updateValue<uint32_t>(name, val);
    }
}
