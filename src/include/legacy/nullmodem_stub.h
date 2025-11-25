#ifndef NULLMODEM_STUB_H
#define NULLMODEM_STUB_H

// Dummy NullModemClass for porting purposes
class NullModemClass {
public:
    void Change_IRQ_Priority(int priority) { (void)priority; } // Placeholder

    bool Init(int, int, char const*, int, int, int, int, bool) { return true; }

    int Send_Message(void const*, int, int) { return 1; }

    int Service() { return 0; }
};

#endif // NULLMODEM_STUB_H
