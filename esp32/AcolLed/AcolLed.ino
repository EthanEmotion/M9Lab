// AcolLed 1.0.1 -> Controlla i led atom matrix via Mario/Remote per illuminare una bacheca Lego
// utilizza lego poweredup con la libreria Legoino by Cornelius Munz  (https://github.com/corneliusmunz/legoino) ver 1.1.0
// AcolLed - 2021 Code by Stefx

/* note per 
  installare cp210x su linux
  apt list linux-modules-extra-5.8.0-38-generic
  dpkg -L linux-modules-extra-5.8.0-38-generic | grep cp210x
  sudo modprobe cp210x
*/

/* led part */
#include <FastLED.h>
#define NUM_LEDS 25
#define DATA_PIN 27


CRGB leds[NUM_LEDS];


#include "Lpf2Hub.h"
DeviceType barcodeSensor = DeviceType::MARIO_HUB_BARCODE_SENSOR;


bool isMarioSensorInitialized = false;
bool isRemoteInitialized = false;
bool isRemoteInitFirst = false;
bool isMarioInitFirst = false;

uint32_t lastColor;

// create a hub instance
Lpf2Hub myRemote;
Lpf2Hub myMario;

byte portLeft = (byte)PoweredUpRemoteHubPort::LEFT;
byte portRight = (byte)PoweredUpRemoteHubPort::RIGHT;


void DeviceCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  Lpf2Hub *myMario = (Lpf2Hub *)hub;

  if (deviceType == DeviceType::MARIO_HUB_BARCODE_SENSOR)
  {
    MarioColor color = myMario->parseMarioColor(pData);
    Serial.print("Mario Color: ");
    Serial.println((byte)color);
    marioColorToLed((byte)color);    
  }

  Lpf2Hub *myRemote = (Lpf2Hub *)hub;

   if (deviceType == DeviceType::REMOTE_CONTROL_BUTTON){
      ButtonState buttonState = myRemote->parseRemoteButton(pData);     
      remoteColorToLed((byte)buttonState,(byte)portNumber);
   }

}

void remoteColorToLed( byte buttonState, byte portNumber){
   
  if (buttonState==1 && portNumber == 0)  fullColor(CRGB::Blue);
  if (buttonState==255 && portNumber == 0)  fullColor(CRGB::Green);
  if (buttonState==1 && portNumber == 1)  fullColor(CRGB::Red);
  if (buttonState==255 && portNumber == 1)  fullColor(CRGB::Yellow);
  if (buttonState==127 && portNumber == 0)  fullColor(CRGB::White);
  if (buttonState==127 && portNumber == 1) {    
    myRemote.shutDownHub();
    isRemoteInitFirst = false;
  }
    
}

void marioColorToLed( byte color){
     
 switch (color) {

        case 23: 
          fullColor(CRGB::Blue);         
        break;

        case 37:           
          fullColor(CRGB::Green);                   
        break;

        case 21: 
          fullColor(CRGB::Red);                   
        break;

        case 24:           
          fullColor(CRGB::Yellow);                   
        break;           
  }        
  
}

void setup()
{
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);    
  fullColor(CRGB::White);
  Serial.begin(115200);  

  // force shutdown
  /*
  if (myMario.isConnected())  myMario.shutDownHub();
  if (myRemote.isConnected())  myRemote.shutDownHub();
  */
  
  //myMario.init(); 
  //delay(200);
  //myRemote.init(); 
}

// main loop
void loop()
{

  /* mario */
  
  // connect flow. Search for BLE services and try to connect
  if (myMario.isConnecting())
  {
    myMario.connectHub();
    if (myMario.isConnected())
    {
      Serial.println("Connected to HUB");
      fullColor(CRGB::MediumPurple); 
    }
    else
    {
      Serial.println("Failed to connect to HUB");
      fullColor(CRGB::White); 
    }
  }

  if (! myMario.isConnected()){
    //if (!isRemoteInitialized)  fullColor(CRGB::White); 
    isMarioSensorInitialized=false;
  }

  if (myMario.isConnected() && !isMarioSensorInitialized)
  {
    delay(200);    
    byte portForDevice = myMario.getPortForDeviceType((byte)barcodeSensor);
    Serial.println(portForDevice, DEC);
    if (portForDevice != 255)
    {    
      myMario.activatePortDevice(portForDevice, DeviceCallback);
      delay(200);
      isMarioSensorInitialized = true;
    };
  }  


  if (! myMario.isConnected() && ! isMarioInitFirst){
    //if (!isMarioSensorInitialized) fullColor(CRGB::White); 
    myMario.init();
    isMarioSensorInitialized = false; 
    isMarioInitFirst = true;
  }

  

  /* remote */
  if (myRemote.isConnecting())
  {
    if (myRemote.getHubType() == HubType::POWERED_UP_REMOTE)
    {
      //This is the right device
      if (!myRemote.connectHub())
      {
        Serial.println("Unable to connect to hub");
      }
      else
      {
        myRemote.setLedColor(GREEN);
        Serial.println("Remote connected.");
      }
    }
  }

  if (myRemote.isConnected() && !isRemoteInitialized)
  {
    Serial.println("System is initialized");
    isRemoteInitialized = true;
    delay(200); //needed because otherwise the message is to fast after the connection procedure and the message will get lost
    // both activations are needed to get status updates
    myRemote.activatePortDevice(portLeft, DeviceCallback);
    myRemote.activatePortDevice(portRight, DeviceCallback);    
    myRemote.setLedColor(GREEN);    
  }

  if (! myRemote.isConnected() && ! isRemoteInitFirst){
    //if (!isMarioSensorInitialized) fullColor(CRGB::White); 
    myRemote.init();
    isRemoteInitialized = false; 
    isRemoteInitFirst = true;
  }

} // End of loop

void fullColor(uint32_t color){

  if (lastColor==color) return;
  
  FastLED.setBrightness(20);  
  fill_solid(leds, NUM_LEDS, color);  
  for (int i=0; i<20; i++){
    FastLED.setBrightness(i);   
    delay(75);
    FastLED.show();
  }   
  lastColor=color;

}
