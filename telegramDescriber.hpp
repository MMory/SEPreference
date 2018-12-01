#ifndef _TELEGRAMDESCRIBER_HPP_
#define _TELEGRAMDESCRIBER_HPP_

#include "telegram.hpp"

namespace sepreference {
    class TelegramDescriber {
    private:
	std::vector<std::unique_ptr<Telegram>> telegrams;
    public:
	TelegramDescriber(nlohmann::json &describer);
	template<typename T> void updateValue(const std::string& name, T val){
	for(auto &t: telegrams){
	    t->updateValue<T>(name, val);
	}
    }
    };
}

#endif
