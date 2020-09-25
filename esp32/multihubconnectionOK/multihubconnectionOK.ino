/**
 * A Legoino example to control a train which has a motor connected
 * to the Port A of the Hub
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


//N - code  - hubobj - hubColor  -  hubAddress - hubState (-1 = off, 0=ready, 1=active)  - trainState(0 = stopped in Shed, 1 = forward, -1 = backwards, 2= stopped in hidden place) 
Train myTrains[MY_TRAIN_LEN] = {
  { 1, "TA", &myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", -1, 0},
  { 2, "TB", &myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", -1, 0}  
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

  checkRemote();
    
  for (int i = 0; i < MY_TRAIN_LEN; i++){      
      if (! myTrains[i].hubobj->isConnected()){        
        scanHub(i);          
      }else{        
        handleHub(i);
      }      
  }

  // main code here
  if (isSystemReady) {
    doMainCode();
  }else{
    _println("System not ready yet!");
  }
    
} // End of loop


void doMainCode(){
  
   _println("Maincode");
   /*
   1) controllo se ci sono treni fa far tornare e li faccio tornare
   2) controllo se ci sono treni fermi e li faccio uscire
   */
  
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
            myRemote.setLedColor(CYAN);     
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
          if (myTrain->isConnected()) {
                      
                _println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   
                myTrains[idTrain].hubState=0;                     
                myTrain->setLedColor(PURPLE);
                connectedTrain++;
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
      
    if (myTrain->isConnected()) {

            //myTrain->registerButtonCallback(&myTrainHubButtonCallback);
            //myTrain->activateButtonReports();

           
            if(myTrain->isButtonPressed()){
                                                      
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
                      myTrain->shutDownHub();       
                      connectedTrain--;                     
                      myTrains[idTrain].hubState=-1;                      
                    
                  }
                  break;
                
                }
                
           }

            if (myTrains[idTrain].hubState==1){
  
              // read color value of sensor              
              int color = myTrain->getColor();              
              _print("Detected color of hub " +  myTrains[idTrain].hubColor + ": ");
              _println(String(color));
              
              // set hub LED color to detected color of sensor
              //rossi-> stop, blu-> stop and go, giallo-> stop and invert e verde->Invert
              
              // 5 verde ; 10 bianco
              if (color == 9) { 
                    _print("Stop");
                    myTrain->setLedColor(RED);
                    myTrain->stopMotor(_portA);     // stop                  
              } else if (color == 6){
                    _print("Invert");
                    myTrain->setLedColor(GREEN);
                    myTrain->stopMotor(_portA);
                    delay(100);
                    myTrain->setMotorSpeed(_portA, -1 * velocity);   // go                  
              }else if (color == 3){ 
                    // TODO Check
                    _print("Stop & Go");
                    if (checkTheTime(stopandgo_previousMillis, stopandgo_interval)){
                       myTrain->setMotorSpeed(_portA, velocity);
                    }else{
                       saveTheTime(stopandgo_previousMillis);
                       myTrain->stopMotor(_portA);  
                    }
                    
                    myTrain->setLedColor(BLUE);
                    myTrain->stopMotor(_portA);                    
                    
                  //myTrain->setMotorSpeedForTime(_portA, 0, 5000);   // stop for 5 seconds             
              }else if (color == 7){ 
                    // TODO Check
                    _print("Stop & Invert");
                    if (checkTheTime(stopandinvert_previousMillis, stopandinvert_interval)){
                       myTrain->setMotorSpeed(_portA, -1 * velocity);
                    }else{
                       saveTheTime(stopandinvert_previousMillis);
                       myTrain->stopMotor(_portA);  
                    }
                    
                    myTrain->setLedColor(BLUE);
                    myTrain->stopMotor(_portA);                    
                    
                  //myTrain->setMotorSpeedForTime(_portA, 0, 5000);   // stop for 5 seconds             
              }              
              
              else{
                  myTrain->setLedColor(CYAN);
              }
                
            }
                 
        }  //end is connected
  
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

void saveTheTime(unsigned long &previousMillis){
  previousMillis = millis();
}  
  
bool checkTheTime(unsigned long previousMillis , int interval){
  return millis() - previousMillis > interval;
}  
