#include <memory>
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include "simulatorExchangeSender.hpp"
#include "telegramDescriber.hpp"

namespace sepreference {
    SimulatorExchangeSenderState SimulatorExchangeSender::state = STATE_OFF;
    std::unique_ptr<TelegramDescriber> SimulatorExchangeSender::describer = std::unique_ptr<TelegramDescriber>(nullptr);
    bool SimulatorExchangeSender::init(std::string &filename){
	if(state == STATE_OFF){
	    rapidjson::Document j;
	    std::ifstream i(filename);
	    if(!i.fail()){
		rapidjson::IStreamWrapper isw(i);
		j.ParseStream(isw);
		describer = std::unique_ptr<TelegramDescriber>(new TelegramDescriber(j));
		state = STATE_INITIALISED;
		return true;
	    }
	}
	return false;
    }

    bool SimulatorExchangeSender::allowSending(bool allowed){
	if(allowed){
	    switch(state){
	    case STATE_INITIALISED:
		state = STATE_SENDING;
		describer->setSending(true);
	    case STATE_SENDING:
		return true;
	    default:
		return false;
	    }
	} else {
	    switch(state){
	    case STATE_SENDING:
		state = STATE_INITIALISED;
		describer->setSending(false);
	    case STATE_INITIALISED:
		return true;
	    default:
		return false;
	    }
	}
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, float val){
	if(state != STATE_OFF)
	    describer->updateValue<float>(name, val);
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, int16_t val){
	if(state != STATE_OFF)
	    describer->updateValue<uint16_t>(name, val);
    }

    void SimulatorExchangeSender::updateValue(const std::string& name, int32_t val){
	if(state != STATE_OFF)
	    describer->updateValue<uint32_t>(name, val);
    }

}
