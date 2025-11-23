#include "error_stub.h"
#include <cstdio> // For printf
#include <cstdlib> // For exit

namespace {
void Memory_Error_Impl() {
    printf("Memory_Error: Dummy implementation called. Exiting.\n");
    exit(1);
}

void Memory_Error_Exit_Impl(char* /*message*/) {
    printf("Memory_Error_Exit: Dummy implementation called. Exiting.\n");
    exit(1);
}
}  // namespace

void (*Memory_Error)(void) = Memory_Error_Impl;
void (*Memory_Error_Exit)(char* string) = Memory_Error_Exit_Impl;

void Print_Error_End_Exit(char* error_message) {
    printf("Print_Error_End_Exit: %s (Dummy implementation called. Exiting.)\n", error_message);
    exit(1);
}

void Print_Error_Exit(char* error_message) {
    printf("Print_Error_Exit: %s (Dummy implementation called. Exiting.)\n", error_message);
    exit(1);
}
