#ifndef SEGMENT_H
#define SEGMENT_H
#include <SevSeg.h>

#define SEG_A 8 //LED pin 7
#define SEG_B 9 // LED pin 6
#define SEG_C 5 // LED pin 4
#define SEG_D 4 // LED pin 2
#define SEG_E 3 // LED pin 1
#define SEG_F 7 // LED pin 9
#define SEG_G 6 // LED pin 10
#define SEGMENT_BRIGHTNESS 90
#define DISPLAY_TIME 60000 // 60 seconds

class Segment {
    private:
        SevSeg sevseg;
        unsigned long blanktime;
        bool blanked=false;
    public:
        void begin(void);
        //Overloading display() so that the caller does not
        //have to distinguish int from char
        void display(int program_num);
        void display_int(int program_num);
        void display_char(char letter);
        void blank(void);
        Segment();
};
#endif