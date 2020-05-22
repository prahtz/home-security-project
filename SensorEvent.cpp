#include "SensorEvent.h"

SensorEvent::SensorEvent(Sensor s, Action a) {
    this->s = s;
    this->a = a;
}

Sensor SensorEvent::getSensor() {
    return s;
}

Action SensorEvent::getAction() {
    return a;
}
