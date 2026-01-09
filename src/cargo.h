#pragma once

class FootClass;

class CargoClass {
public:
    CargoClass() : Quantity(0), CargoHold(0) {}

#ifdef CHEAT_KEYS
    void Debug_Dump(class MonoClass* mono) const;
#endif

    void AI(void) {}

    int How_Many(void) const { return Quantity; }
    bool Is_Something_Attached(void) const { return (CargoHold != 0); }

    FootClass* Attached_Object(void) const;
    FootClass* Detach_Object(void);
    void Attach(FootClass* object);

    void Code_Pointers(void);
    void Decode_Pointers(void);

private:
    unsigned char Quantity;
    FootClass* CargoHold;
};

