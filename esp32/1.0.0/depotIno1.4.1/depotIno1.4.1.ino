
// Questo sketch gestisce 3 treni (A,B,C) su un tracciato che va dal deposito alla galleria 
// i treni partono una alla volta in maniera casuale
// utilizza lego poweredup con la libreria Legoino by Cornelius Munz  (https://github.com/corneliusmunz/legoino)
// necessita di 3 hub city per i treni (motore + sensore colore) e un hub technic (3 motori) per gli scambi.
// TrenIno4 - 2020 Code by Stefx

// test track all ok

// TODO:
// scambi per far entrare treni o mandarli


#include "Lpf2Hub.h"

String ver = "1.4.1";

// create a hub instance for train
Lpf2Hub myTrainHub_TA;
Lpf2Hub myTrainHub_TB;
Lpf2Hub myTrainHub_TC;
byte portA = (byte)PoweredUpHubPort::A;
byte portB = (byte)PoweredUpHubPort::B;
int trainSpeed = 25;
int connectedTrain = 0;
int colorInterval = 5000;
int beforeStartInterval = 5000;
int lastTrainStarted = -1;

// create a hub instance for switch
Lpf2Hub mySwitchController;
byte pPortC = (byte)ControlPlusHubPort::C; //0 -> A) Black (C)
byte pPortD = (byte)ControlPlusHubPort::D; //1 -> B) Orange (D)
byte pPortA = (byte)ControlPlusHubPort::A; //2 -> C) White (A) // battery shed
int switchInterval = 250;
int switchVelocity = 35;
String switchControllerAddress = "90:84:2b:51:ba:b0";

// global
bool isSystemReady = false;
bool isVerbose = false;


// Trains structure
typedef struct {
  Lpf2Hub* hubobj;
  String hubColor;
  String hubAddress;
  int speed;
  int lastcolor;
  unsigned long colorPreviousMillis;
  int hubState;
  int trainState;
  int batteryLevel;
} Train;

// Switches structure
typedef struct {
  byte* port;
  String switchColor;
  bool switchState;  
} Switches;


#define MY_TRAIN_LEN 3
#define MY_SWITCH_LEN 3
#define MY_COLOR_LEN 3

// exclude black (trail) and RED (table) ..for now
//GREEN instead CYAN -> to add
//  (byte)Color::BLUE,(byte)Color::YELLOW

// Color Maps
byte sensorAcceptedColors[MY_COLOR_LEN] = {(byte)Color::WHITE, (byte)Color::CYAN,  (byte)Color::RED};

// Trains Maps
//code  - hubobj - hubColor  -  hubAddress - speed - lastcolor - hubState (-1 = off, 0=ready, 1=active) - trainstate - batteryLevel
Train myTrains[MY_TRAIN_LEN] = {
  { &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", trainSpeed, 0, 0, -1, 0, 100}
  , { &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", trainSpeed, 0, 0 , -1, 0, 100}
  , { &myTrainHub_TC, "Green", "90:84:2b:16:9a:1f", trainSpeed, 0, 0 , -1, 0, 100}
};


// Switch Maps
//port  - color  -  status (0= straight 1= change) - vel_str - vel_change 
Switches mySwitchControlleres[MY_SWITCH_LEN] = {
  { &pPortC, "Black" , 0, }, //primo switch
  { &pPortD, "Orange" , 0,}, // secondo switch
  { &pPortA, "White" , 0, } // battery shed switch
};



void printLegenda() {

  // print command
  Serial.println("*** M9Lab - DepotIno v." + ver + " ***");
  Serial.println("_________________________________________________");
  Serial.println("List of commands used by system:");

  Serial.println("on = set system on");
  Serial.println("off = set system off");
  Serial.println("panic = shutDown all hubs and reset the system");
  Serial.println("reset = reset the system");
  Serial.println("status = show system status");
  Serial.println("help = show this message again");
  Serial.println("verboseon = show more status messages");
  Serial.println("verboseoff = show less status messages");
  
  Serial.println("swa0 = show less status messages");
  Serial.println("swa1 = show less status messages");
  Serial.println("swb0 = show less status messages");
  Serial.println("swb1 = show less status messages");
  Serial.println("swc0 = show less status messages");
  Serial.println("swc1 = show less status messages");
  Serial.println("resetsw = show less status messages");

  Serial.println("_________________________________________________");
}

void readFromSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
	Serial.println(">" + command);
    if (command == "panic") panic();
    else if(command == "reset") systemReset();
    else if(command == "on") systemOn();
    else if(command == "off") systemOff();
    else if(command == "help") printLegenda();
    else if(command == "status") systemStatus();
	else if(command == "verboseon") verboseOn();
	else if(command == "verboseoff") verboseOff();
	else if (command == "swa0") setSwitch(&mySwitchControlleres[0],0);
    else if(command == "swa1") setSwitch(&mySwitchControlleres[0],1);
    else if(command == "swb0") setSwitch(&mySwitchControlleres[1],0);
    else if(command == "swb1") setSwitch(&mySwitchControlleres[1],1);
    else if(command == "swc0") setSwitch(&mySwitchControlleres[2],0);
    else if(command == "swc1") setSwitch(&mySwitchControlleres[2],1);
	else if(command == "resetsw") resetSwitch();
	else{
		Serial.println(">command not found");
	}
  }
}


