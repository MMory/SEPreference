#include "telegramDescriber.hpp"
#include "telegram.hpp"

namespace sepreference {
    
    TelegramDescriber::TelegramDescriber(rapidjson::Document &describer){
	const rapidjson::Value& json_telegrams = describer["telegrams"];
	for(auto& json_t : json_telegrams.GetArray()){
	    std::string ip = json_t["IP"].GetString();
	    int port = json_t["port"].GetInt();
	    int cycle = json_t.HasMember("cycle") && json_t["cycle"].IsInt() ? json_t["cycle"].GetInt() : 0;
	    telegrams.push_back(std::unique_ptr<Telegram> (new Telegram(ip, port, cycle, json_t)));
	}
    }
    void TelegramDescriber::setSending(bool sending){
	for(auto &t: telegrams){
	    t->setSending(sending);
	}
    }
}
