#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace curlpp::options;
using namespace std;

class http_utils {
    public:
        static string sendPost(string url, list<string>& header, string httpBody) {
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
};