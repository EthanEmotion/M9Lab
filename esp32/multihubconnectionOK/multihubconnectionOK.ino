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


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("Hold Hub button for at least half second to change status");        
} 


// main loop
void loop() {

  checkRemote();
    
  for (int i = 0; i < MY_TRAIN_LEN; i++){      
      if (! myTrains[i].hubobj->isConnected()){
        scanHub(myTrains[i].hubobj,i);          
      }else{
        handleHub(myTrains[i].hubobj,i);        
      }      
  }

  // main code here
  if (isSystemReady) {
    doMainCode();
  }else{
    Serial.println("System not ready yet!");
  }
    
} // End of loop


void doMainCode(){

  // faccio partire il primo treno disponibile
   Serial.println("Maincode");
  
}

void checkRemote(){
  
  if (myRemote.isConnecting()){
    
      Serial.println("hub type: " + myRemote.getHubType());
      //if (myRemote.getHubType() == POWERED_UP_REMOTE){
        //This is the right device 
        if (!myRemote.connectHub())
        {
          Serial.println("Unable to connect to hub");
        }
        else
        {
          myRemote.setLedColor(YELLOW);
          Serial.println("Remote connected.");
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

      Serial.println("System is initialized");
      isInitialized = true;
      // both activations are needed to get status updates
      myRemote.activateButtonReports(); 
      myRemote.activatePortDevice(_portLeft, 55);
      //myRemote.activatePortDevice(_portRight, 55);
      myRemote.setLedColor(BLUE);    
  }  

  
  if (isInitialized) {
    if (myRemote.isLeftRemoteStopButtonPressed()) {
        if (isSystemReady){
            isSystemReady = false;
            myRemote.setLedColor(RED);  

            Serial.println("System is stopped");
            
        }else{
            isSystemReady = true;
            myRemote.setLedColor(GREEN);     
            Serial.println("System is running");
        }        
    }      
  }  
  
}


void scanHub(PoweredUpHub *myTrain, int idTrain) {
    //Serial.print("Scan for Hub ");
    //Serial.println(myTrains[idTrain].id);

    if (!myTrain->isConnected() && !myTrain->isConnecting()) {     
      myTrain->init(myTrains[idTrain].hubAddress.c_str(),1);         
      //myTrain->init();                
    }  

      if (myTrain->isConnecting()) {
          myTrain->connectHub();
          if (myTrain->isConnected()) {
                      
                Serial.println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   
				myTrain->activateButtonReports();
                myTrain->activatePortDevice(0x3A, 40); // Tilt-Sensor            
                myTrain->activatePortDevice(_portB, 37);    // port for sensor
                

                delay(300);
                
                Serial.print("BatteryLevel [%]: ");
                Serial.println(myTrain->getBatteryLevel(), DEC);   
      
                myTrains[idTrain].hubState=0; 
                    
                myTrain->setLedColor(BLUE);
                connectedTrain++;
          }
      
      }            
}

void myTrainHubButtonCallback(bool isPressed) {
  if (isPressed) {
      Serial.println("myTrainHub1 Button pressed");
  } else {
      Serial.println("myTrainHub1 Button released");
  }
}

void handleHub(PoweredUpHub *myTrain, int idTrain) {

  //Serial.print("Handle Hub ");
  //Serial.println(idTrain);
      
    if (myTrain->isConnected()) {

            //myTrain->registerButtonCallback(&myTrainHubButtonCallback);
            //myTrain->activateButtonReports();

           
            if(myTrain->isButtonPressed()){

              //delay(500);
                                        
              switch (myTrains[idTrain].hubState){
        
                  case 0: //ready -> active
                  {
                    myTrain->setLedColor(GREEN);
                    Serial.println("Hub " + myTrains[idTrain].code + " started"); 
                    myTrains[idTrain].hubState=1;                
                    //myTrain->setMotorSpeed(_portA, 15);
                    
                  }
                  break;
          
                  case 1: //active -> turnoff
                  {
                         
                      myTrain->setLedColor(RED);
                      delay(500); 
                      //myTrain->setMotorSpeed(_portA, 0);                
                      myTrain->shutDownHub();       
                      connectedTrain--;                     
                      myTrains[idTrain].hubState=-1;                      
                    
                  }
                  break;
                
                }
                
           }

            if (myTrains[idTrain].hubState==1){
  
              // read color value of sensor
              delay(100);
              int color = myTrain->getColor();
              delay(100);
              Serial.print("Detected color of hub " +  myTrains[idTrain].hubColor + ": ");
              Serial.println(color, DEC);
              
              // set hub LED color to detected color of sensor
              // 5 verde ; 10 bianco
              if (color == 9) { 
                  //myTrain->setLedColor(RED);
                  myTrain->stopMotor(_portA);     // stop
              } else if (color == 7){
                  //myTrain->setLedColor(YELLOW);
                  myTrain->setMotorSpeed(_portA, 25);   // go
              }else if (color == 3){ 
                  //myTrain->setLedColor(BLUE);
                  //myTrain->setMotorSpeedForTime(_portA, 0, 5000);   // stop for 5 seconds             
              } 
                
            }
            
           
                 
        }  //end is connected
  
}
