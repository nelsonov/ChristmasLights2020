#include <EEPROM.h>
#define EEPROM_ADDR 0

class NonVolatile
{
    public:
        uint8_t getProgramNum();
        void setProgramNum(uint8_t program_num);
        NonVolatile();
};
