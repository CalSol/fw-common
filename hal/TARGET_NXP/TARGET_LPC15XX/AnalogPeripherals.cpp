#include "AnalogPeripherals.h"

#include "analogin_api.h"
#include "pinmap.h"

const uint32_t ADCn_INSEL_ADC = 0x0;
const uint32_t ADCn_INSEL_CORE = 0x1;
const uint32_t ADCn_INSEL_BANDGAP = 0x2;
const uint32_t ADCn_INSEL_TEMPERATURE = 0x3;
const uint32_t SYSCON_PDRUNCFG_TS_PD = 1 << 18;
const uint32_t SYSCON_PDRUNCFG_IREF_PD = 1 << 17;

BandgapReference::BandgapReference() {
  LPC_SYSCON->PDRUNCFG &= ~(SYSCON_PDRUNCFG_IREF_PD | SYSCON_PDRUNCFG_TS_PD);  // enable bandgap sensor

  // power up ADC
  LPC_SYSCON->PDRUNCFG &= ~(1 << 10);
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 27);
  // select IRC as asynchronous clock, divided by 1
  LPC_SYSCON->ADCASYNCCLKSEL  = 0;
  LPC_SYSCON->ADCASYNCCLKDIV  = 1;

  // determine the system clock divider for a 500kHz ADC clock during calibration
  uint32_t clkdiv = (SystemCoreClock / 500000) - 1;

  // perform a self-calibration
  LPC_ADC0->CTRL = (1UL << 30) | (clkdiv & 0xFF);
  while ((LPC_ADC0->CTRL & (1UL << 30)) != 0);

  // switch to asynchronous mode
  LPC_ADC0->CTRL = (1UL << 8);

  _adc.adc = ADC0_0;
}

float BandgapReference::read() {
  LPC_ADC0->INSEL = ADCn_INSEL_BANDGAP;
  float readOut = analogin_read(&_adc);
  LPC_ADC0->INSEL = ADCn_INSEL_ADC;
  return readOut;
}

unsigned short BandgapReference::read_u16() {
  LPC_ADC0->INSEL = ADCn_INSEL_BANDGAP;
  unsigned short readOut = analogin_read_u16(&_adc);
  LPC_ADC0->INSEL = ADCn_INSEL_ADC;
  return readOut;
}

TempSensor::TempSensor() {
  LPC_SYSCON->PDRUNCFG &= ~(SYSCON_PDRUNCFG_IREF_PD | SYSCON_PDRUNCFG_TS_PD);  // enable bandgap sensor

  // power up ADC
  LPC_SYSCON->PDRUNCFG &= ~(1 << 10);
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 27);
  // select IRC as asynchronous clock, divided by 1
  LPC_SYSCON->ADCASYNCCLKSEL  = 0;
  LPC_SYSCON->ADCASYNCCLKDIV  = 1;

  // determine the system clock divider for a 500kHz ADC clock during calibration
  uint32_t clkdiv = (SystemCoreClock / 500000) - 1;

  // perform a self-calibration
  LPC_ADC0->CTRL = (1UL << 30) | (clkdiv & 0xFF);
  while ((LPC_ADC0->CTRL & (1UL << 30)) != 0);

  // switch to asynchronous mode
  LPC_ADC0->CTRL = (1UL << 8);

  _adc.adc = ADC0_0;
}

float TempSensor::read() {
  LPC_ADC0->INSEL = ADCn_INSEL_TEMPERATURE;
  float readOut = analogin_read(&_adc);
  LPC_ADC0->INSEL = ADCn_INSEL_ADC;
  return readOut;
}

unsigned short TempSensor::read_u16() {
  LPC_ADC0->INSEL = ADCn_INSEL_TEMPERATURE;
  unsigned short readOut = analogin_read_u16(&_adc);
  LPC_ADC0->INSEL = ADCn_INSEL_ADC;
  return readOut;
}
