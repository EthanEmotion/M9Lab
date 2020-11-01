#include "Lpf2Hub.h"

// create a hub instance
Lpf2Hub myTrainHub_TA;
Lpf2Hub myTrainHub_TB;
Lpf2Hub myTrainHub_TC;
byte portA = (byte)PoweredUpHubPort::A;
byte portB = (byte)PoweredUpHubPort::B;


int connectedTrain=0;
bool isVerbose = true;
bool isInitialized = false;
bool isSystemReady = false;

// declare a custom structure for trains
typedef struct {    
  Lpf2Hub* hubobj;
  String hubColor;
  String hubAddress;
  int speed;
  int lastcolor;
  unsigned long colorPreviousMillis;
  byte hubState;
  byte trainState;
} Train;

/* config */
int trainSpeed = 25;
int colorInterval=5000;
// exclude black (trail) and RED (table) ..for now    
//GREEN instead CYAN -> to add (byte)Color::RED,
byte sensorAcceptedColors[] = {(byte)Color::WHITE, (byte)Color::CYAN, (byte)Color::BLUE, (byte)Color::YELLOW}; 


// Trains Maps
//code  - hubobj - hubColor  -  hubAddress - speed - lastcolor - hubState (2 = off, 0=ready, 1=active)  - trainState(0 = stopped in Shed, 1 = forward, -1 = backwards, 2= stopped in hidden place) 
Train myTrains[] = {
   { &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", trainSpeed, 0, 0, 2, 0}
  ,{ &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", trainSpeed, 0, 0 ,2, 0}  
  ,{ &myTrainHub_TC, "Green", "90:84:2b:16:9a:1f", trainSpeed, 0, 0 ,2, 0}     
};

/* end config */
#define MY_COLOR_LEN (sizeof(sensorAcceptedColors)/sizeof(int))
#define MY_TRAIN_LEN (sizeof(myTrains)/sizeof(int))



void hubButtonCallback(void *hub, HubPropertyReference hubProperty, uint8_t *pData){
  Lpf2Hub *myHub = (Lpf2Hub *)hub;   
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());    
  if (idTrain==-1) return;

  if (hubProperty == HubPropertyReference::BUTTON)
  {
    ButtonState buttonState = myHub->parseHubButton(pData);
    if (buttonState == ButtonState::PRESSED)
    {      	 
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
  
          case 1: //active -> shutdown
          {                         
              myHub->setLedColor(RED);                                          
              //myHub->deactivatePortDevice(_portB, 37);                                                         
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
	// 5 green ; 10 white; 9 red; 3 blue; 7 yellow
	*/
  
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  int idTrain = getHubIdByAddress(myHub->getHubAddress().toString().c_str());  
  if (idTrain==-1) return;  
  if (myTrains[idTrain].hubState!=1) return;  
  
  if (deviceType == DeviceType::COLOR_DISTANCE_SENSOR){
    int color = myHub->parseColor(pData);
          
    //if ((myTrains[idTrain].lastcolor == color || color==0 || color==9 || color == 255)) return;
    if (myTrains[idTrain].lastcolor == color || !checkIfSensorColorIsAccepted(color)) return;
    
    myTrains[idTrain].lastcolor = color;    
	/*
    _print("Color ");
    _print("Hub " + myTrains[idTrain].hubColor + ":"); 
    _println(COLOR_STRING[color]);
    _print("Color dec: ");
    _println(color,DEC);    
	*/

    // set hub LED color to detected color of sensor and set motor speed dependent on color
    if (color == (byte)Color::BLUE){
      _print("Stop");
      myHub->setLedColor((Color)color);
      myHub->stopBasicMotor(portA);
    }
    
    else if (color == (byte)Color::WHITE){
      _print("Go");
      myHub->setLedColor((Color)color);
      myHub->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    }

    else if (color == (byte)Color::YELLOW) {              
              _print("Stop & Invert");
              myHub->setLedColor((Color)color);
            
              if (myTrains[idTrain].colorPreviousMillis == 0){
                saveInterval(myTrains[idTrain].colorPreviousMillis);                      
                myHub->stopBasicMotor(portA);
                // invert
                myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;                
              }
                                                                                                                              
    }
	
    else if (color == (byte)Color::RED) {              
              _print("Stop & Go");
              myHub->setLedColor((Color)color);
            
              if (myTrains[idTrain].colorPreviousMillis == 0){
                saveInterval(myTrains[idTrain].colorPreviousMillis);                      
                myHub->stopBasicMotor(portA);             
              }
                                                                                                                              
    }	
    
    else if (color == (byte)Color::CYAN){ //GREEN
      _print("Invert");
		  myHub->setLedColor((Color)color);
	    myTrains[idTrain].speed = -1 * myTrains[idTrain].speed;
		  myHub->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    }
	
  }
}


void setup() {
  
    delay(3000);
    Serial.begin(115200);
    _println("Ready!\nHold Hub button to change status.");  

}

void loop() {

    //if (!isInitialized) checkRemote();
    for (int i = 0; i < MY_TRAIN_LEN; i++){      
      checkIntervalisExpired(i);
      if (! myTrains[i].hubobj->isConnected())scanHub(i);                
    }
    //if (isSystemReady) doMainCode();  

}


void doMainCode(){
  
   _println("Maincode");
   /*
   1) controllo se ci sono treni fa far tornare e li faccio tornare   
   2) controllo se ci sono treni fermi e li faccio uscire
   */
   
   /*
   var trainId = checkIfTrainToComeBack();
   if (trainId >- 1){
	   PoweredUpHub *myTrain = myTrains[trainId].hubobj;
	   myTrain->setMotorSpeed(_portA, -1 * velocity);   // go                               
   }else{
	   if (checkIfAllTrainIsStopped()){
		    int randNumber = random(1, MY_TRAIN_LEN) - 1;
			PoweredUpHub *myTrain = myTrains[randNumber].hubobj;			
			myTrain->setMotorSpeed(_portA,  velocity);   
	   }		   
   }
   */
  
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



void scanHub( int idTrain) {

    Lpf2Hub *myTrain = myTrains[idTrain].hubobj;

    if (!myTrain->isConnected() && !myTrain->isConnecting()) myTrain->init(myTrains[idTrain].hubAddress.c_str(),1);                   

      if (myTrain->isConnecting()) {
          
          myTrain->connectHub();          
          if (myTrain->isConnected()) {   

            // set the name
            char hubName[myTrains[idTrain].hubColor.length()];
            myTrains[idTrain].hubColor.toCharArray(hubName, myTrains[idTrain].hubColor.length());
            myTrain->setHubName(hubName);
            
				    delay(500);		  
				    _println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   				  				
  				  myTrain->activateHubPropertyUpdate(HubPropertyReference::BUTTON, hubButtonCallback);  				  
  				  myTrain->setLedColor(PURPLE);
  				  myTrains[idTrain].hubState=0;    
  				  connectedTrain++;
           _print("Hub name: ");
           _println(myTrain->getHubName().c_str());
  				  //isInitialized = true;  				
				                
           }else{
                _println("Failed to Connect to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   
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
  _println("Timing Set");
  previousMillis = millis();
}  
  
void checkIntervalisExpired(int idTrain ){

  Lpf2Hub *myTrain = myTrains[idTrain].hubobj;    
  if (millis() - myTrains[idTrain].colorPreviousMillis > colorInterval && myTrains[idTrain].colorPreviousMillis>0){    
    _println("Interval Expired");      
    myTrain->setBasicMotorSpeed(portA, myTrains[idTrain].speed);
    myTrains[idTrain].colorPreviousMillis = 0;
    
  }    
}  
