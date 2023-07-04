#pragma once

#include <string>
#include <list>
#include <fstream>
#include <filesystem>
#include <map>
#include <numeric>

#include "Sensor.h"
#include "Definitions.h"
#include "Locking.hpp"
#include "DoorSensor.h"
#include "Action.h"

using namespace std;

class SensorsHandler
{
private:
    list<Sensor*> sensorList;
    map<code, pair<Action, Sensor*>*> codeMap;

public:
    void updateCodeMap(Sensor* s)
    {
        list<pair<code, Action>> codeActionList = s->getCodeActionList();
        for (auto it = codeActionList.begin(); it != codeActionList.end(); ++it)
            codeMap[(*it).first] = new pair<Action, Sensor*>((*it).second, s);
    }

    void setupKnownSensors()
    {
        std::filesystem::create_directory(PATH);
        ifstream readingFile(KNOWN_PATH);
        string line;
        if (readingFile.is_open())
        {
            while (getline(readingFile, line))
            {
                istringstream streamString(line);
                string field;
                getline(streamString, field, ';');
                switch (stoi(field))
                {
                case DOOR_SENSOR:
                    DoorSensor* ds = new DoorSensor();
                    getline(streamString, field, ';');
                    ds->setSensorID(stoi(field));
                    getline(streamString, field, ';');
                    ds->setSensorState((State)stoi(field));
                    getline(streamString, field, ';');
                    ds->isEnabled((bool)stoi(field));
                    getline(streamString, field, ';');
                    ds->isCharged((bool)stoi(field));
                    getline(streamString, field, ';');
                    ds->setOpenCode(stoi(field));
                    getline(streamString, field, ';');
                    ds->setCloseCode(stoi(field));
                    getline(streamString, field);
                    ds->setSensorName(field);
                    sensorList.push_back(ds);
                    updateCodeMap(ds);
                    break;
                }
            }
            readingFile.close();
        }
        else
        {
            ofstream createdFile(KNOWN_PATH);
            createdFile.close();
        }
    };

    void updateKnownFile() {
        ofstream out(KNOWN_PATH, ios::trunc);
        for (Sensor* s : sensorList)
            s->writeToFile(out);
        out.close();
    }

    bool addSensorToList(Sensor* s)
    {
        list<Sensor*>::iterator it = std::find_if(sensorList.begin(), sensorList.end(), [s](Sensor* sensor)
                                                   { return sensor->getSensorID() == s->getSensorID(); });
        if (it == sensorList.end())
        {
            sensorList.push_back(s);
            updateCodeMap(s);
            updateKnownFile();
            return true;
        }
        return false;
    }

    bool removeSensorFromList(int sensorID)
    {
        list<Sensor *>::iterator it = std::find_if(
            sensorList.begin(), 
            sensorList.end(), 
            [sensorID](Sensor *sensor) { return sensor->getSensorID() == sensorID; });
        if (it != sensorList.end())
        {
            for (pair<code, Action> codeActionPair : (*it)->getCodeActionList())
                codeMap.erase(codeActionPair.first);
            sensorList.erase(it);
            updateKnownFile();
            return true;
        }
        return false;
    }

    bool allSensorsReady() {
        return std::find_if(
            sensorList.begin(), 
            sensorList.end(), 
            [](Sensor* s) { 
                return !s->isSensorReady(); 
            }
        ) == sensorList.end();
    }

    int getNewSensorID() {
        return std::accumulate(
            sensorList.begin(), 
            sensorList.end(),
            0,
            [](int acc, Sensor* s) {return max(acc, s->getSensorID());}
        ) + 1;
    }

    list<Sensor*>& getSensorList()
    {
        return sensorList;
    }

    map<code, pair<Action, Sensor*>*>& getCodeMap()
    {
        return codeMap;
    }
};