#include <string>
#include <list>
#include "Definitions.h"

using namespace std;

class FirebaseNotification {
    private:
        string authKey = "AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR";
        string url = "https://fcm.googleapis.com/fcm/send";

        list<string> header;
        string title = "";
        string body = "";
        string token = "APA91bFyPthniy3dbDzXVPAexa26XXSqcfL00xHY3_TNdoJELgADQ_Jw7BHieac7mIVY70duFaPhU-KkWHlTaWZ7ENmoAUevDxk-tw2oe0WdoOMG_-IPf6g";
    public:
        FirebaseNotification();
        string getUrl();
        string getBody();
        string getTitle();
        string getToken();
        list<string> getHeader();
        string getHttpBody();
        
        void setUrl(string url);
        void setTitle(string title);
        void setBody(string body);
        void setToken(string token);
        void addHeaderEntry(string entry);
};