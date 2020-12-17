#include <stdint.h>
#include <Arduino.h>
#include <FastLED.h>
#include "main.h"
#include "segment.h"
#include "nonvolatile.h"
#include <Bounce2.h>

#define BUTTON_PIN 2
#define BOUNCE_INTERVAL 25
#define ANALOG_RESOLUTION 1023 // 0-1023 = 10 bits on 328p
#define DATA_PIN 10 // pixel strip data pin
#define NUM_LEDS 8
#define RANDOM 10
#define CYCLE 11
#define TOTAL_PROGS 12
#define LAST_EXEC 9
#define RANDOM_LETTER "A"
#define CYCLE_LETTER "C"
#define CYCLE_HOLD 10000  // miliseconds
#define MAXBRIGHT 128
//#define NUM_LEDS 8
//#define SERIALDEBUG

int analog_pin = A0;
uint8_t program_num=0;
uint8_t exec_prog=0;
NonVolatile nv;
Segment segment;
Bounce button = Bounce(); // Instantiate a Bounce object
bool repeat = false;
uint8_t bright;
uint16_t analog;

CRGB strip[NUM_LEDS];

void setup()
{
  //FastLED.addLeds<NEOPIXEL, DATA_PIN, RGB>(strip, NUM_LEDS); // setup the strip
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(strip, NUM_LEDS); // setup the strip
  //fill_solid( &(strip[0]), NUM_LEDS /number of leds/, CRGB::Black ); // set them all to off
  //FastLED.show(); // then display the strip

	segment.begin();

  //program_num=nv.getProgramNum();
  program_num=2;

  button.attach (BUTTON_PIN, INPUT_PULLUP);
  button.interval(BOUNCE_INTERVAL);
  #ifdef SERIALDEBUG
  Serial.begin(9600);
  Serial.println("Initialized Serial");
  Serial.print("MCUSR: ");
  Serial.println(MCUSR);
   switch (MCUSR) {
   case 2: // b00000010
           // Reset button or otherwise some software reset
           Serial.println("Reset button was pressed.");
           break;
   case 7: // b00000111
           // Boot up after power loss
           Serial.println("Power loss occured!");
           break;
   }
#endif
}

void readInput() 
{
  button.update();
  if (button.fell()) {
#ifdef SERIALDEBUG
  Serial.println ("Button Press Detected");
#endif
    repeat=false;
    program_num++;
  }
  uint16_t this_analog=analogRead(analog_pin);
  if (this_analog != analog) {
    analog=this_analog;
    int bright=map(analog, 0, ANALOG_RESOLUTION, 0, MAXBRIGHT);
    FastLED.setBrightness(bright);
   FastLED.show();
  }
}

void loop()
{
  int hold_time=0;
#ifdef SERIALDEBUG
  Serial.print ("Top of loop\t\t");
  Serial.print (program_num);
  Serial.println ();
#endif
  program_num=program_num%TOTAL_PROGS;
  #ifdef SERIALDEBUG
  Serial.print ("Normalized program_num\t");
  Serial.print (program_num);
  Serial.println ();
#endif
  if (program_num == RANDOM) {
#ifdef SERIALDEBUG
  Serial.println ("Cycle");
#endif
    exec_prog=(exec_prog%LAST_EXEC)+1;
    hold_time=CYCLE_HOLD;
    repeat=true;
  } else if (program_num == CYCLE) {
#ifdef SERIALDEBUG
  Serial.println ("Random");
#endif
    exec_prog=rand() % LAST_EXEC + 1;
    hold_time=CYCLE_HOLD;
    repeat=true;
  } else {
    exec_prog=program_num;
    repeat=true;
  }
  nv.setProgramNum(program_num);  // store current program in nvram (eeprom)
#ifdef SERIALDEBUG
  Serial.print("exec_prog:\t\t");
  Serial.print(exec_prog);
  Serial.println ();
#endif
  switch (exec_prog) {
    case 0 :
      nothing();
      break;
    case 1 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Red");
#endif
      colorWipe(CRGB::Red, 50, hold_time);   // Red
      break;
    case 2 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Green");
#endif
      colorWipe(CRGB::Green, 50, hold_time); // Green
      break
      ;
    case 3 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Blue");
#endif
      colorWipe(CRGB::Blue, 50, hold_time);  // Blue
      break;
    case 4 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Gray");
#endif
      colorWipe(CRGB::Gray, 50, hold_time);
//      theaterChase(CRGB::White, 50); // White
      break;
    case 5 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Yellow");
#endif
      colorWipe(CRGB::Yellow, 50, hold_time);
//      theaterChase(CRGB::Red, 50);   // Red
      break;
    case 6 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Orange");
#endif
      colorWipe(CRGB::Orange, 50, hold_time);
//      theaterChase(CRGB::Blue, 50);  // Blue
      break;
    case 7 : 
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Aqua");
#endif
      colorWipe(CRGB::Aqua, 50, hold_time);
//      rainbow(20);
      break;
    case 8 : 
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Purple");
#endif
      colorWipe(CRGB::Purple, 50, hold_time);
//      rainbowCycle(20);
      break;
    case 9 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Pink");
#endif
      colorWipe(CRGB::Pink, 50, hold_time);
//      theaterChaseRainbow(50);
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
  //Wait for button press
  while (repeat) {
    readInput();
  }
}

void colorWipe(uint32_t c, uint8_t wait, uint32_t hold)
{
#ifdef SERIALDEBUG
  Serial.print("Top of colorWipe. c ");
  Serial.print(c);
  Serial.print(" wait: ");
  Serial.print(wait);
  Serial.print(" hold: ");
  Serial.print(hold);
  Serial.println();
#endif
  unsigned long now;
  unsigned long delaytime;
  readInput();
  uint16_t i = 0;
  while ((repeat) && (i < NUM_LEDS))
  {
    strip[i] = c;
    i++;
    FastLED.show();
    now = millis();
    delaytime = now + wait;
    while ((repeat) && (now < delaytime))
    {
      now = millis();
      readInput();
    }
  }
  if (hold > 0)
  {
    now = millis();
    delaytime = now + hold;
#ifdef SERIALDEBUG
    Serial.print("Holding for ");
    Serial.print(hold);
    Serial.print(" milliseconds. repeat: ");
    Serial.print(repeat);
    Serial.println();
#endif
    while ((repeat) && (now < delaytime))
    {
      now = millis();
      readInput();
    }
  } else {
    while (repeat) {
      readInput();
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