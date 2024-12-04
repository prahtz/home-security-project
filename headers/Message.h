#pragma once
#include <string>
namespace message
{
    static const std::string ACTIVATE_ALARM = "00";
    static const std::string DEACTIVATE_ALARM = "01";
    static const std::string REGISTER_DOORSENSOR = "02";
    static const std::string SENSOR_LIST = "03";
    static const std::string ACK = "04";
    static const std::string ABORT = "05";
    static const std::string NEXT_CODE = "06";
    static const std::string REGISTER_SUCCESS = "07";
    static const std::string REGISTER_FAILED = "08";
    static const std::string ACTIVATION_SUCCESS = "09";
    static const std::string ACTIVATION_FAILED = "0A";
    static const std::string DEACTIVATION_SUCCESS = "0B";
    static const std::string DEACTIVATION_FAILED = "0C";
    static const std::string END_SENSOR_LIST = "0D";
    static const std::string TIME_OUT = "0E";
    static const std::string DEACTIVATE_SENSOR = "0F";
    static const std::string DEACTIVATE_SENSOR_SUCCESS = "10";
    static const std::string DEACTIVATE_SENSOR_FAILED = "11";
    static const std::string ACTIVATE_SENSOR = "12";
    static const std::string ACTIVATE_SENSOR_SUCCESS = "13";
    static const std::string ACTIVATE_SENSOR_FAILED = "14";
    static const std::string REMOVE_SENSOR = "15";
    static const std::string REMOVE_SENSOR_SUCCESS = "16";
    static const std::string REMOVE_SENSOR_FAILED = "17";
    static const std::string ALARM_ACTIVE = "18";
    static const std::string ALARM_INACTIVE = "19";
    static const std::string INFO_REQUEST = "1A";
    static const std::string UPDATE_BATTERY = "1B";
    static const std::string UPDATE_BATTERY_SUCCESS = "1C";
    static const std::string UPDATE_BATTERY_FAILED = "1D";
    static const std::string UPDATE_PIN = "1E";
    static const std::string UPDATE_PIN_SUCCESS = "1F";
    static const std::string UPDATE_PIN_FAILED = "20";
    static const std::string PIN_CHECK = "21";
    static const std::string PIN_CHECK_SUCCESS = "22";
    static const std::string PIN_CHECK_FAILED = "23";
    static const std::string FIREBASE_TOKEN = "24";
    static const std::string FIREBASE_TOKEN_RECEIVED = "25";
    static const std::string STRING_REQUEST = "26";
    static const std::string PIN_FIRST_SETUP = "27";
    static const std::string PIN_FIRST_SETUP_SUCCESS = "28";
    static const std::string PIN_FIRST_SETUP_FAILED = "29";
    static const std::string DEFENSES_ACTIVE = "2A";
    static const std::string DEFENSES_INACTIVE = "2B";
    static const std::string EOM = "//eom";
    static const std::string FAIL = "//null";
    static const std::string STRING = "//·";
    static std::string clear_string_message(std::string msg) {return msg.substr(STRING.length());}
}

