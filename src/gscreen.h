#pragma once

#include "function.h"
#include "cell.h"

class GadgetClass;
class GraphicBufferClass;

template <typename T>
class VectorClass;

class GScreenClass : public VectorClass<CellClass> {
public:
    GScreenClass(void);

    /*
    ** Initialization
    */
    virtual void One_Time(void);           // One-time initializations
    virtual void Init(TheaterType theater = THEATER_NONE); // Inits everything
    virtual void Init_Clear(void);         // Clears all to known state
    virtual void Init_IO(void);            // Inits button list
    virtual void Init_Theater(TheaterType theater); // Theater-specific inits

    /*
    ** Player I/O is routed through here. It is called every game tick.
    */
    virtual void Input(KeyNumType& key, int& x, int& y);
    virtual void AI(KeyNumType&, int, int) {}
    virtual void Add_A_Button(GadgetClass& gadget);
    virtual void Remove_A_Button(GadgetClass& gadget);

    /*
    ** Called when map needs complete updating.
    */
    virtual void Flag_To_Redraw(bool complete = false);

    /*
    ** Render maintenance routine (call every game tick). Probably no need
    ** to override this in derived classes.
    */
    virtual void Render(void);

    /*
    ** Is called when actual drawing is required. This is the function to
    ** override in derived classes.
    */
    virtual void Draw_It(bool = false) {}

    /*
    ** This moves the hidpage up to the seenpage.
    */
    static void Blit_Display(void);

    /*
    ** Changes the mouse shape as indicated.
    */
    virtual void Set_Default_Mouse(MouseType mouse, bool wwsmall) {}
    virtual bool Override_Mouse_Shape(MouseType mouse, bool wwsmall) { return false; }
    virtual void Revert_Mouse_Shape(void) {}
    virtual void Mouse_Small(bool wwsmall) {}

    /*
    ** File I/O.
    */
    virtual void Code_Pointers(void);
    virtual void Decode_Pointers(void);

    /*
    ** Misc routines.
    */
    virtual void* Shadow_Address(void) { return ShadowPage; }

    /*
    ** This points to the buttons that are used for input. All of the derived classes will
    ** attached their specific buttons to this list.
    */
    static GadgetClass* Buttons;

    static GraphicBufferClass* ShadowPage;

private:
    unsigned IsToRedraw : 1;
    unsigned IsToUpdate : 1;
};

