
#include "Lpf2Hub.h"

// create a hub instance
Lpf2Hub myTrainHub_TA;
Lpf2Hub myTrainHub_TB;
Lpf2Hub myTrainHub_TC;
byte portA = (byte)PoweredUpHubPort::A;
byte portB = (byte)PoweredUpHubPort::B;


int trainSpeed = 25;
int connectedTrain=0;
bool isVerbose = true;
int colorInterval=5000;
bool isInitialized = false;
bool isSystemReady = false;


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
#define MY_COLOR_LEN 5

// exclude black (trail) and RED (table) ..for now    
//GREEN instead CYAN -> to add 
byte sensorAcceptedColors[MY_COLOR_LEN] = {(byte)Color::WHITE, (byte)Color::CYAN, (byte)Color::BLUE, (byte)Color::RED, (byte)Color::YELLOW}; 

//code  - hubobj - hubColor  -  hubAddress - speed - lastcolor - hubState (2 = off, 0=ready, 1=active)  - trainState(0 = stopped in Shed, 1 = forward, -1 = backwards, 2= stopped in hidden place)  - batteryLevel
Train myTrains[MY_TRAIN_LEN] = {
   { &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", trainSpeed, 0, 0, 2, 0, 100}
  ,{ &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", trainSpeed, 0, 0 ,2, 0, 100}  
  ,{ &myTrainHub_TC, "Green", "90:84:2b:16:9a:1f", trainSpeed, 0, 0 ,2, 0, 100}     
};




void hubButtonCallback(void *hub, HubPropertyReference hubProperty, uint8_t *pData){
  Lpf2Hub *myHub = (Lpf2Hub *)hub;   
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());    
  if (idTrain==-1) return;

  if (hubProperty == HubPropertyReference::BATTERY_VOLTAGE)
  {    
    myTrains[idTrain].batteryLevel = myHub->parseBatteryLevel(pData);
    Serial.print("BatteryLevel: ");    
    Serial.println(myTrains[idTrain].batteryLevel, DEC);   
    return;
  }

  if (hubProperty == HubPropertyReference::BUTTON)
  {
    ButtonState buttonState = myHub->parseHubButton(pData);
    if (buttonState == ButtonState::PRESSED)
    {
      //myHub->setBasicMotorSpeed(portA, 15);
	  
      switch (myTrains[idTrain].hubState){

          case 0: //ready -> active
          {
                        
            _println("Hub " + myTrains[idTrain].hubColor + " started"); 
            myTrains[idTrain].hubState=1;  

            _print("check ports... if needed sensor is already connected: ");
            byte portForDevice = myHub->getPortForDeviceType((byte)DeviceType::COLOR_DISTANCE_SENSOR);            
            if (portForDevice != 255){
              // activate hub button to receive updates                                  
      			  _println("activatePortDevice");
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
              myTrains[idTrain].hubState=2;                      
            
          }
          break;
        
        }	  	  	  	  
	  
    }
  }
}

// callback function to handle updates of sensor values
void colorDistanceSensorCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData){
	
	
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());  
  if (idTrain==-1) return;  
  if (myTrains[idTrain].hubState!=1) return;  
  
  if (deviceType == DeviceType::COLOR_DISTANCE_SENSOR){
    int color = myHub->parseColor(pData);

    //if ((myTrains[idTrain].lastcolor == color || color==0 || color==9 || color == 255)) return;
    if (myTrains[idTrain].lastcolor == color || !checkIfSensorColorIsAccepted(color)) return;
    myTrains[idTrain].lastcolor = color;    
    Serial.print("Color ");
    Serial.print("Hub " + myTrains[idTrain].hubColor + ":"); 
    Serial.println(COLOR_STRING[color]);
    Serial.print("Color dec: ");
    Serial.println(color,DEC);    

    // aggiorno livello batteria (quando) ?
    //setBatteryLevel(idTrain);

    // set hub LED color to detected color of sensor and set motor speed dependent on color
    if (color == (byte)Color::RED){
      _println("Stop");
      myHub->setLedColor((Color)color);
      myHub->stopBasicMotor(portA);
      setBatteryLevel(idTrain);
    }
    
    else if (color == (byte)Color::WHITE){
      _println("Go");
      myHub->setLedColor((Color)color);
      myHub->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    }

    else if (color == (byte)Color::YELLOW) {              
              _println("Stop & Invert");
              myHub->setLedColor((Color)color);
            
              if (myTrains[idTrain].colorPreviousMillis == 0){
                saveInterval(myTrains[idTrain].colorPreviousMillis);                      
                myHub->stopBasicMotor(portA);
                // invert
                myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;                
              }
    }  
    else if (color == (byte)Color::BLUE) {              
              _println("Stop & Go");
              if (myTrains[idTrain].colorPreviousMillis == 0){
                saveInterval(myTrains[idTrain].colorPreviousMillis);                      
                myHub->stopBasicMotor(portA);             
              }
                                                                                                                              
    } 
    
    else if (color == (byte)Color::CYAN){ //GREEN
      _println("Invert");
      myHub->setLedColor((Color)color);
      myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;
      myHub->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    }
  }
}


