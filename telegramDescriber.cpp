#include <json.hpp>
#include "telegramDescriber.hpp"
#include "telegram.hpp"

namespace sepreference {
    
    void foo(){
	nlohmann::json j;
	j["pi"] = 3.14159;
	Telegram t;
    }

    TelegramPartType getTelegramPartType(std::string typestr){

    }
    
    TelegramDescriber::TelegramDescriber(nlohmann::json &describer){
	for(auto &json_t: describer["telegrams"]){
	    std::unique_ptr<Telegram> t(new Telegram());
	    t->ip = json_t["ip"];
	    t->port = json_t["port"];
	    for(auto &json_tp: json_t["format"]){
		std::unique_ptr<TelegramPart> tp(new TelegramPart());
		tp->type = getTelegramPartType(json_t["type"]);
		t->format.push_back(std::move(tp));
	    }
	    telegrams.push_back(std::move(t));
	}
    }
}
