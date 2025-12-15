/*
** Debug helpers (partial port)
*/

#include "function.h"
#include "wwlib32.h"
#include "wwalloc.h"
#include <unistd.h>

// Forward declaration of PCX writer (filepcx.h not yet ported into include path)
void Write_PCX_File(const char *filename, const GraphicBufferClass &page, unsigned char const *palette);

// Forward declarations used by the debug helpers
void Sound_Callback();

#ifdef CHEAT_KEYS

extern bool ScreenRecording;

void Debug_Key(unsigned input)
{
    if (!input || input & KN_BUTTON) return;

    if (Debug_Flag) {
        switch (input) {
        case SDLK_k: {
            GraphicBufferClass temp_page(SeenBuff.Get_Width(), SeenBuff.Get_Height());
            char filename[30];

            temp_page.Blit(SeenBuff, 0, 0, 0, 0, SeenBuff.Get_Width(), SeenBuff.Get_Height());
                for (int lp = 0; lp < 99; lp++) {
                if (lp < 10) {
                    snprintf(filename, sizeof(filename), "scrsht0%d.pcx", lp);
                } else {
                    snprintf(filename, sizeof(filename), "scrsht%d.pcx", lp);
                }
                if (access(filename, F_OK) == -1)
                    break;
            }

            const unsigned char *pal = Palette ? Palette : GamePalette;
            Write_PCX_File(filename, temp_page, pal);
            break; }

        case SDLK_p:
            Keyboard::Clear();
            while (!Keyboard::Check()) {
                Self_Regulate();
                Sound_Callback();
            }
            Keyboard::Clear();
            break;

        default:
            break;
        }
    }
}

#define UPDATE_INTERVAL TIMER_SECOND
void Self_Regulate(void)
{
    static CountDownTimerClass DebugTimer(BT_SYSTEM);
    static ObjectClass * _lastobject = 0;

    if (!DebugTimer.Time()) {
        DebugTimer.Set(UPDATE_INTERVAL);

        if (MonoClass::Is_Enabled()) {
            MonoClass *mono = MonoClass::Get_Current();
            mono->Set_Default_Attribute(2);

            switch (MonoPage) {
            case 0:
                mono = &MonoArray[0];
                mono->Clear();

                if (CurrentObject.Count()) {
                    _lastobject = CurrentObject[0];
                }
                if (_lastobject && !_lastobject->IsActive) {
                    _lastobject = 0;
                }
                if (_lastobject) {
                    _lastobject->Debug_Dump(mono);
                }
                Logic.Debug_Dump(mono);
                mono->Set_Cursor(0, 20);
                mono->Printf(
                    "Heap size:%10ld \r"
                    "Largest:  %10ld \r"
                    "Ttl Free: %10ld \r"
                    "Frag:     %10ld \r",
                    Heap_Size(MEM_NORMAL),
                    Ram_Free(MEM_NORMAL),
                    Total_Ram_Free(MEM_NORMAL),
                    Total_Ram_Free(MEM_NORMAL)-Ram_Free(MEM_NORMAL)
                );
                *MonoClass::Get_Current() = *mono;
                break;
            }

            MonoArray[MonoPage] = *mono;
        }
    }
}

#endif
