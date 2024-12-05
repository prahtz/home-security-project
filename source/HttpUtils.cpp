#include "HttpUtils.h"

string http_utils::sendPost(string url, list<string>& header, string httpBody) {
    stringstream out;
    try
    {
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;

        myRequest.setOpt<Url>(url);
        myRequest.setOpt<CaInfo>("./curl-ca-bundle.crt");
        myRequest.setOpt<HttpHeader>(header);
        myRequest.setOpt<PostFields>(httpBody);
        myRequest.setOpt<PostFieldSize>(httpBody.length());
        myRequest.setOpt<WriteStream>(&out);

        myRequest.perform();
    }
    catch(curlpp::RuntimeError & e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError & e)
    {
        std::cout << e.what() << std::endl;
    }
    string response = out.str();
    return response;
}