#include "ConnHandler.h"
#include <stdio.h>

#include <cstdlib>
#include <cerrno>



#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

using namespace curlpp::options;


int main() {
	try
	{
		// That's all that is needed to do cleanup of used resources (RAII style).
		curlpp::Cleanup myCleanup;

		// Our request to be sent.
		curlpp::Easy myRequest;

		list<string> header;
		/*
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		string httpBody = "{ \"notification\": {\n\"title\": \"5x1\",\n\"body\": \"15:10\"\n},\n\"to\" : \"cSyIBvc7S0G3c9JZZHtI7V:APA91bFwEICEonE8wKLoe3c-1AZeSJtIQtq7ajBIMGRKW5GbKc--6fmL7rYlo8hnz4mv_oYEWEPOL9dO0cA2HC_dZoWSSqdGyU4Yey1zEjl_8pZr8yQYZGWRKFrb8j6PATydE0IqYOCJ\"\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/send");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());*/
		
		/*
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		header.push_back("project_id:749962426974");
		string httpBody = "{ \"operation\": \"create\",\n\"notification_key_name\": \"androidDevices\",\n\"registration_ids\": [\"cSyIBvc7S0G3c9JZZHtI7V:APA91bFwEICEonE8wKLoe3c-1AZeSJtIQtq7ajBIMGRKW5GbKc--6fmL7rYlo8hnz4mv_oYEWEPOL9dO0cA2HC_dZoWSSqdGyU4Yey1zEjl_8pZr8yQYZGWRKFrb8j6PATydE0IqYOCJ\"]\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/notification");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());
		string notification_key_name = "androidDevices";
		string notification_key = "APA91bFyPthniy3dbDzXVPAexa26XXSqcfL00xHY3_TNdoJELgADQ_Jw7BHieac7mIVY70duFaPhU-KkWHlTaWZ7ENmoAUevDxk-tw2oe0WdoOMG_-IPf6g";
		*/

		/*
		string notification_key = "APA91bFyPthniy3dbDzXVPAexa26XXSqcfL00xHY3_TNdoJELgADQ_Jw7BHieac7mIVY70duFaPhU-KkWHlTaWZ7ENmoAUevDxk-tw2oe0WdoOMG_-IPf6g";
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		string httpBody = "{ \"notification\": {\n\"title\": \"5x1\",\n\"body\": \"15:10\"\n},\n\"to\" : \"" + notification_key + "\"\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/send");
		myRequest.setOpt<CaInfo>("./curl-ca-bundle.crt");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());
		// Send request and get a result.
		// By default the result goes to standard output.
		myRequest.perform();*/
	}
	catch(curlpp::RuntimeError & e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(curlpp::LogicError & e)
	{
		std::cout << e.what() << std::endl;
	}

    ConnHandler ch;
    return 0;
}