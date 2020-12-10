#include "segment.h"

Segment::Segment (void)
{
    this->blanktime=millis() + DISPLAY_TIME;
}

void Segment::begin(void)
{
    byte segmentPins[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};
    byte digitPins[] = {};
    this->sevseg.begin(COMMON_ANODE, 1, digitPins, segmentPins, true, false, false, true);
    this->sevseg.setBrightness(SEGMENT_BRIGHTNESS);

}

void Segment::display(int program_num)
{
    if (program_num == 10) {
        this->display_char(67);
    } else if (program_num == 11) {
        this->display_char(65);
    } else if ((program_num > 0) && (program_num < 10)) {
        this->display_int(program_num);
    } else {
        this->display_char(69);
    }
}

void Segment::display_int(int digit)
{
    this->sevseg.setNumber(digit);
    this->sevseg.refreshDisplay();
}

void Segment::display_char(char letter)
{
    char chars[]={letter, 0};
    this->sevseg.setChars(chars);
    this->sevseg.refreshDisplay();
}

void Segment::blank(void)
{
    if (this->blanked == false) {
        unsigned long now=millis();
        if (now > this->blanktime) {
            this->sevseg.blank();
            this->blanked=true;
        }
    }
}