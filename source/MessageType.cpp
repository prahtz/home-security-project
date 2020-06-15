#include "MessageType.h"

const std::string MessageType::MESSAGE = "0";
const std::string MessageType::STRING = "1";

MessageType* MessageType::instance = 0;

MessageType* MessageType::getInstance()
{
    if (instance == 0)
    {
        instance = new MessageType();
    }

    return instance;
}

MessageType::MessageType()
{}