#ifndef TELEGRAMDESCRIBER_HPP
#define TELEGRAMDESCRIBER_HPP

#include "telegram.hpp"

namespace sepreference {
    class TelegramDescriber {
    private:
	std::vector<std::unique_ptr<Telegram>> telegrams;
    public:
	TelegramDescriber(nlohmann::json &describer);
	void updateValue(const std::string& name, uint32_t val);
    };
}

#endif