void setSwitch(Switches *cSwitch, bool position){	

  byte *myPort = (byte *)cSwitch->port;

  	// position 0=straight, 1= change
	
	 if(cSwitch->switchState == position){
		_println("already setted");
		return;
	 }
	 
	 int velocity = position ? switchVelocity : (switchVelocity * -1);  

	mySwitchController.setLedColor(YELLOW);    
	mySwitchController.setTachoMotorSpeed(*myPort, velocity);
	delay(switchInterval);
	mySwitchController.stopTachoMotor(*myPort);
	mySwitchController.setLedColor(BLUE);    	
	cSwitch->switchState = position;
    
}

void resetSwitch() {
  for (int idSwitch = 0; idSwitch < MY_SWITCH_LEN; idSwitch++) {   
	  setSwitch(&mySwitchControlleres[idSwitch],0);  		
  }
}


void verboseOn() {
  isVerbose = true;
}

void verboseOff() {
  isVerbose = false;
}

void systemOn() {
  if(!mySwitchController.isConnected()){
	  _println("Cannot find the Switch Controller");
  }else{
	  isSystemReady = true;
  }
  
}

void systemOff() {
  isSystemReady = false;
}

void systemReset() {
  //TODO
  systemOff();
  for (int idTrain = 0; idTrain < MY_TRAIN_LEN; idTrain++) {
    stopTrain(idTrain);
    myTrains[idTrain].speed = 0;
    myTrains[idTrain].lastcolor = 0;
    myTrains[idTrain].colorPreviousMillis = 0;
  }
  resetSwitch();
}

void panic() {
  //TODO
  systemReset();

  for (int idTrain = 0; idTrain < MY_TRAIN_LEN; idTrain++) {
    // shutDown all Hub
    Lpf2Hub *myTrain = myTrains[idTrain].hubobj;
    myTrain->shutDownHub();
    Serial.println("Disconnected from hub " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);
  }
  connectedTrain = 0;
  
  mySwitchController.shutDownHub();
  
}


// uso interno private
void systemStatus() {

  //TODO


	Serial.println("hubColor    batteryLevel hubState trainState speed");
	Serial.println("_________________________________________________");
    for (int idTrain = 0; idTrain < MY_TRAIN_LEN; idTrain++) {
		
		String space = "";
		for (int x=0; x<myTrains[idTrain].hubColor.length()-10;x++){
		  space += " ";			
    }
		Serial.println(myTrains[idTrain].hubColor + space + " " + myTrains[idTrain].batteryLevel + " " + myTrains[idTrain].hubState + " " +  myTrains[idTrain].trainState + " " + myTrains[idTrain].speed);
    }
	Serial.println("_________________________________________________");


	/*    

    String commandx = "{\"TA\":" +  String(myTrains[0].stato) + ",\"TB\":" +  String(myTrains[1].stato) + ",\"TC\":" +  String(myTrains[2].stato) + ",\"SA\":" +  String(speedA1) + ",\"SB\":" +  String(speedA2)  + ",\"SC\":" +  String(speedB)  + ",\"S1\":" +  String(mySwitch[0].stato) + ",\"S2\":" +  String(mySwitch[1].stato) + "}";
    if (verbose) Serial.println(commandx);
	

  */



}

