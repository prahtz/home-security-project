#include "Message.h"

const std::string Message::ACTIVATE_ALARM = "00";
const std::string Message::DEACTIVATE_ALARM = "01";
const std::string Message::REGISTER_DOORSENSOR = "02";
const std::string Message::SENSOR_LIST = "03";
const std::string Message::ACK = "04";
const std::string Message::ABORT = "05";
const std::string Message::NEXT_CODE = "06";
const std::string Message::REGISTER_SUCCESS = "07";
const std::string Message::REGISTER_FAILED = "08";
const std::string Message::ACTIVATION_SUCCESS = "09";
const std::string Message::ACTIVATION_FAILED = "0A";
const std::string Message::DEACTIVATION_SUCCESS = "0B";
const std::string Message::DEACTIVATION_FAILED = "0C";
const std::string Message::END_SENSOR_LIST = "0D";
const std::string Message::TIME_OUT = "0E";
const std::string Message::DEACTIVATE_SENSOR = "0F";
const std::string Message::DEACTIVATE_SENSOR_SUCCESS = "10";
const std::string Message::DEACTIVATE_SENSOR_FAILED = "11";
const std::string Message::ACTIVATE_SENSOR = "12";
const std::string Message::ACTIVATE_SENSOR_SUCCESS = "13";
const std::string Message::ACTIVATE_SENSOR_FAILED = "14";

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