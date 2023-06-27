#include "PowerLoss.h"
void PowerLoss::startService() {
    wiringPiSetup();
    wiringPiISR(PIN, INT_EDGE_RISING, PowerLoss::callback);
}

void PowerLoss::callback() {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    int samplingTime = 10;
    int pwr_loss = 0, pwr_stable = 0;
    double pwr_loss_thr = 0.75;
    while(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - begin).count() < samplingTime) {
        int r = digitalRead(PIN);
        if(r == LOW_VALUE)
            pwr_loss++;
        else
            pwr_stable++;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if((double) pwr_loss / (pwr_loss + pwr_stable) >= pwr_loss_thr) {
        sendNotifications();
    }
}

void PowerLoss::sendNotifications() {
    for(string token : Core::tokenList) {
        FirebaseNotification* notification = new FirebaseNotification();
        notification->setTitle("CORRENTE ASSENTE!");
        notification->setBody("Rilevata assenza di corrente presso la centralina.");
        notification->setToken(token);
        notification->setTTL("18000s");
        notification->setNType("battery");
        FirebaseMessagesHandler::addMessage(notification);
    }
    statical::newFirebaseNotification.notify_all();
    Logger::log("Power loss detected");
}