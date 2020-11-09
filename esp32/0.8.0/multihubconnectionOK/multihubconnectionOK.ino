/**
 * Step order:
 * 
 * 1) turn on all train hubs and set on ready (CYAN)
 * 2) turn on remote controller and set system on ready (CYAN)
 * 
 * (c) Copyright 2020 - SteFX
 * Released under MIT License
 * 
 */

#include "PoweredUpRemote.h"
#include "PoweredUpHub.h"

PoweredUpRemote myRemote;
PoweredUpRemote::Port _portLeft = PoweredUpRemote::Port::LEFT;
//PoweredUpRemote::Port _portRight = PoweredUpRemote::Port::RIGHT;

// create a hub instance
PoweredUpHub myTrainHub_TA;
PoweredUpHub myTrainHub_TB;
PoweredUpHub::Port _portA = PoweredUpHub::Port::A;
PoweredUpHub::Port _portB = PoweredUpHub::Port::B;


// Trains Maps
#define MY_TRAIN_LEN 2

// dichiaro struttura custom per treni
typedef struct {
  byte id;
  String code;
  PoweredUpHub* hubobj;
  String hubColor;
  String hubAddress;
  byte hubState;
  byte trainState;
} Train;


//N - code  - hubobj - hubColor  -  hubAddress - hubState (2 = off, 0=ready, 1=active)  - trainState(0 = stopped in Shed, 1 = forward, -1 = backwards, 2= stopped in hidden place) 
Train myTrains[MY_TRAIN_LEN] = {
  { 1, "TA", &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", 2, 0}
  ,{ 2, "TB", &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", 2, 0}  
};


unsigned long stopandgo_previousMillis = 0;
int stopandgo_interval = 5000; //Wait for ?

unsigned long stopandinvert_previousMillis = 0;
int stopandinvert_interval = 5000; //Wait for ?

unsigned long maincode_previousMillis = 0;
int maincode_interval = 10000; //Wait for ?

int velocity = 50;
int connectedTrain=0;
bool isInitialized = false;
bool isSystemReady = false;
bool isVerbose = true;

// prototype


void setup() {
    delay(3000);
    Serial.begin(115200);
    _println("Hold Hub button for at least half second to change status");        
} 


// main loop
void loop() {

  //if (!isInitialized) checkRemote();
    
  for (int i = 0; i < MY_TRAIN_LEN; i++){      
      if (! myTrains[i].hubobj->isConnected()){        
        scanHub(i);          
      }else{        
        handleHub(i);
        // if (isSystemReady)
		handleColor(i);
      }      
  }

  // main code here
  if (isSystemReady) doMainCode();  
    
} // End of loop


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

void checkRemote(){
  
  if (myRemote.isConnecting()){
    
      _println("hub type: " + myRemote.getHubType());
      //if (myRemote.getHubType() == POWERED_UP_REMOTE){
        //This is the right device 
        if (!myRemote.connectHub())
        {
          _println("Unable to connect to hub");
        }
        else
        {
          myRemote.setLedColor(YELLOW);
          _println("Remote connected.");
        }
        
      //}
    }

  if (!myRemote.isConnected()){
    myRemote.init("04:ee:03:b9:d8:19",1); // addr?     

    isInitialized = false;
    
    // se scollegando remote vuoi interrompere il programma decommenta prossima riga   
    //isSystemReady = false;
  }    


  if (myRemote.isConnected() && ! isInitialized){ //&& MY_TRAIN_LEN == connectedTrain o tutti alemeno 1 , nessuno?

      _println("System is initialized");
      isInitialized = true;
      // both activations are needed to get status updates
      myRemote.activateButtonReports(); 
      myRemote.activatePortDevice(_portLeft, 55);
      //myRemote.activatePortDevice(_portRight, 55);
      myRemote.setLedColor(PURPLE);    
  }  

  
  if (isInitialized) {
    if (myRemote.isLeftRemoteStopButtonPressed()) {
        if (isSystemReady){
            isSystemReady = false;
            myRemote.setLedColor(RED);  

            _println("System is stopped");
            
        }else{
            isSystemReady = true;
            myRemote.setLedColor(GREEN);     
            _println("System is running");
        }        
    }      
  }  
  
}


void scanHub( int idTrain) {

    PoweredUpHub *myTrain = myTrains[idTrain].hubobj;

    if (!myTrain->isConnected() && !myTrain->isConnecting()) {           
      myTrain->init(myTrains[idTrain].hubAddress.c_str(),1);               
    }  

      if (myTrain->isConnecting()) {
          
          myTrain->connectHub();
          delay (500);
          if (myTrain->isConnected()) {                      
                _println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   
                myTrains[idTrain].hubState=0;                     
                myTrain->setLedColor(PURPLE);
                connectedTrain++;
          }
      
      }            
}

void handleColor(int idTrain) {

    PoweredUpHub *myTrain = myTrains[idTrain].hubobj;
          
    if (myTrains[idTrain].hubState==1){

          // read color value of sensor              
          int color = myTrain->getColor();    
          if (color>0){          
            _print("Detected color of hub " +  myTrains[idTrain].hubColor + ": ");
            _println(String(color));
          }  
                    
          // set hub LED color to detected color of sensor
          //rossi-> stop, blu-> stop and go, giallo-> stop and invert e verde->Invert
          
        // 5 verde ; 10 bianco; 9 rosso; 3 blu; 7 giallo
        if (color == 9) { 
              _print("Stop");
              myTrain->setLedColor(RED);
              myTrain->stopMotor(_portA);     // stop  
			  myTrains[idTrain].trainState=0;
        } else if (color == 10){
              _print("Go");
              myTrain->setLedColor(WHITE);                    
              myTrain->setMotorSpeed(_portA, velocity);   // go                               
			  myTrains[idTrain].trainState=1;
        } else if (color == 5){
              _print("Invert");
              myTrain->setLedColor(GREEN);
              myTrain->stopMotor(_portA);                    
              delay(100);
              myTrain->setMotorSpeed(_portA, -1 * velocity);   // -1 * velocity   
			  myTrains[idTrain].trainState=-1;			  
        }else if (color == 3){                     
              _print("Stop & Go");
              myTrain->setLedColor(BLUE);                    

              if (stopandgo_previousMillis==0){
                saveInterval(stopandgo_previousMillis);                      
                myTrain->stopMotor(_portA);  
              }
              
              if (checkIntervalisExpired(stopandgo_previousMillis, stopandgo_interval)){
                 myTrain->setMotorSpeed(_portA, velocity);
				 myTrains[idTrain].trainState=1;			  
                 stopandgo_previousMillis = 0;
              }                                                               
              
            //myTrain->setMotorSpeedForTime(_portA, 0, 5000);   // stop for 5 seconds             
        }else if (color == 7){ 
              // TODO Check
              _print("Stop & Invert");
              myTrain->setLedColor(YELLOW);

              if (stopandgo_previousMillis==0){
                saveInterval(stopandinvert_previousMillis);                      
                myTrain->stopMotor(_portA);  
              }
              
              if (checkIntervalisExpired(stopandinvert_previousMillis, stopandinvert_interval)){
                 myTrain->setMotorSpeed(_portA, -1 * velocity);
				 myTrains[idTrain].trainState=-1;			  
                 stopandinvert_previousMillis = 0;
              }                                                                                                   
        }              
        
        else{
            myTrain->setLedColor(CYAN);
        }
          
      }                

}    

/*
void myTrainHubButtonCallback(bool isPressed) {
  if (isPressed) {
      _println("myTrainHub1 Button pressed");
  } else {
      _println("myTrainHub1 Button released");
  }
}
*/

void handleHub(int idTrain) {

    PoweredUpHub *myTrain = myTrains[idTrain].hubobj;         
    //myTrain->registerButtonCallback(&myTrainHubButtonCallback);
    //myTrain->activateButtonReports();
   
    if(myTrain->isButtonPressed()){
      delay(500);                                                   
      switch (myTrains[idTrain].hubState){

          case 0: //ready -> active
          {
            myTrain->setLedColor(CYAN);
            
            _println("Hub " + myTrains[idTrain].code + " started"); 
            myTrains[idTrain].hubState=1;  

            _print("BatteryLevel [%]: ");
            _println( String(getTrainBattery(myTrain)));                      

            // connect color sensor   
            myTrain->activatePortDevice(_portB, 37);    // port for sensor                                                                 
            
          }
          break;
  
          case 1: //active -> turnoff
          {                         
              myTrain->setLedColor(RED);                            
              // disconnect color sensor  
              myTrain->deactivatePortDevice(_portB, 37);                                           
              delay(100);
              myTrain->shutDownHub();   
                  
              connectedTrain--;                     
              myTrains[idTrain].hubState=2;                      
            
          }
          break;
        
        }
        
   }
                     
}

uint8_t getTrainBattery(PoweredUpHub *myTrain){      
    return myTrain->getBatteryLevel();
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
  
bool checkIntervalisExpired(unsigned long &previousMillis , int interval){
    
  if (millis() - previousMillis > interval && previousMillis>0){
    _println("true");  
  }else{
    _println("false");
  }
  
  return millis() - previousMillis > interval;
}  
