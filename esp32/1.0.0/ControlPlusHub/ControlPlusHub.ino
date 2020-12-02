/**
 * A Legoino example to control a control plus hub
 * with a Motor connected on Port D
 * 
 * (c) Copyright 2020 - Cornelius Munz
 * Released under MIT License
 * 
 */
 
 /* ver 1.1 */

#include "Lpf2Hub.h"

// create a hub instance
Lpf2Hub mySwitch;
byte portC = (byte)ControlPlusHubPort::C; //0 -> Yellow
byte portD = (byte)ControlPlusHubPort::D; //1 -> Red
int switchInterval = 220;
bool isVerbose = true;


typedef struct {
  byte port;
  String switchColor;
  bool switchState;  
  int switchVelocity_straight;
  int switchVelocity_change;  
} Switches;


#define MY_SWITCH_LEN 2

//port  - color  -  status (0= straight 1= change) - vel_str - vel_change 
Switches mySwitches[MY_SWITCH_LEN] = {
  { portC, "Yellow" , 0, 35, 0},
  { portD, "Red" , 0, -35, 0}  
};




void readFromSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
	Serial.println(">" + command);
    if (command == "swa0") setSwitch(mySwitches[0],0);
    else if(command == "swa1") setSwitch(mySwitches[0],1);
    else if(command == "swb0") setSwitch(mySwitches[1],0);
    else if(command == "swb1") setSwitch(mySwitches[1],1);
	else if(command == "resetsw") resetSwitch();
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


void setSwitch(Switches cSwitch, bool position){	

	// position 0=straight, 1= change

	 _println("setSwitch");
	 Serial.println(cSwitch.port,DEC);
	 
	 if(cSwitch.switchState == position){
		_println("already setted");
		return;
	 }
	 
	 int velocity = position ? cSwitch.switchVelocity_change : cSwitch.switchVelocity_straight;
    
	mySwitch.setTachoMotorSpeed(cSwitch.port, velocity);
	delay(switchInterval);
	mySwitch.stopTachoMotor(cSwitch.port);
	
	cSwitch.switchState = position;
    
}



void _print(String text) {
  if (isVerbose) Serial.print(text);
}

void _println(String text) {
  if (isVerbose) Serial.println(text);
}

void resetSwitch() {
  for (int idSwitch = 0; idSwitch < MY_SWITCH_LEN; idSwitch++) {   
	  setSwitch(mySwitches[idSwitch],0);  		
  }
}
