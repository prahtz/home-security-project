#include "ConnHandler.h"
#include <stdio.h>
#include <cstdlib>
#include <cerrno>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include "PowerLoss.h"

using namespace curlpp::options;


int main(int argc, char *argv[]) {
	try
	{
		/*
		// That's all that is needed to do cleanup of used resources (RAII style).
		curlpp::Cleanup myCleanup;

		// Our request to be sent.
		curlpp::Easy myRequest;

		list<string> header;
		
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		//string httpBody = "{ \"notification\": {\n\"title\": \"5x1\",\n\"body\": \"15:10\"\n}\n\"data\": {\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  },\n\"to\" : \"dKVnNURXTsmisz-mhCo7Ys:APA91bEELnfHR-QAeSfRCSo2VRQYy75YBoYzmQNOWmF-sW95YAxIjCvB7f18KJWjSQdfp6Sh2lhgdbU-N1EpgrI74a4YtcIWWdGYTSWngS6qasgOtlBGihFJ8WE08yZpmK7tk_48eS5A\"\n}";
		string httpBody = "{ \"priority\": \"high\",\n\"data\": {\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"\n \"n_type\":\"battery\"},\n\"to\" : \"dKVnNURXTsmisz-mhCo7Ys:APA91bEELnfHR-QAeSfRCSo2VRQYy75YBoYzmQNOWmF-sW95YAxIjCvB7f18KJWjSQdfp6Sh2lhgdbU-N1EpgrI74a4YtcIWWdGYTSWngS6qasgOtlBGihFJ8WE08yZpmK7tk_48eS5A\"\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/send");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());
		myRequest.perform();
		
		/*
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		header.push_back("project_id:749962426974");
		string httpBody = "{ \"operation\": \"create\",\n\"notification_key_name\": \"androidDevices\",\n\"registration_ids\": [\"cygQYOKOTN2Xmc8jyXFEw4:APA91bEz0yE1iiit7_tQdFMrB9lK2jgwhoVTKkmOklgySv8PS693NvxJlyPhhoS-5A8pV-zw8a_Hc-hwMqYRA6MPa8Bqvv4Z8dijoiaxYcw3LnHP2M02gF28kOf8zJWaRoib5uLHhOjL\"]\n}";
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
		/*
		string notification_key = "APA91bFyPthniy3dbDzXVPAexa26XXSqcfL00xHY3_TNdoJELgADQ_Jw7BHieac7mIVY70duFaPhU-KkWHlTaWZ7ENmoAUevDxk-tw2oe0WdoOMG_-IPf6g";
		//notification_key = "fF05oSO1TlWfAjPNnmYUjF:APA91bErFT-Xv8gKPRqdaFX4nkDZ-jnTi7hnUeUwKt_n8Eh1noHgKI5WaOHAmVvVS15Vxyu1eyLqUnTvBdPpFZ6EN8SNeGKVFbsMg-2TYjzjL5bVOjHRmp0UowaW7eURacB0YH9xi_fw";
		notification_key = "eN3fC93HQo6B6ebFAMt32M:APA91bGzr3NCz9VUgDDugbI0KNtO9FMK7v6iID-h0BAXjjLByAev4vJDyEKYNhyVV-eJowWda49GbgaeutE4fgLiJx6qxrnyu4VLwj2hrENGfMwYsM2GRE4LCS6BQdRV74ce2Brmc0f5";
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		//string httpBody = "{ \"notification\": {\n\"title\": \"5x1\",\n\"body\": \"15:10\"\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  },\n\"to\" : \"cY6GVFLoQJS9l0-ZXsiBB8:APA91bE9ZKK3YiTkl4qUvSaIqZKuO007IKzLmMKS763zB8Kfrn2TkDlUKlJeilBp1gTEF6cvNF2k5nAzdvwcE3vLP4jKJr76FXellDv1EWjF452NuJm1yA6EZn7Zc8FdCSyvCddyzrJM\"\n}";
		string httpBody = "{ \"priority\": \"high\",\n\"data\": {\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  }, \"android\": { \"ttl\": \"0\"},\n\"to\" : \"" + notification_key +"\"\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/send");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());
		myRequest.perform();


		notification_key = "fF05oSO1TlWfAjPNnmYUjF:APA91bErFT-Xv8gKPRqdaFX4nkDZ-jnTi7hnUeUwKt_n8Eh1noHgKI5WaOHAmVvVS15Vxyu1eyLqUnTvBdPpFZ6EN8SNeGKVFbsMg-2TYjzjL5bVOjHRmp0UowaW7eURacB0YH9xi_fw";
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		//string httpBody = "{ \"notification\": {\n\"title\": \"5x1\",\n\"body\": \"15:10\"\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  },\n\"to\" : \"cY6GVFLoQJS9l0-ZXsiBB8:APA91bE9ZKK3YiTkl4qUvSaIqZKuO007IKzLmMKS763zB8Kfrn2TkDlUKlJeilBp1gTEF6cvNF2k5nAzdvwcE3vLP4jKJr76FXellDv1EWjF452NuJm1yA6EZn7Zc8FdCSyvCddyzrJM\"\n}";
		httpBody = "{ \"priority\": \"high\",\n\"data\": {\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  }, \"android\": { \"ttl\": \"0\"},\n\"to\" : \"" + notification_key +"\"\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/send");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());
		myRequest.perform();

		notification_key = "c3USeT5SSzqbH0yX41T9Ki:APA91bFZDvibMHQ5_3YZ1OmehfUxeQmDQ7_Jk5o6kB6SV_PhMTntCD8Kn1u0F8BN6l5vhw0BY8f-YyMKdsaV_-_bRo8-OywrOC3XnslThr890ipxzXjeBJSC_gs6_QFhT_ZWwCqjt8xa";
		header.push_back("Content-Type:application/json");
		header.push_back("Authorization:key=AAAArp0-el4:APA91bFPiEP_Q0o8J6jP_9EvuUpFjcEqe-IG9XQF62WCiqJO9NJH7F_Qusdv2vGf1qiyvAIm5LqWUOkZvZLXpGrar1x34rCvzOvnyp6IUvr4H_mXqfCHoNoHyKP3BbRqwGTu4Q1HSOmR");
		//string httpBody = "{ \"notification\": {\n\"title\": \"5x1\",\n\"body\": \"15:10\"\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  },\n\"to\" : \"cY6GVFLoQJS9l0-ZXsiBB8:APA91bE9ZKK3YiTkl4qUvSaIqZKuO007IKzLmMKS763zB8Kfrn2TkDlUKlJeilBp1gTEF6cvNF2k5nAzdvwcE3vLP4jKJr76FXellDv1EWjF452NuJm1yA6EZn7Zc8FdCSyvCddyzrJM\"\n}";
		httpBody = "{ \"priority\": \"high\",\n\"data\": {\n\"click_action\": \n \"FLUTTER_NOTIFICATION_CLICK\"  }, \"android\": { \"ttl\": \"0\"},\n\"to\" : \"" + notification_key +"\"\n}";
		// Set the URL.
		myRequest.setOpt<Url>("https://fcm.googleapis.com/fcm/send");
		myRequest.setOpt<HttpHeader>(header);
		myRequest.setOpt<PostFields>(httpBody);
      	myRequest.setOpt<PostFieldSize>(httpBody.length());
		myRequest.perform();*/

		/*\"notification\": { \"body\": \"allarme\", \"title\": \"we\" }*/
	}
	catch(curlpp::RuntimeError & e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(curlpp::LogicError & e)
	{
		std::cout << e.what() << std::endl;
	}

	if(argc != 3) {
		cout << "Invalid arguments" << endl;
		return -1;
	}
	PowerLoss::startService();
    ConnHandler ch(argv[1], argv[2]);
    return 0;
}