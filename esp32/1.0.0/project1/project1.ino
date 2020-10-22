
#include "Lpf2Hub.h"

// create a hub instance
Lpf2Hub myTrainHub_TA;
Lpf2Hub myTrainHub_TB;
byte portA = (byte)PoweredUpHubPort::A;
byte portB = (byte)PoweredUpHubPort::B;


int velocity = 25;
int connectedTrain=0;
bool isVerbose = true;

bool isInitialized = false;
bool isSystemReady = false;


// dichiaro struttura custom per treni
typedef struct {  
  String code;
  Lpf2Hub* hubobj;
  String hubColor;
  String hubAddress;
  int speed;
  int lastcolor;
  byte hubState;
  byte trainState;
} Train;


// Trains Maps
#define MY_TRAIN_LEN 2

//code  - hubobj - hubColor  -  hubAddress - speed - lastcolor - hubState (2 = off, 0=ready, 1=active)  - trainState(0 = stopped in Shed, 1 = forward, -1 = backwards, 2= stopped in hidden place) 
Train myTrains[MY_TRAIN_LEN] = {
   { "TA", &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", velocity, 0, 2, 0}
  ,{ "TB", &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", velocity, 0, 2, 0}  
};



void hubButtonCallback(void *hub, HubPropertyReference hubProperty, uint8_t *pData){
  Lpf2Hub *myHub = (Lpf2Hub *)hub;   
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());    
  if (idTrain==-1) return;

  if (hubProperty == HubPropertyReference::BUTTON)
  {
    ButtonState buttonState = myHub->parseHubButton(pData);
    if (buttonState == ButtonState::PRESSED)
    {
      //myHub->setBasicMotorSpeed(portA, 15);
	  
      switch (myTrains[idTrain].hubState){

          case 0: //ready -> active
          {
                        
            _println("Hub " + myTrains[idTrain].code + " started"); 
            myTrains[idTrain].hubState=1;  

            _print("check ports... if needed sensor is already connected: ");
            byte portForDevice = myHub->getPortForDeviceType((byte)DeviceType::COLOR_DISTANCE_SENSOR);
            Serial.println(portForDevice, DEC); 
          
            if (portForDevice != 255){
              // activate hub button to receive updates                                  
      			  Serial.println("activatePortDevice");
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
	
	/*
	// set hub LED color to detected color of sensor
	// 5 verde ; 10 bianco; 9 rosso; 3 blu; 7 giallo
	*/
	
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());
  _println("idTrain" + idTrain);
  if (idTrain==-1) return;  
  if (myTrains[idTrain].hubState!=1) return;
  
  if (deviceType == DeviceType::COLOR_DISTANCE_SENSOR){
    int color = myHub->parseColor(pData);

     // exclude black (trail) and RED (table)  
    if (myTrains[idTrain].lastcolor == color || color==0 || color==9 || color == 255) return;
    myTrains[idTrain].lastcolor = color;    
    Serial.print("Color: ");
    Serial.println(COLOR_STRING[color]);
    Serial.print("Color: ");
    Serial.println(color,DEC);    

    // set hub LED color to detected color of sensor and set motor speed dependent on color
    if (color == (byte)Color::BLUE)
    {
      myHub->setLedColor((Color)color);
      myHub->stopBasicMotor(portA);
    }
    else if (color == (byte)Color::WHITE)
    {
      myHub->setLedColor((Color)color);
      myHub->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    }
    else if (color == (byte)Color::CYAN)
    {
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
