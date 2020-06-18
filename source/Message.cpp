#include "Message.h"

const std::string Message::ACTIVATE_ALARM = "0";
const std::string Message::DEACTIVATE_ALARM = "1";
const std::string Message::REGISTER_DOORSENSOR = "2";
const std::string Message::SENSOR_LIST = "3";
const std::string Message::ACK = "4";
const std::string Message::ABORT = "5";
const std::string Message::NEXT_CODE = "6";
const std::string Message::REGISTER_SUCCESS = "7";
const std::string Message::ACTIVATION_SUCCESS = "8";
const std::string Message::ACTIVATION_FAILED = "9";
const std::string Message::DEACTIVATION_SUCCESS = "10";
const std::string Message::DEACTIVATION_FAILED = "11";

Message* Message::instance = 0;

Message* Message::getInstance()
{
    if (instance == 0)
    {
        instance = new Message();
    }

    return instance;
}

Message::Message()
{}