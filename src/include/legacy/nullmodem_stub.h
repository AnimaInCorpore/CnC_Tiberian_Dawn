#ifndef NULLMODEM_STUB_H
#define NULLMODEM_STUB_H

// Dummy NullModemClass for porting purposes
class NullModemClass {
public:
    void Change_IRQ_Priority(int priority) { (void)priority; } // Placeholder
};

#endif // NULLMODEM_STUB_H