// Questo sketch gestisce 3 treni (A,B,C) su un tracciato dal deposito alla galeria
// utilizza lego poweredup con la libreria Legoino by Cornelius Munz  (https://github.com/corneliusmunz/legoino)
// necessita di 3 hub city per i treni (motore + sensore colore) e un hub technic (3 motori) per gli scambi.
// TrenIno4 - 2020 Code by Stefx

// test track all ok

// TODO:
// scambi per far entrare treni o mandarli


#include "Lpf2Hub.h"

String ver = "1.3.0";

// create a hub instance
Lpf2Hub myTrainHub_TA;
Lpf2Hub myTrainHub_TB;
Lpf2Hub myTrainHub_TC;
byte portA = (byte)PoweredUpHubPort::A;
byte portB = (byte)PoweredUpHubPort::B;

bool isSystemReady = false;
int trainSpeed = 25;
int connectedTrain = 0;
bool isVerbose = true;
int colorInterval = 5000;
int beforeStartInterval = 5000;
int lastTrainStarted = -1;

// dichiaro struttura custom per treni
typedef struct {
  Lpf2Hub* hubobj;
  String hubColor;
  String hubAddress;
  int speed;
  int lastcolor;
  unsigned long colorPreviousMillis;
  byte hubState;
  byte trainState;
  int batteryLevel;
} Train;


// Trains Maps
#define MY_TRAIN_LEN 3
#define MY_COLOR_LEN 3

// exclude black (trail) and RED (table) ..for now
//GREEN instead CYAN -> to add
//  (byte)Color::BLUE,(byte)Color::YELLOW
byte sensorAcceptedColors[MY_COLOR_LEN] = {(byte)Color::WHITE, (byte)Color::CYAN,  (byte)Color::RED};

//code  - hubobj - hubColor  -  hubAddress - speed - lastcolor - hubState (-1 = off, 0=ready, 1=active) - batteryLevel
Train myTrains[MY_TRAIN_LEN] = {
  { &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", trainSpeed, 0, 0, -1, 0, 100}
  , { &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", trainSpeed, 0, 0 , -1, 0, 100}
  , { &myTrainHub_TC, "Green", "90:84:2b:16:9a:1f", trainSpeed, 0, 0 , -1, 0, 100}
};



void printLegenda() {

  // print command
  _println("*** M9Lab - TrenIno4 v." + ver + " ***");
  _println("_________________________________________________");
  _println("List of commands used by system:");

  _println("on = set system on");
  _println("off = set system off");
  _println("panic = shutDown all hubs and reset the system");
  _println("reset = reset the system");
  _println("status = show system status");
  _println("help = show this message again");

  _println("_________________________________________________");
}

void readFromSerial() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    if (command == "panic") panic();
    if (command == "reset") systemReset();
    if (command == "on") systemOn();
    if (command == "off") systemOff();
    if (command == "help") printLegenda();
    if (command == "status") systemStatus();
  }
}

void systemOn() {
  isSystemReady = true;
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
}

void panic() {
  //TODO
  systemReset();

  for (int idTrain = 0; idTrain < MY_TRAIN_LEN; idTrain++) {
    // shutDown all Hub
    Lpf2Hub *myTrain = myTrains[idTrain].hubobj;
    myTrain->shutDownHub();
    _println("Disconnected: " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);
  }
  connectedTrain = 0;
}


// uso interno private
void systemStatus() {

  //TODO

  String command = "";

  /*
    for (int i = 0; i < MY_TRAIN_LEN; i++) {
    command += myTrains[i].codice	+ ":" + String(myTrains[i].stato) + ",";
    }

    for (int i = 0; i < MY_TRACK_LEN; i++) {
    command += myTrack[i].codice  + ":" +  String(myTrack[i].stato) + ",";
    }


    for (int i = 0; i < MY_SWITCH_LEN; i++) {
    command += mySwitch[i].codice  + ":" + String(mySwitch[i].stato) + ",";
    }

    OR

    String commandx = "{\"TA\":" +  String(myTrains[0].stato) + ",\"TB\":" +  String(myTrains[1].stato) + ",\"TC\":" +  String(myTrains[2].stato) + ",\"SA\":" +  String(speedA1) + ",\"SB\":" +  String(speedA2)  + ",\"SC\":" +  String(speedB)  + ",\"S1\":" +  String(mySwitch[0].stato) + ",\"S2\":" +  String(mySwitch[1].stato) + "}";
    if (verbose) Serial.println(commandx);
  */

  command =  command.substring(0, command.length() - 1);
  Serial.println('{' + command + '}');


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

            _println("Hub " + myTrains[idTrain].hubColor + " started");
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
            _println("Disconnected: " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);

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

void scanHub( int idTrain) {

  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

  if (!myTrain->isConnected() && !myTrain->isConnecting()) myTrain->init(myTrains[idTrain].hubAddress.c_str(), 1);

  if (myTrain->isConnecting()) {

    myTrain->connectHub();
    if (myTrain->isConnected()) {
      delay(500);
      _println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);

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
      _println("Failed to connect to HUB");
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
