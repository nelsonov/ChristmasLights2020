#include "nonvolatile.h"

NonVolatile::NonVolatile(void)
{
    uint8_t program_num;
    program_num = EEPROM.read(EEPROM_ADDR);
    if (program_num > 11) {
        program_num=1;
        EEPROM.write(EEPROM_ADDR, program_num);
    }
}

uint8_t NonVolatile::getProgramNum(void)
{
    uint8_t program_num;
    program_num = EEPROM.read(EEPROM_ADDR);
    return (program_num);
}

void NonVolatile::setProgramNum(uint8_t program_num)
{
    EEPROM.update(EEPROM_ADDR, program_num);
}
