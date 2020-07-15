#include "Receiver.h"

Receiver::Receiver()
{
    stopReceive = false;
}

void Receiver::startReceiving()
{
    wiringPiSetup();
    rc.enableReceive(RECEIVER_PIN);
    int buffSize = 100;
    stopReceive = false;
    while (!stopReceive)
    {
        if (rc.available())
        {
            code codeRecieved = rc.getReceivedValue();
            cout << codeRecieved << endl;
            if (codeRecieved != 0)
            {
                if (codesBuffer.size() >= buffSize)
                {
                    cout << codesBuffer.back() << endl;
                    codesBuffer.pop_back();
                }
                codesBuffer.push_front(codeRecieved);
                codeAvailable.notify_all();
            }
            rc.resetAvailable();
        }
    }
}

void Receiver::stopReceiving()
{
    stopReceive = true;
}

bool Receiver::isBufferEmpty()
{
    return codesBuffer.empty();
}

int Receiver::popCodeFromBuffer()
{
    code code = codesBuffer.back();
    codesBuffer.pop_back();
    return code;
}