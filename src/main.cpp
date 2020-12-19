#include <stdint.h>
#include <Arduino.h>
#include <FastLED.h>
#include <Bounce2.h>
#include "main.h"
#include "segment.h"
#include "nonvolatile.h"
#include "twinklefox.h"
#include "defaults.h"

int analog_pin = A0;
uint8_t program_num=0;
uint8_t exec_prog=0;
NonVolatile nv;
Segment segment;
Bounce button = Bounce(); // Instantiate a Bounce object
bool repeat = false;
uint8_t bright;
uint16_t analog;
//CRGB strip[NUM_LEDS];
CRGBArray <NUM_LEDS> strip;

void setup()
{
  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(strip, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);


  program_num=nv.getProgramNum();
  segment.begin();
  segment.display(program_num);


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
#ifdef SERIALDEBUG
    Serial.print("Analog\t");
    Serial.print(analog);
    Serial.println();
#endif
    int bright=map(analog, 0, ANALOG_RESOLUTION, 0, MAXBRIGHT);
    FastLED.setBrightness(bright);
    FastLED.show();
  }
}

void waitForInput (uint32_t ms)
{
  unsigned long now;
  unsigned long delaytime;

  now = millis();
  delaytime = now + ms;
  while ((repeat) && (now < delaytime))
  {
    now = millis();
    readInput();
  }
}

void loop()
{
  uint32_t hold_time=0;
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
    hold_time=0;
    exec_prog=program_num;
    repeat=true;
  }
  nv.setProgramNum(program_num);  // store current program in nvram (eeprom)
  segment.display(program_num);

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
      Serial.println("redGreen");
#endif
      redGreen(hold_time);
      break;
    case 2 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Red");
#endif
      colorWipe(CRGB::Red, WIPE_DELAY, hold_time);   // Red
      break;
    case 3 :
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Green");
#endif
      colorWipe(CRGB::Green, WIPE_DELAY, hold_time); // Green
      break
      ;
    case 4 :
#ifdef SERIALDEBUG
      Serial.println("redGreenRed");
#endif
      redGreenRed(hold_time);
      break;
    case 5 : 
#ifdef SERIALDEBUG
      Serial.println("Color Wipe Aqua");
#endif
      colorWipe(CRGB::Aqua, WIPE_DELAY, hold_time);
//      rainbow(20);
      break;
    case 6 :
#ifdef SERIALDEBUG
      Serial.println("RainbowCycle");
#endif
      rainbowCycle(hold_time);
      break;
    case 7 :
#ifdef SERIALDEBUG
      Serial.println("Theater Chase");
#endif
      theaterChase(CRGB::Red, CRGB::Green, WIPE_DELAY, hold_time);   // Red
      break;
    case 8 :
#ifdef SERIALDEBUG
      Serial.println("theaterChaseRainbow");
#endif
      theaterChaseRainbow(WIPE_DELAY, hold_time);
      break;
    case 9 :
#ifdef SERIALDEBUG
      Serial.println("Twinkle");
#endif
      twinkle(strip, &repeat, hold_time);
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

void redGreen(uint32_t wait)
{
  uint16_t i;
  for(i=0; i < NUM_LEDS; i+=2) {
    strip[i] = CRGB::Red;
    strip[i+1] = CRGB::Green;
    FastLED.show();
  }
  if (wait) {
    waitForInput(wait);
  } else {
    while(repeat) {
      readInput();
    }
  }
}
void redGreenRed(uint32_t wait)
{
  uint32_t colors[]={CRGB::Red, CRGB::Green};
  uint8_t color=0;
  uint16_t i;
  uint8_t pass=1;
  while(repeat) {
    for (i = 0; i < NUM_LEDS; i += 2)
    {
      strip[i] = colors[color];
      color = (color + 1) % 2;
      strip[i + 1] = colors[color];
      FastLED.show();
    }
    if (! wait) {
      waitForInput(2000);
    } else if ((wait) && (pass <= 4)) {
      pass++;
      waitForInput(wait/4);
    } else {
      break;
    }
  }
}

void colorWipe(uint32_t c, uint32_t wait, uint32_t hold)
{
  unsigned long now;
  unsigned long delaytime;
#ifdef SERIALDEBUG
  Serial.print("Top of colorWipe. c ");
  Serial.print(c);
  Serial.print(" wait: ");
  Serial.print(wait);
  Serial.print(" hold: ");
  Serial.print(hold);
  Serial.println();
#endif
  readInput();
  uint16_t i = 0;
  while ((repeat) && (i < NUM_LEDS))
  {
    strip[i] = c;
    i++;
    FastLED.show();
    waitForInput(wait);
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
    waitForInput(hold);
  } else {
    while (repeat) {
      readInput();
    }
  }
}

