#ifndef __EEPROM_H_
#define __EEPROM_H_

#include <mbed.h>


namespace EEPROM {

    const uint32_t IAP_LOCATION = 0x03000205;

    typedef void (*IAP)(unsigned int [],unsigned int[]);
    
    const IAP iap_entry = (IAP) IAP_LOCATION;

    void init();

    void write(uint32_t address, uint8_t *data, uint32_t bytes);
    void read(uint32_t address, uint8_t *data, uint32_t bytes);

}

#endif
