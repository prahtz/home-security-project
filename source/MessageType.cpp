#include "MessageType.h"

const std::string MessageType::MESSAGE = "a";
const std::string MessageType::STRING = "b";

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