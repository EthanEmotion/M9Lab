/**
 * A Legoino example to control a control plus hub
 * with a Motor connected on Port D
 * 
 * (c) Copyright 2020 - Cornelius Munz
 * Released under MIT License
 * 
 */

#include "Lpf2Hub.h"

// create a hub instance
Lpf2Hub myHub;
byte portD = (byte)ControlPlusHubPort::D;

void setup() {
    Serial.begin(115200);
    Serial.println("ready");
} 


// main loop
void loop() {

  if (!myHub.isConnected() && !myHub.isConnecting()) 
  {
    myHub.init(); 
  }

  // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
  if (myHub.isConnecting()) {
    myHub.connectHub();
    if (myHub.isConnected()) {
      Serial.println("Connected to HUB");
    } else {
      Serial.println("Failed to connect to HUB");
    }
  }

  // if connected, you can set the name of the hub, the led color and shut it down
  if (myHub.isConnected()) {

    char hubName[] = "myControlPlusHub";
    myHub.setHubName(hubName);
  
    myHub.setLedColor(GREEN);
    delay(1000);
    myHub.setLedColor(RED);

    //sw
    delay(1000);
    myHub.setTachoMotorSpeed(portD, 35);
    delay(220);
    myHub.stopTachoMotor(portD);
    delay(5000);
    // stra
    myHub.setTachoMotorSpeed(portD, -35);
    delay(220);
    myHub.stopTachoMotor(portD);
    delay(5000);

  } else {
    Serial.println("ControlPlus hub is disconnected");
  }
  
} // End of loop