void rainbowCycle(uint32_t wait)
{
#ifdef SERIALDEBUG
  Serial.print("Top of rainbowCycle. wait: ");
  Serial.print(wait);
  Serial.println();
#endif
  uint16_t i, j;
  unsigned long now;
  unsigned long delaytime;
  if (wait) {
    now=millis();
    delaytime=now+wait;
#ifdef SERIALDEBUG
  Serial.print("now:\t");
  Serial.print(now);
  Serial.print("\t\tdelaytime\t");
  Serial.print(delaytime);
  Serial.println();
#endif
  }
  bool myRepeat=true;
  while (myRepeat) {
#ifdef SERIALDEBUG
    Serial.println("Top of while loop in rainbowCycle");
#endif
    for (j = 0; j < 256 * 5; j++)
    { // 5 cycles of all colors on wheel
      for (i = 0; i < NUM_LEDS; i++)
      {
        strip[i].setHue(((i * 256 / NUM_LEDS) + j) & 255);
      }
      FastLED.show();
      readInput();
      if (! repeat) {
        myRepeat=false;
        break;
      } else if (wait) {
        now=millis();
        if (now > delaytime) {
          myRepeat=false;
          break;
        }
      }
    }
  }
}

void theaterChase(uint32_t c1, uint32_t c2, uint32_t raw, uint32_t hold)
{
  uint16_t i, j, q;
  uint32_t wait = raw * 30;
  uint32_t colors[] = {c1, c2};
  uint8_t color=0;
  unsigned long now;
  unsigned long delaytime;
  if (hold)
  {
    now = millis();
    delaytime = now + hold;
#ifdef SERIALDEBUG
    Serial.print("now:\t");
    Serial.print(now);
    Serial.print("\t\tdelaytime\t");
    Serial.print(delaytime);
    Serial.println();
#endif
  }
  bool myRepeat = true;
  while (myRepeat)
  {
    for (q = 0; q < 3; q++)
    {
      for (i = 0; i + q < NUM_LEDS; i = i + 3)
      {
        strip[i + q]=colors[color];
        color=(color+1)%2;
      }
      FastLED.show();

      waitForInput(wait);
      if (!repeat)
      {
        myRepeat = false;
        break;
      }
      else if (hold)
      {
        now = millis();
        if (now > delaytime)
        {
          myRepeat = false;
          break;
        }
      }

      for (i = 0; i + q < NUM_LEDS; i = i + 3)
      {
        strip[i + q] = 0; //turn every third pixel off
      }
    }
  }
}

void theaterChaseRainbow(uint32_t raw, uint32_t hold)
{
#ifdef SERIALDEBUG
    Serial.print("theaterChaseRainbow raw:\t");
    Serial.print(raw);
    Serial.print("\t\thold\t");
    Serial.print(hold);
    Serial.println();
#endif
  uint16_t i, j, q;
  uint32_t wait=raw*10;
  unsigned long now;
  unsigned long delaytime;
  if (hold)
  {
    now = millis();
    delaytime = now + hold;
#ifdef SERIALDEBUG
    Serial.print("now:\t");
    Serial.print(now);
    Serial.print("\t\tdelaytime:\t");
    Serial.print(delaytime);
    Serial.print("\twait\t");
    Serial.print(wait);
    Serial.print("\t\thold:\t");
    Serial.print(hold);
    Serial.println();
#endif
  }
  bool myRepeat = true;
  while (myRepeat)
  {
    for (j = 0; j < 256; j += 4)
    { // cycle all 256 colors in the wheel
      for (q = 0; q < 3; q++)
      {
        for (i = 0; i + q < NUM_LEDS; i = i + 3)
        {
          strip[i + q].setHue((i + j) % 255); //turn every third pixel on
        }
        FastLED.show();

        waitForInput(wait);
        if (!repeat)
        {
          myRepeat = false;
          break;
        }
        else if (hold)
        {
          now = millis();
          if (now > delaytime)
          {
            myRepeat = false;
            break;
          }
        }

        for (i = 0; i + q < NUM_LEDS; i = i + 3)
        {
          strip[i + q] = 0; //turn every third pixel off
        }
      }
    }
  }
}