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
        static string sendPost(string url, list<string>& header, string httpBody);
};