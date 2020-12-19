#ifndef MAIN_H
#define MAIN_H
void rainbowCycle(uint32_t wait);
void colorWipe(uint32_t c, uint32_t wait, uint32_t hold);
void nothing(void);
void readInput(void);
void waitForInput (uint32_t ms);
void redGreen(uint32_t wait);
void redGreenRed(uint32_t wait);
void theaterChase(uint32_t c1, uint32_t c2, uint32_t wait, uint32_t hold);
void theaterChaseRainbow(uint32_t wait, uint32_t hold);
#endif