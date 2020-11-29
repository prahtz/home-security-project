#pragma once
#include <stdlib.h>
#include <stdio.h>

#include <list>
#include <future>
#include "TCPComm.h"
#include "EventHandler.h"

using namespace std;

class ClientUpdater {
    public:
        static list<future<void>> clientThreads;
        static list<TCPComm*> tcpCommList;
        static void sendUpdatesToClients();
        template<typename T>
        static bool is_ready(std::future<T> const &f) { 
            return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
        }
  
};