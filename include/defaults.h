#include <FastLED.h>
#include "segment.h"
#include "nonvolatile.h"
#include <Bounce2.h>

#ifndef DEFAULTS_H
#define LED_TYPE   WS2812
#define COLOR_ORDER   GRB
#define VOLTS          5
#define MAX_MA       4000
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

#endif