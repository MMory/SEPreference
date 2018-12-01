#include <json.hpp>
#include "telegramDescriber.hpp"
#include "telegram.hpp"

namespace sepreference {
    
    TelegramDescriber::TelegramDescriber(nlohmann::json &describer){
	for(auto &json_t: describer["telegrams"]){
	    std::string ip = json_t["IP"];
	    int port = json_t["port"];
	    int cycle = json_t["cycle"].is_number() ? (int)json_t["cycle"] : 0;
	    telegrams.push_back(std::unique_ptr<Telegram> (new Telegram(ip, port, cycle, json_t["format"])));
	}
    }
    void TelegramDescriber::init_sockets(){
	for(auto &t: telegrams)
	    t->init_socket();
    }
}
