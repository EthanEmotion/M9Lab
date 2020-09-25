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


// Mappatura treni
// numero trenif
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

int connectedTrain=0;
bool isInitialized = false;
bool isSystemReady = false;
bool isVerbose = false;


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

  // faccio partire il primo treno disponibile
   _println("Maincode");
  
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
                    myTrain->setLedColor(GREEN);
                    
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
              //2x rossi, blu, giallo e verde
              
              // 5 verde ; 10 bianco
              if (color == 9) { 
                  _print("Stop");
                  myTrain->setLedColor(RED);
                  myTrain->stopMotor(_portA);     // stop                  
              } else if (color == 7){
                  _print("Invert");
                  myTrain->setLedColor(YELLOW);
                  myTrain->stopMotor(_portA);
                  delay(100);
                  myTrain->setMotorSpeed(_portA, -50);   // go                  
              }else if (color == 3){ 
                  // to implement
                  _print("stop & go");
                  myTrain->setLedColor(BLUE);
                  //myTrain->setMotorSpeedForTime(_portA, 0, 5000);   // stop for 5 seconds             
              } else{
                  myTrain->setLedColor(GREEN);
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
