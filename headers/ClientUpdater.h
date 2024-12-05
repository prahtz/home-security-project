#pragma once
#include <stdlib.h>
#include <stdio.h>

#include <list>
#include <future>
#include "TCPComm.h"
#include "CriticalSection.h"

using namespace std;

class ClientUpdater {
    public:
        ClientUpdater();
        list<future<void>> clientThreads;
        list<TCPComm> tcpCommList;
        void sendUpdatesToClients();
        template<typename T>
        bool is_ready(std::future<T> const &f) { 
            return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
        }
  
};