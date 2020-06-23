#include "Message.h"

const std::string Message::ACTIVATE_ALARM = "0";
const std::string Message::DEACTIVATE_ALARM = "1";
const std::string Message::REGISTER_DOORSENSOR = "2";
const std::string Message::SENSOR_LIST = "3";
const std::string Message::ACK = "4";
const std::string Message::ABORT = "5";
const std::string Message::NEXT_CODE = "6";
const std::string Message::REGISTER_SUCCESS = "7";
const std::string Message::REGISTER_FAILED = "8";
const std::string Message::ACTIVATION_SUCCESS = "9";
const std::string Message::ACTIVATION_FAILED = "10";
const std::string Message::DEACTIVATION_SUCCESS = "11";
const std::string Message::DEACTIVATION_FAILED = "12";
const std::string Message::END_SENSOR_LIST = "13";
const std::string Message::TIME_OUT = "14";

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