void hubButtonCallback(void *hub, HubPropertyReference hubProperty, uint8_t *pData) {
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());
  if (idTrain == -1) return;

  if (hubProperty == HubPropertyReference::BATTERY_VOLTAGE)
  {
    myTrains[idTrain].batteryLevel = myHub->parseBatteryLevel(pData);    
    return;
  }

  if (hubProperty == HubPropertyReference::BUTTON)
  {
    ButtonState buttonState = myHub->parseHubButton(pData);
    if (buttonState == ButtonState::PRESSED)
    {
      //myHub->setBasicMotorSpeed(portA, 15);

      switch (myTrains[idTrain].hubState) {

        case 0: //ready -> active
          {

            Serial.println("Hub " + myTrains[idTrain].hubColor + " is ready");
            myTrains[idTrain].hubState = 1;
            connectedTrain++;

            
            byte portForDevice = myHub->getPortForDeviceType((byte)DeviceType::COLOR_DISTANCE_SENSOR);
            if (portForDevice != 255) {
              // activate hub button to receive updates              
              myHub->activatePortDevice(portB, colorDistanceSensorCallback);
              delay(200);
            }

            myHub->setLedColor(CYAN);

          }
          break;

        case 1: //active -> turnoff
          {
            myHub->setLedColor(RED);
            // disconnect color sensor
            //myHub->deactivatePortDevice(_portB, 37);
            delay(100);
            myHub->shutDownHub();
            Serial.println("Disconnected from hub " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);

            connectedTrain--;
            myTrains[idTrain].hubState = -1;

          }
          break;

      }

    }
  }
}

// callback function to handle updates of sensor values
void colorDistanceSensorCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData) {


  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());
  if (idTrain == -1) return;
  if (myTrains[idTrain].hubState != 1) return;

  if (deviceType == DeviceType::COLOR_DISTANCE_SENSOR) {
    int color = myHub->parseColor(pData);

    //if ((myTrains[idTrain].lastcolor == color || color==0 || color==9 || color == 255)) return;
    if (myTrains[idTrain].lastcolor == color || !checkIfSensorColorIsAccepted(color)) return;
    myTrains[idTrain].lastcolor = color;

    /*
      Serial.print("Color ");
      Serial.print("Hub " + myTrains[idTrain].hubColor + ":");
      Serial.println(COLOR_STRING[color]);
      Serial.print("Color dec: ");
      Serial.println(color,DEC);
    */

    myHub->setLedColor((Color)color);

    // set hub LED color to detected color of sensor and set motor speed dependent on color
    if (color == (byte)Color::RED) stopTrain(idTrain);
    else if (color == (byte)Color::WHITE) startTrain(idTrain);
    else if (color == (byte)Color::CYAN) stopAndTrain(idTrain, true); //GREEN
    else if (color == (byte)Color::YELLOW) stopAndTrain(idTrain, false);
    else if (color == (byte)Color::BLUE) invertTrain(idTrain);

  }
}

void stopAndTrain(int idTrain, bool invert) {
  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

  _print("Stop & ");
  (invert) ? _print("Invert") : _print("Go");
  _println(" " + myTrains[idTrain].hubColor);

  if (myTrains[idTrain].colorPreviousMillis == 0) {
    saveInterval(myTrains[idTrain].colorPreviousMillis);
    myTrain->stopBasicMotor(portA);
    if (invert) myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;
  }

}

void startTrain(int idTrain) {
  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

  _println("Start " + myTrains[idTrain].hubColor);
  _println("Train " + myTrains[idTrain].hubColor + " Battery Level: "  + myTrains[idTrain].batteryLevel);
  // TODO -> setta scambi per il ritorno e/o BAtttery Level


  if (myTrains[idTrain].speed < 0) myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;
  myTrains[idTrain].trainState = myTrains[idTrain].speed;
  myTrain->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
  _println("Train: " + myTrains[idTrain].hubColor + " Started!!!");


}

void stopTrain(int idTrain) {
  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;
  
  _println("Stop " + myTrains[idTrain].hubColor);
  myTrain->stopBasicMotor(portA);
  //myTrains[idTrain].speed = 0;
  myTrains[idTrain].trainState = 0;

}

