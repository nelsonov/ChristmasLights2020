#include <stdint.h>
#include <Arduino.h>
#include <FastLED.h>
#include "main.h"
#include "segment.h"
#include "nonvolatile.h"

#define DATA_PIN 10 // pixel strip data pin
//#define NUM_LEDS 150
#define NUM_LEDS 8
//#define SERIALDEBUG

uint8_t program_num=0;
uint8_t exec_prog=0;
bool repeat=true;
NonVolatile nv;
Segment segment;

CRGB strip[NUM_LEDS];

void setup()
{
  //FastLED.addLeds<NEOPIXEL, DATA_PIN, RGB>(strip, NUM_LEDS); // setup the strip
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(strip, NUM_LEDS); // setup the strip
  //fill_solid( &(strip[0]), NUM_LEDS /number of leds/, CRGB::Black ); // set them all to off
  //FastLED.show(); // then display the strip

	segment.begin();

  program_num=nv.getProgramNum();
#ifdef SERIALDEBUG
  Serial.begin(9600);
  Serial.println("Initialized Serial");
#endif
}

void loop()
{
#ifdef SERIALDEBUG
  Serial.print ("Top of loop\t");
  Serial.print (program_num);
  Serial.println ();
#endif
  program_num=program_num%12;
  if (program_num == 10) {
    exec_prog=(exec_prog%9)+1;
    repeat=false;
  } else if (program_num == 11) {
    exec_prog=rand() % 9 + 1;
    repeat=false;
  } else {
    exec_prog=program_num;
    repeat=true;
  }

#ifdef SERIALDEBUG
  Serial.print("exec_prog:\t");
  Serial.print(exec_prog);
  Serial.println ();
#endif
  switch (exec_prog) {
    case 0 :
      nothing();
      break;
    case 1 :
      colorWipe(CRGB::Red, 50);   // Red
      break;
    case 2 :
      colorWipe(CRGB::Green, 50); // Green
      break;
    case 3 :
      colorWipe(CRGB::Blue, 50);  // Blue
      break;
    case 4 :
      theaterChase(CRGB::White, 50); // White
      break;
    case 5 :
      theaterChase(CRGB::Red, 50);   // Red
      break;
    case 6 :
      theaterChase(CRGB::Blue, 50);  // Blue
      break;
    case 7 : 
      rainbow(20);
      break;
    case 8 : 
      rainbowCycle(20);
      break;
    case 9 :
      theaterChaseRainbow(50);
      break;
  }
}

void nothing()
{
  //Turn off all lights
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    strip[i] = CRGB::Black;
    FastLED.show();
  }
  //Wait for intterupt
  while (repeat) {}
}


void colorWipe(uint32_t c, uint8_t wait)
{
  while (repeat) {
    uint16_t i=0;
    while ((repeat) && (i < NUM_LEDS)) {
      i++;
      strip[i] = c;
      FastLED.show();
      unsigned long now=millis();
      unsigned long delaytime=now + wait;
      while ((repeat) && (now < delaytime)) {
        now=millis();
     }
   }
  }
}

void rainbow(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256; j++)
  {
    for (i = 0; i < NUM_LEDS; i++)
    {
      strip[i].setHue((i + j) & 255);
    }
    FastLED.show();
    delay(wait);
  }
}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_LEDS; i++)
    {
      strip[i].setHue(((i * 256 / NUM_LEDS) + j) & 255);
    }
    FastLED.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait)
{
  uint16_t i, j, q;
  for (j = 0; j < 10; j++)
  { //do 10 cycles of chasing
    for (q = 0; q < 3; q++)
    {
      for (i = 0; i + q < NUM_LEDS; i = i + 3)
      {
        strip[i + q] = c; //turn every third pixel on
      }
      FastLED.show();

      delay(wait);

      for (i = 0; i + q < NUM_LEDS; i = i + 3)
      {
        strip[i + q] = 0; //turn every third pixel off
      }
    }
  }
}
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
  uint16_t i, j, q;
  for (j = 0; j < 256; j++)
  { // cycle all 256 colors in the wheel
    for (q = 0; q < 3; q++)
    {
      for (i = 0; i + q < NUM_LEDS; i = i + 3)
      {
        strip[i + q].setHue((i + j) % 255); //turn every third pixel on
      }
      FastLED.show();

      delay(wait);

      for (i = 0; i + q < NUM_LEDS; i = i + 3)
      {
        strip[i + q] = 0; //turn every third pixel off
      }
    }
  }
}