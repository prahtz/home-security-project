
#include <string>

class MessageType {
    private:
        static MessageType* instance;
        MessageType();
    public:
        static const std::string MESSAGE;
        static const std::string STRING;
        static MessageType* getInstance();
};

