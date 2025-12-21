/* Ported from STARTUP.CPP -- fatal error/exit helpers. */

#include "legacy/error.h"

#include "legacy/function.h"

#include <cstdio>
#include <cstdlib>

namespace {
void Memory_Error_Impl() {
  Print_Error_End_Exit(const_cast<char*>("Memory allocation failure."));
}

void Memory_Error_Exit_Impl(char* message) {
  Print_Error_End_Exit(message ? message : const_cast<char*>("Memory allocation failure."));
}
} // namespace

void (*Memory_Error)(void) = Memory_Error_Impl;
void (*Memory_Error_Exit)(char* string) = Memory_Error_Exit_Impl;

void Print_Error_End_Exit(char* string) {
  if (string && *string) {
    std::printf("%s\n", string);
  }
  Get_Key();
  Prog_End();
  if (string && *string) {
    std::printf("%s\n", string);
  }
  std::exit(1);
}

void Print_Error_Exit(char* string) {
  if (string && *string) {
    std::printf("%s\n", string);
  }
  std::exit(1);
}

