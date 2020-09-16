/**
 * A Legoino example to control a train which has a motor connected
 * to the Port A of the Hub
 * 
 * (c) Copyright 2019 - Cornelius Munz
 * Released under MIT License
 * 
 */

#include "PoweredUpHub.h"

// create a hub instance
PoweredUpHub myTrainHub1;
PoweredUpHub myTrainHub2;

PoweredUpHub::Port _port = PoweredUpHub::Port::A;


void setup() {
    Serial.begin(115200);
} 


// main loop
void loop() {

  if (!myTrainHub1.isConnected() && !myTrainHub1.isConnecting()) 
  {
    myTrainHub1.init(); // initalize the PoweredUpHub instance
    myTrainHub1.init("90:84:2B:04:A8:C5"); //example of initializing an hub with a specific address
  }

  // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
  if (myTrainHub1.isConnecting()) {
    myTrainHub1.connectHub();
    if (myTrainHub1.isConnected()) {
      Serial.println("Connected to HUB1");
      myTrainHub2.init("90:84:2B:1C:BE:CF");
    } else {
      Serial.println("Failed to connect to HUB");
    }
  }
  

  if (myTrainHub2.isConnecting()) {
    myTrainHub2.connectHub();
    if (myTrainHub2.isConnected()) {
      Serial.println("Connected to HUB2");     
    } else {
      Serial.println("Failed to connect to HUB");
    }
  }


  if (myTrainHub1.isConnected() && myTrainHub1.isButtonPressed()){
    Serial.println("Button Hub1 is pressed:" );
    delay(3000);
  }
  if (myTrainHub2.isConnected() && myTrainHub2.isButtonPressed()){
    Serial.println("Button Hub2 is pressed:" );
    delay(3000);
  }

  
} // End of loop
