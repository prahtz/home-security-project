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
        this_thread::sleep_for(std::chrono::milliseconds(1));
        if (rc.available())
        {
            code codeReceived = rc.getReceivedValue();
            cout << "Receiver code: " << codeReceived << endl;
            if (codeReceived != 0)
            {
                critical_section::codesBuffer.with_lock<void>([this, codeReceived, buffSize](list<code>& codesBuffer){
                    if (codesBuffer.size() >= buffSize){
                        cout << codesBuffer.back() << endl;
                        codesBuffer.pop_back();
                    }
                    codesBuffer.push_front(codeReceived);
                });
                critical_section::codeAvailable.notify_all();
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
    return critical_section::codesBuffer->empty();
}

int Receiver::popCodeFromBuffer()
{
    code c = critical_section::codesBuffer.with_lock<code>([this](list<code>& codesBuffer){
        code code = codesBuffer.back();
        codesBuffer.pop_back();
        return code;
    });
    return c;
}