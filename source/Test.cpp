#include "Core.h"

void testAddSensorToList() {
    Sensor x(0, OPENED);
    Sensor s(1, OPENED);
    list<Sensor> l;
    list<Sensor> l1;
    l.push_back(x);
    l.push_back(s);
    Core c(l,l1);
    Sensor f(3, OPENED);
    cout<<c.addSensorToList(f,l);
    int a;
    cin>>a;
}

void testRemoveSensorFromList() {
    Sensor x(0, OPENED);
    Sensor s(1, OPENED);
    list<Sensor> l,l1;
    l.push_back(x);
    l.push_back(s);
    Core c(l,l1);
    Sensor f(3, OPENED);
    cout<<c.removeSensorFromList(f,l);
    int a;
    cin>>a;
}

void testIsAlarmReady() {
    Sensor x(0, CLOSED);
    Sensor s(1, CLOSED);
    Sensor f(3, CLOSED);
    list<Sensor> l,l1;
    l.push_back(x);
    l.push_back(s);
    l.push_front(f);
    Core c(l,l1);
    cout<<c.isAlarmReady(COMPLETE);
    int a;
    cin>>a;
}

int main() {
    //testRemoveSensorFromList();
    testIsAlarmReady();
    return 0;
}