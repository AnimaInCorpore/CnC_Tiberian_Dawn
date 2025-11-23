#ifndef ERROR_STUB_H
#define ERROR_STUB_H

// Declarations for legacy error handling functions
extern void (*Memory_Error)(void);
extern void (*Memory_Error_Exit)(char *string);

void Print_Error_End_Exit(char* string);
void Print_Error_Exit(char* string);

#endif // ERROR_STUB_H