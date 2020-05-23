#include "Core.h"

void testAddSensorToList() {
    Sensor x(0);
    Sensor s(1);
    list<Sensor> l;
    list<Sensor> l1;
    l.push_back(x);
    l.push_back(s);
    Core c(l,l1);
    Sensor f(3);
    cout<<c.addSensorToList(f,l);
    int a;
    cin>>a;
}

void testRemoveSensorFromList() {
    Sensor x(0);
    Sensor s(1);
    list<Sensor> l,l1;
    l.push_back(x);
    l.push_back(s);
    Core c(l,l1);
    Sensor f(3);
    cout<<c.removeSensorFromList(f,l);
    int a;
    cin>>a;
}

int main() {
    testRemoveSensorFromList();
    return 0;
}