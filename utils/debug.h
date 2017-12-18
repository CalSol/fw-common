#include "mbed.h"

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <cstdio>

#ifndef DEBUG_MODULE
  #define DEBUG_MODULE __FILE__
#endif

namespace calsol {
namespace util {

namespace debugConsole {
  extern char buffer[];
  void puts(const char* string);
}

}}

#ifdef DEBUG_ENABLED

#define STRINGIFY(X) #X
#define TOSTRING(x) STRINGIFY(x)

#define debugPrint(f, ...)  \
  sprintf(calsol::util::debugConsole::buffer, f, ## __VA_ARGS__);  \
  calsol::util::debugConsole::puts(calsol::util::debugConsole::buffer);  \

#define debugInfo(f, ...)  \
  calsol::util::debugConsole::puts("\033[36mInfo)\033[0m " DEBUG_MODULE " " TOSTRING(__LINE__) ": ");  \
  sprintf(calsol::util::debugConsole::buffer, f, ## __VA_ARGS__);  \
  calsol::util::debugConsole::puts(calsol::util::debugConsole::buffer);  \
  calsol::util::debugConsole::puts("\r\n");

#define debugWarn(f, ...)  \
  calsol::util::debugConsole::puts("\033[33mWarn)\033[0m " DEBUG_MODULE " " TOSTRING(__LINE__) ": ");  \
  sprintf(calsol::util::debugConsole::buffer, f, ## __VA_ARGS__);  \
  calsol::util::debugConsole::puts(calsol::util::debugConsole::buffer);  \
  calsol::util::debugConsole::puts("\r\n");

#else

#define debugInfo(f, ...)  ;
#define debugWarn(f, ...)  ;

#endif

#endif
