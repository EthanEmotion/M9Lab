/**
 * A Legoino example to control a train which has a motor connected
 * to the Port A of the Hub
 * 
 * pBLEScan->start(1); (Lpf2Hub.cpp row 680 in  Lpf2Hub::init)
 * 
 * (c) Copyright 2020 - SteFX
 * Released under MIT License
 * 
 */

#include "PoweredUpHub.h"

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
  PoweredUpHub hubobj;
  String hubColor;
  String hubAddress;
  byte hubState;
  byte trainState;
} Train;


//N - code  - hubobj - hubColor  -  hubAddress - hubState (-1 = off, 0=ready, 1=active)  - trainState(0 = stopped in Shed, 1 = forward, -1 = backwards, 2= stopped in hidden place) 
Train myTrains[MY_TRAIN_LEN] = {
  { 1, "TA", myTrainHub_TA, "Yellow" , "90:84:2b:04:a8:c5", -1, 0},
  { 2, "TB", myTrainHub_TB, "Red", "90:84:2b:1c:be:cf", -1, 0}  
};

int connectedTrain=0;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("Hold Hub button for at least half second to change status");        
} 


// main loop
void loop() {
    
  for (int i = 0; i < MY_TRAIN_LEN; i++){
      
      if (! myTrains[i].hubobj.isConnected()){
        scanHub(&myTrains[i].hubobj,i);          
      }else{
        handleHub(&myTrains[i].hubobj,i);        
      }      
  }
    
} // End of loop


void scanHub(PoweredUpHub *myTrain, int idTrain) {
    //Serial.print("Scan for Hub ");
    //Serial.println(myTrains[idTrain].id);

    if (!myTrain->isConnected() && !myTrain->isConnecting()) {     
      myTrain->init(myTrains[idTrain].hubAddress.c_str());         
      //myTrain->init();                
    }  

      if (myTrain->isConnecting()) {
          myTrain->connectHub();
          if (myTrain->isConnected()) {
                      
                Serial.println("Connected to " + myTrains[idTrain].hubColor + " -> "  + myTrains[idTrain].hubAddress);   
                
                myTrain->activatePortDevice(_portB, 37);    // port for sensor
                myTrain->activatePortDevice(0x3A, 40); // Tilt-Sensor            
                
                Serial.print("BatteryLevel [%]: ");
                Serial.println(myTrain->getBatteryLevel(), DEC);   

      
                myTrains[idTrain].hubState=0; 

                    
                myTrain->setLedColor(BLUE);
                connectedTrain++;
          }
      
      }            
}

void handleHub(PoweredUpHub *myTrain, int idTrain) {

  //Serial.print("Handle Hub ");
  //Serial.println(idTrain);
      
    if (myTrain->isConnected()) {
                               
            if(myTrain->isButtonPressed()){

              delay(1000);
                                        
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


            // read color value of sensor
            int color = myTrain->getColor();
            Serial.print("Color: ");
            Serial.println(color, DEC);
            
            // set hub LED color to detected color of sensor
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
           
                 
        }  //end is connected
  
}
