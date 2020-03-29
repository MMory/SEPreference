#ifndef _SIMULATOREXCHANGERECEIVER_HPP_
#define _SIMULATOREXCHANGERECEIVER_HPP_



namespace sepreference {
    class SimulatorExchangeReceiver{
    private:
        enum class SimulatorExchangeReceiverState{
            STATE_OFF,
            STATE_INITIALISED,
            STATE_SENDING
        };

        static SimulatorExchangeReceiverState state;
        static std::map<std::string, std::function<void(rapidjson::Value&)>> handlers;
        static std::thread receiveThread;
        static void addHandler(std::string, std::function<void(rapidjson::Value&)>);
        static void removeHandler(std::string);
        static void setReceiving(bool);
        static void receiveThreadFunction();
    };
}
#endif