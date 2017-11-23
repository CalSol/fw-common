#ifndef _ANALOGPERIPHERALS_H_
#define _ANALOGPERIPHERALS_H_

#include "analogin_api.h"

#include "mbed.h"

// Internal bandgap reference with the internal ADC
class BandgapReference {
public:
  BandgapReference();

  float read();
  unsigned short read_u16();

protected:
    analogin_t _adc;
};

// Internal temperature sensor with the internal ADC
class TempSensor {
public:
  TempSensor();
  float read();
  unsigned short read_u16();

protected:
    analogin_t _adc;
};

#endif
