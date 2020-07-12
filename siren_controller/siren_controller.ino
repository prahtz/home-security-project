#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
int gatePin = 12;
int txTransmitter = 13;
int txTamper = 11;
int rxTamper = 10;
bool tamperActive = false;
bool tamperEnabled = true;
unsigned long activateSirenCode = 14152368;
unsigned long deactivateSirenCode = 14476512;
unsigned long tamperActiveCode = 16557824;
unsigned long enableTamperCode = 10286572;

unsigned long ackActivateCode = 13354356;
unsigned long ackDeactivateCode = 13357324;
unsigned long ackControlUnitCode = 12343212;
unsigned long ackEnableTamperCode = 16752004;

unsigned long transmitDelay = 3000;

void activateSiren() {
  bool sirenActivated = true;
  //mySwitch.send(ackActivateCode, 24);
  while (sirenActivated) {
    digitalWrite(gatePin, LOW);
    if (mySwitch.available()) {
      unsigned long receivedValue = mySwitch.getReceivedValue();
      mySwitch.resetAvailable();
      if (receivedValue == deactivateSirenCode) {
        sirenActivated = false;
        mySwitch.send(ackDeactivateCode, 24);
      }
      else if (receivedValue == activateSirenCode) {
        Serial.println("ACTIVE ");
        mySwitch.send(ackActivateCode, 24);
      }
    }
  }
}

void activateTamper() {
  bool sirenActivated = true;
  bool ackReceived = false;
  unsigned long startMillis = millis();
  mySwitch.send(tamperActiveCode, 24);
  
  while (sirenActivated) {
    digitalWrite(gatePin, LOW);
    if (!ackReceived && millis() - startMillis >= transmitDelay) {
      mySwitch.send(tamperActiveCode, 24);
      startMillis = millis();
    }
    if (mySwitch.available()) {
      unsigned long receivedValue = mySwitch.getReceivedValue();
      mySwitch.resetAvailable();
      Serial.println(String(receivedValue));
      if (receivedValue == deactivateSirenCode) {
        sirenActivated = false;
        mySwitch.send(ackDeactivateCode, 24);
      }
      if (receivedValue == ackControlUnitCode) {
        ackReceived = true;
      }
    }
  }
}


void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);
  mySwitch.setProtocol(1);
  mySwitch.enableTransmit(13);

  pinMode(gatePin, OUTPUT);
  pinMode(txTransmitter, OUTPUT);
  pinMode(txTamper, OUTPUT);
  pinMode(rxTamper, INPUT);
  digitalWrite(txTamper, HIGH);
  delay(100);
}

void loop() {
  digitalWrite(gatePin, HIGH);
  digitalWrite(txTamper, HIGH);

  int tamperValue = digitalRead(rxTamper);
  if (tamperEnabled) {
    if (tamperValue == LOW) {
      activateTamper();
      tamperEnabled = false;
    }
  }

  if (mySwitch.available()) {
    unsigned long receivedValue = mySwitch.getReceivedValue();
    mySwitch.resetAvailable();
    if (receivedValue == activateSirenCode) {
      Serial.println("ACTIVE ");
      //mySwitch.send(ackActivateCode, 24);
      activateSiren();
    }
    else if (receivedValue == deactivateSirenCode) {
      Serial.println("DEACTIVE ");
      mySwitch.send(ackDeactivateCode, 24);
    }
    else if (receivedValue == enableTamperCode) {
      Serial.println("ENABLE TAMPER ");
      tamperEnabled = true;
      mySwitch.send(ackEnableTamperCode, 24);
    }
  }
}
