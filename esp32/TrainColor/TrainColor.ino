/**
 * A Train hub basic example to connect a PoweredUp hub, and set the speed of the train 
 * motor dependent on the detected color. If the train is stopped, you can start the train by
 * pressing the hub button.
 * 
 * (c) Copyright 2019 - Cornelius Munz
 * Released under MIT License
 * 
 */

#include "PoweredUpHub.h"

// create a hub instance
PoweredUpHub myHub1;
PoweredUpHub myHub2;
PoweredUpHub::Port _portA1 = PoweredUpHub::Port::A;
PoweredUpHub::Port _portB1 = PoweredUpHub::Port::B;
PoweredUpHub::Port _portA2 = PoweredUpHub::Port::A;
PoweredUpHub::Port _portB2 = PoweredUpHub::Port::B;


int velocity1 = 25;
int velocity2 = 25;

unsigned long ignoreColor1_previousMillis = 0;
int ignoreColor1_interval = 700; //Wait for ?
unsigned long ignoreColor2_previousMillis = 0;
int ignoreColor2_interval = 700; //Wait for ?


void setup() {
    Serial.begin(115200);    
} 


// main loop
void loop() {

  if (!myHub1.isConnected() && !myHub1.isConnecting()) myHub1.init("90:84:2b:04:a8:c5",1);                 
  if (!myHub2.isConnected() && !myHub2.isConnecting()) myHub2.init("90:84:2b:1c:be:cf",1);               

  if (checkIntervalisExpired(ignoreColor1_previousMillis, ignoreColor1_interval))ignoreColor1_previousMillis = 0;                                                                      
  if (checkIntervalisExpired(ignoreColor2_previousMillis, ignoreColor2_interval))ignoreColor2_previousMillis = 0;                                                                      


  // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
  if (myHub1.isConnecting()) {
    myHub1.connectHub();
    if (myHub1.isConnected()) {
      Serial.println("Connected to HUB1");
      // connect color/distance sensor to port c, activate sensor for updates
      myHub1.deactivatePortDevice(_portB1);
      delay(5000);
      myHub1.activatePortDevice(_portB1, 37);
      myHub1.setLedColor(GREEN);
      Serial.println("HUB1 is ready");
    } else {
      Serial.println("Failed to connect to HUB1");
    }
  }

  if (myHub2.isConnecting()) {
    myHub2.connectHub();
    if (myHub2.isConnected()) {
      Serial.println("Connected to HUB2");
      // connect color/distance sensor to port c, activate sensor for updates
      myHub2.deactivatePortDevice(_portB2);
      delay(5000);
      myHub2.activatePortDevice(_portB2, 37);
      myHub2.setLedColor(GREEN);
      Serial.println("HUB2 is ready");
    } else {
      Serial.println("Failed to connect to HUB2");
    }
  }  

  // if connected, you can start your control-flow
  if (myHub1.isConnected() && myHub2.isConnected() && ignoreColor1_previousMillis==0 ) {
        
   
    // read color value of sensor
    int color = myHub1.getColor();
    Serial.print("Color hub1: ");
    Serial.println(color, DEC);
    
    // set hub LED color to detected color of sensor
    // 5 verde ; 10 bianco; 9 rosso; 3 blu; 7 giallo
    if (color == 3) { 
        myHub1.setLedColor(BLUE);
        myHub1.stopMotor(_portA1);     
    } else if (color == 5){
        myHub1.setLedColor(GREEN);
        
        if (ignoreColor1_previousMillis==0){
          Serial.println("Wating for color1"); 
          saveInterval(ignoreColor1_previousMillis);                      
          velocity1 = -1 * velocity1;
          myHub1.setMotorSpeed(_portA1, velocity1);                  
        }
                

    }else if (color == 10){ 
        myHub1.setLedColor(WHITE);

        if (ignoreColor1_previousMillis==0){   
          Serial.println("Wating for color1");        
          saveInterval(ignoreColor1_previousMillis);                      
          myHub1.setMotorSpeed(_portA1, velocity1);                  
        }        
        
    }      
  }

  if (myHub2.isConnected() && myHub1.isConnected() && ignoreColor2_previousMillis==0) {
        
   
    // read color value of sensor
    int color = myHub2.getColor();
    Serial.print("Color hub2: ");
    Serial.println(color, DEC);
    
    // set hub LED color to detected color of sensor
    // 5 verde ; 10 bianco; 9 rosso; 3 blu; 7 giallo
    if (color == 3) { 
        myHub2.setLedColor(BLUE);
        myHub2.stopMotor(_portA2);     
    } else if (color == 5){
        myHub2.setLedColor(GREEN);
        
        if (ignoreColor2_previousMillis==0){
          Serial.println("Wating for color2"); 
          saveInterval(ignoreColor2_previousMillis);                      
          velocity2 = -1 * velocity2;
          myHub2.setMotorSpeed(_portA2, velocity2);                  
        }               

    }else if (color == 10){ 
        myHub2.setLedColor(WHITE);

        if (ignoreColor2_previousMillis==0){   
          Serial.println("Wating for color1");        
          saveInterval(ignoreColor2_previousMillis);                      
          myHub2.setMotorSpeed(_portA2, velocity2);                  
        }        
        
    }      
  }  
  
} // End of loop

void saveInterval(unsigned long &previousMillis){  
  previousMillis = millis();
}  
  
bool checkIntervalisExpired(unsigned long &previousMillis , int interval){   
  return millis() - previousMillis > interval;
}