void invertTrain(int idTrain) {
  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;
  
  _println("Invert " + myTrains[idTrain].hubColor);
  
  myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;
  myTrains[idTrain].trainState = myTrains[idTrain].speed;
  myTrain->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
}


void setup() {

  delay(3000);
  Serial.begin(115200);
  printLegenda();

}

void loop() {

  readFromSerial();
  while (Serial.available() == 0) {
	  
	//check for switch controller	
	if (! mySwitchController.isConnected()) scanSwitchController();        	  
  
	// check for train  
    for (int i = 0; i < MY_TRAIN_LEN; i++) {
      checkIntervalisExpired(i);
      if (! myTrains[i].hubobj->isConnected())scanHub(i);
    }
    if (isSystemReady) doMainCode();
  }

}

void doMainCode() {

  if (checkIfAllTrainIsStopped()) {

    int randIdTrain = random(1, MY_TRAIN_LEN + 1) - 1;
    if (connectedTrain > 1 && lastTrainStarted == randIdTrain) return;
    delay(beforeStartInterval);    
    lastTrainStarted = randIdTrain;
    startTrain(randIdTrain);
  }

}

void scanSwitchController(){
	
	if (!mySwitchController.isConnected() && !mySwitchController.isConnecting()) mySwitchController.init(switchControllerAddress); 
	
	// connect flow. Search for BLE services and try to connect if the uuid of the hub is found
	  if (mySwitchController.isConnecting()) {
		mySwitchController.connectHub();
		if (mySwitchController.isConnected()) {
		  Serial.println("Connected to HUB");
		  char hubName[] = "Switch";
		  mySwitchController.setHubName(hubName);
		
		} else {
		  Serial.println("Failed to connect to HUB");
		}
	  }
	  
}

void scanHub( int idTrain) {

  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

  if (!myTrain->isConnected() && !myTrain->isConnecting()) myTrain->init(myTrains[idTrain].hubAddress.c_str(), 1);

  if (myTrain->isConnecting()) {

    myTrain->connectHub();
    if (myTrain->isConnected()) {
      delay(500);
      Serial.println("Now connected with hub " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);

      // set the name
      char hubName[myTrains[idTrain].hubColor.length() + 1];
      myTrains[idTrain].hubColor.toCharArray(hubName, myTrains[idTrain].hubColor.length() + 1);
      myTrain->setHubName(hubName);

      // activate Property Update
      myTrain->activateHubPropertyUpdate(HubPropertyReference::BUTTON, hubButtonCallback);
      delay(200);
      myTrain->activateHubPropertyUpdate(HubPropertyReference::BATTERY_VOLTAGE, hubButtonCallback);
      delay(200);
      myTrain->setLedColor(PURPLE);
      myTrains[idTrain].hubState = 0;
      connectedTrain++;

    } else {
      Serial.println("Failed to connect with hub" +  myTrains[idTrain].hubColor);
    }

  }
}

int getHubIdByAddress(String address) {
  for (int i = 0; i < MY_TRAIN_LEN; i++) {
    if (myTrains[i].hubAddress == address) return i;
  }
  return -1;
}

void _print(String text) {
  if (isVerbose) Serial.print(text);
}

void _println(String text) {
  if (isVerbose) Serial.println(text);
}


void saveInterval(unsigned long &previousMillis) {  
  previousMillis = millis();
}


bool checkIfAllTrainIsStopped() {
  for (int i = 0; i < MY_TRAIN_LEN; i++) {
    if (myTrains[i].trainState != 0) return false;
  }
  return true;
}


bool checkIfSensorColorIsAccepted(byte inputColor) {
  for (int i = 0; i < MY_COLOR_LEN; i++) {
    if (sensorAcceptedColors[i] == inputColor) return true;
  }
  return false;
}



void checkIntervalisExpired(int idTrain ) {

  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

  if (millis() - myTrains[idTrain].colorPreviousMillis > colorInterval && myTrains[idTrain].colorPreviousMillis > 0) {    
    myTrains[idTrain].trainState = myTrains[idTrain].speed;
    myTrain->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    myTrains[idTrain].colorPreviousMillis = 0;
  }

}