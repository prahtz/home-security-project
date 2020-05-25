#include "Sensor.h"
#include "Action.h"

class SensorEvent {
    private:
        Sensor s;
        Action a;
    public:
        SensorEvent(Sensor s, Action a);
        Sensor getSensor();
        Action getAction();
};