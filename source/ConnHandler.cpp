#include "ConnHandler.h"

ConnHandler::ConnHandler()
{
    setupServerSocket();
    startClientServerComunication();
}

ConnHandler::ConnHandler(string lan_IP, string port)
{
    this->lan_IP = lan_IP;
    this->port = port;
    try
    {
        setupServerSocket();
        core.startService();
        startClientServerComunication();
    }
    catch (BindFailedException e)
    {
        std::cout << e.what() << std::endl;
    }
}

void ConnHandler::setupServerSocket()
{
    struct sockaddr_in serv_addr, cli_addr;
    serverSocket = socket(DOMAIN, TRANSPORT, 0);
    bzero((char *)&serv_addr, sizeof(serv_addr));
    int portno = atoi(port.c_str());
    struct linger l;
    l.l_onoff = 1;
    l.l_linger = 0;
    int reuseaddr = 1;
    int reuseport = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(lan_IP.c_str());
    serv_addr.sin_port = htons(portno);
    std::cout << this->lan_IP << " " << this->port << std::endl;
    if (bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        throw BindFailedException("Bind failed");

    if (listen(serverSocket, MAX_CLIENTS) < 0)
        throw BindFailedException("Listen failed");
}

void ConnHandler::startClientServerComunication()
{
    while (true)
    {
        int clientSocket = INVALID_SOCKET;
        while (clientSocket == INVALID_SOCKET)
            clientSocket = accept(serverSocket, NULL, NULL);
        initClientSocket(clientSocket);
        critical_section::clientUpdater.with_lock<void>([this, clientSocket](ClientHandler& clientUpdater) {
            list<future<void>>::iterator it = clientUpdater.clientThreads.begin();
            list<TCPComm>::iterator itTCP = clientUpdater.tcpCommList.begin();
            while (it != clientUpdater.clientThreads.end())
            {
                if (clientUpdater.is_ready((*it)))
                {
                    (*it).wait();
                    it = clientUpdater.clientThreads.erase(it);
                    itTCP = clientUpdater.tcpCommList.erase(itTCP);
                }
                else
                {
                    ++it;
                    ++itTCP;
                }
            }
            
            clientUpdater.tcpCommList.emplace_back(clientSocket);
            TCPComm &tcpComm = clientUpdater.tcpCommList.back();
            clientUpdater.clientThreads.emplace_back(std::async(std::launch::async, &ConnHandler::clientThread, this, clientSocket, std::ref(tcpComm)));
        });
    }
    closesocket(serverSocket);
}

void ConnHandler::initClientSocket(int clientSocket)
{
    int yes = 1;
    setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));
    int idle = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

    int interval = 1;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

    int maxpkt = 10;
    setsockopt(clientSocket, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
}

void ConnHandler::clientThread(int clientSocket, TCPComm &tcpComm)
{
    Subscription<string> &sub = tcpComm.getMessageStream().listen([this, &tcpComm](string message) {
        core.getMutex().lock();
        if (message == message::ACTIVATE_ALARM)
            core.activateAlarm(tcpComm);
        else if (message == message::DEACTIVATE_ALARM)
            core.deactivateAlarm(tcpComm);
        else if (message == message::REGISTER_DOORSENSOR)
            core.registerNewDoorSensor(tcpComm);
        else if (message == message::SENSOR_LIST)
            core.sensorList(tcpComm);
        else if (message == message::INFO_REQUEST)
            critical_section::clientUpdater->sendUpdatesToClients();
        else if (message == message::DEACTIVATE_SENSOR)
            core.deactivateSensor(tcpComm);
        else if (message == message::ACTIVATE_SENSOR)
            core.activateSensor(tcpComm);
        else if (message == message::REMOVE_SENSOR)
            core.removeSensor(tcpComm);
        else if (message == message::UPDATE_BATTERY)
            core.updateBattery(tcpComm);
        else if (message == message::FIREBASE_TOKEN)
            core.handleFirebaseToken(tcpComm);
        else if (message == message::UPDATE_PIN)
            core.setupNewPIN(tcpComm);
        else if(message == message::PIN_FIRST_SETUP){
            core.setupFirstPIN(tcpComm);
        }
        else if(message.find(message::REPEATER_CODE) != string::npos) {
            core.addRepeaterCode(message);
        }
        core.getMutex().unlock();
    });
    tcpComm.startReceive();
    closesocket(clientSocket);
}