#include "mbed.h"
#include "DmaSerial.h"

extern DmaSerial<1024> swdConsole;

namespace debug {
char buffer[128];

void puts(const char* string) {
  swdConsole.puts(string);
}
}
