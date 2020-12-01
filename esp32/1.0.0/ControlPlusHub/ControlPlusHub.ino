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
Lpf2Hub mySwitch;
byte portC = (byte)ControlPlusHubPort::C; //1
byte portD = (byte)ControlPlusHubPort::D; //2

int switchInterval = 220;
int switchVelocity = 35;


void readFromSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
	Serial.println(">" + command);
    if (command == "swa0") setSwitch(portC,switchVelocity);
    else if(command == "swa1") setSwitch(portC,-1*switchVelocity)
    else if(command == "swb0") setSwitch(portD,-1*switchVelocity);
    else if(command == "swb1") setSwitch(portD,switchVelocity)
	else{
		Serial.println(">command not found");
	}
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("ready");
} 


// main loop
void loop() {
		

  if (!mySwitch.isConnected() && !mySwitch.isConnecting()) 
  {
    mySwitch.init(); 
  }

  // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
  if (mySwitch.isConnecting()) {
    mySwitch.connectHub();
    if (mySwitch.isConnected()) {
      Serial.println("Connected to HUB");
	  char hubName[] = "Switch";
	  mySwitch.setHubName(hubName);
  	
    } else {
      Serial.println("Failed to connect to HUB");
    }
  }

  // if connected, you can set the name of the hub, the led color and shut it down
  if (mySwitch.isConnected()) {
	  	
	readFromSerial();        

  } else {
    Serial.println("ControlPlus hub is disconnected");
  }
  
} // End of loop


void setSwitch(byte port, int velocity){	

	_println("setSwitch");
	_println(port.toString().c_str());
    
    mySwitch.setTachoMotorSpeed(port, velocity);
    delay(switchInterval);
    mySwitch.stopTachoMotor(port);
    
}



void _print(String text) {
  if (isVerbose) Serial.print(text);
}

void _println(String text) {
  if (isVerbose) Serial.println(text);
}