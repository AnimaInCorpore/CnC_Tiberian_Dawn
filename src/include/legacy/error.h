#pragma once

// Error/oom hooks used by the legacy allocation layer.
extern void (*Memory_Error)(void);
extern void (*Memory_Error_Exit)(char* string);

void Print_Error_End_Exit(char* string);
void Print_Error_Exit(char* string);

