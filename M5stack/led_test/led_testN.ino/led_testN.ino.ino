#include <FastLED.h>
#define NUM_LEDS 25
#define DATA_PIN 27


byte charStart = 0x30;
byte chargen[][5] = {
{0x0E, 0x13, 0x15, 0x19, 0x0E},  // 0
{0x00, 0x10, 0x1F, 0x12, 0x00},  // 1
{0x12, 0x15, 0x15, 0x15, 0x19},  // 2
{0x0A, 0x15, 0x15, 0x11, 0x11},  // 3
{0x1F, 0x04, 0x04, 0x04, 0x03},  // 4
{0x09, 0x15, 0x15, 0x15, 0x17},  // 5
{0x09, 0x15, 0x15, 0x15, 0x0E},  // 6
{0x03, 0x1D, 0x01, 0x01, 0x01},  // 7
{0x0A, 0x15, 0x15, 0x15, 0x0A},  // 8
{0x0E, 0x15, 0x15, 0x05, 0x02},  // 9
};

// ref: http://fastled.io/docs/3.1/struct_c_r_g_b.html
uint32_t colour[] = {CRGB::Black, CRGB::Red, CRGB::Green, CRGB::Yellow, CRGB::Blue, CRGB::Cyan, CRGB::Purple };

//rotate / flip
// https://macetech.github.io/FastLED-XY-Map-Generator/
const uint8_t XYTable[] = {
     4,   9,  14,  19,  24,
     3,   8,  13,  18,  23,
     2,   7,  12,  17,  22,
     1,   6,  11,  16,  21,
     0,   5,  10,  15,  20
  };

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);   
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);  
  FastLED.setBrightness(20);    
}

void loop() {
  
  for(int numC=1; numC<7; numC++){

    doCountdown(numC);

    fullColor(colour[numC]);
    delay(3000);
  }
  
}

void doCountdown(int colorIndex){
 for(int num=9; num>-1; num--){
      
      char b[1];
      String str;
      str=String(num);
      str.toCharArray(b,1);
      osCopyChar(str[0],colorIndex);  
      delay(1000);  
   }  
}

void fullColor(uint32_t color){

 for(int num=0; num<NUM_LEDS; num++) {    
    leds[num] = color;
    FastLED.show();     
  }

}

void osCopyChar (char myChar, int colorIndex)
{
  myChar -= charStart;

  for (int i=0; i<5; i++)
    for (int j=0; j<5; j++)
    {
      uint32_t typex = (bitRead(chargen[myChar][i], j) == 0x00) ? colour[0] : colour[colorIndex];
      int pos = (i*5)+j;
      leds[XYTable[pos]] = typex;
      FastLED.show(); 
    }
}
