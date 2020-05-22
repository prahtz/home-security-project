

class Sensor {
    private:
        int sensorID;
    public:
        Sensor(){sensorID = -1;};
        Sensor(int sensorID);
        int getSensorID();
};