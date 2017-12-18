#include "mbed.h"
#include "DmaSerial.h"

extern DmaSerial<1024> swdConsole;

namespace calsol {
namespace util {

namespace debugConsole {
char buffer[128];

void puts(const char* string) {
  swdConsole.puts(string);
}
}

}}