void setup() {
  
    delay(3000);
    Serial.begin(115200);
    _println("Hold Hub button to change status");  

}

void loop() {

    //if (!isInitialized) checkRemote();
    for (int i = 0; i < MY_TRAIN_LEN; i++){      
      checkIntervalisExpired(i);
      if (! myTrains[i].hubobj->isConnected())scanHub(i);                
    }
    //if (isSystemReady) doMainCode();  

}

void scanHub( int idTrain) {

    Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

    if (!myTrain->isConnected() && !myTrain->isConnecting()) myTrain->init(myTrains[idTrain].hubAddress.c_str(),1);                   

      if (myTrain->isConnecting()) {
          
          myTrain->connectHub();          
          if (myTrain->isConnected()) {   
				  delay(500);		  
          _println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   

            // set the name
            char hubName[myTrains[idTrain].hubColor.length()+1];
            myTrains[idTrain].hubColor.toCharArray(hubName, myTrains[idTrain].hubColor.length()+1);
            myTrain->setHubName(hubName);
            _print("Setting name->" )
            _println(hubName);
				  				
  				  myTrain->activateHubPropertyUpdate(HubPropertyReference::BUTTON, hubButtonCallback);
  				  delay(200);
  				  myTrain->setLedColor(PURPLE);
  				  myTrains[idTrain].hubState=0;    
  				  connectedTrain++;
  				  //isInitialized = true;  				
				                
           }else{
                _println("Failed to connect to HUB");
            }
      
      }            
}

int getHubIdByAddress(String address){		
	for (int i = 0; i < MY_TRAIN_LEN; i++){
	  if(myTrains[i].hubAddress == address) return i;
	}
	return -1;	
}

void _print(String text){
  if(isVerbose) Serial.print(text);
}

void _println(String text){
  if(isVerbose) Serial.println(text);
}


void saveInterval(unsigned long &previousMillis){
  _println("Setto timing");
  previousMillis = millis();
} 


int checkIfTrainToComeBack(){    
  for (int i = 0; i < MY_TRAIN_LEN; i++){  
    if (myTrains[i].trainState==2) return i;
  } 
  return -1;
}

bool checkIfAllTrainIsStopped(){    
  for (int i = 0; i < MY_TRAIN_LEN; i++){  
    if (myTrains[i].trainState>0) return false;
  } 
  return true;
}


bool checkIfSensorColorIsAccepted(byte inputColor){      
  for (int i = 0; i < MY_COLOR_LEN; i++){  
    if (sensorAcceptedColors[i] == inputColor) return true;
  } 
  return false;
}


void setBatteryLevel(int idTrain){
  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;
  myTrain->activateHubPropertyUpdate(HubPropertyReference::BATTERY_VOLTAGE, hubButtonCallback);
}

  
void checkIntervalisExpired(int idTrain ){

  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;
    
  if (millis() - myTrains[idTrain].colorPreviousMillis > colorInterval && myTrains[idTrain].colorPreviousMillis>0){
    
    _println("Expired");      
    myTrain->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    myTrains[idTrain].colorPreviousMillis = 0;
    
  }else{
    //_println("NotExipired");
  }
    
}  
