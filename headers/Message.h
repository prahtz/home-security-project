
#include <string>

class Message{
    private:
        static Message* instance;
        Message();
    public:
        static const std::string ACTIVATE_ALARM;
        static const std::string DEACTIVATE_ALARM;
        static const std::string REGISTER_DOORSENSOR;
        static const std::string SENSOR_LIST;
        static const std::string ACK;
        static const std::string ABORT;
        static const std::string NEXT_CODE;
        static const std::string REGISTER_SUCCESS;
        static Message* getInstance();
};
