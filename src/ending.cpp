/*
**	Command & Conquer(tm) - ported ending sequence
*/

#include "function.h"
#include "textblit.h"
#include <vector>
#include <cstring>

static void Free_Sample(void const*) {}


void GDI_Ending(void)
{
#ifdef DEMO
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    Load_Title_Screen("DEMOPIC.PCX", &HidPage, Palette);
    HidPage.Blit(SeenBuff);
    Fade_Palette_To(Palette, FADE_PALETTE_MEDIUM, Call_Back);
    Clear_KeyBuffer();
    Get_Key_Num();
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    VisiblePage.Clear();

#else
    if (TempleIoned) {
        Play_Movie("GDIFINB");
    } else {
        Play_Movie("GDIFINA");
    }

    Score.Presentation();

    if (TempleIoned) {
        Play_Movie("GDIEND2");
    } else {
        Play_Movie("GDIEND1");
    }

    CountDownTimerClass count;
    if (CCFileClass("TRAILER.VQA").Is_Available()) {
        Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
        Clear_KeyBuffer();
        count.Set(TIMER_SECOND*3);
        while (count.Time()) {
            Call_Back();
        }
        Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);

        Play_Movie("TRAILER");
    }

    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    Clear_KeyBuffer();
    count.Set(TIMER_SECOND*3);
    while (count.Time()) {
        Call_Back();
    }
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);

    Play_Movie("CC2TEASE");
#endif
}

#ifndef DEMO
void Nod_Ending(void)
{
    static unsigned char const _tanpal[] = {0x0,0xED,0xED,0x2C,0x2C,0xFB,0xFB,0xFD,0xFD,0x0,0x0,0x0,0x0,0x0,0x52,0x0};

    char fname[12];
#ifdef NOT_FOR_WIN95
    std::vector<unsigned char> satpic(64000);
#endif
    int oldfontxspacing = FontXSpacing;
    void const *oldfont;

    Score.Presentation();

    oldfont = nullptr;
    Platform_Set_Fonts(ScoreFontPtr, nullptr, FontHeight, FontYSpacing);

    PseudoSeenBuff = new GraphicBufferClass(320,200,(void*)NULL);
    TextPrintBuffer = new GraphicBufferClass(SeenBuff.Get_Width(), SeenBuff.Get_Height(), (void*)NULL);
    TextPrintBuffer->Clear();
    BlitList.Clear();
    SeenBuff.Clear();
    HidPage.Clear();
    PseudoSeenBuff->Clear();

#ifdef NOT_FOR_WIN95
    auto sat_pal_file = CCFileClass("SATSEL.PAL");
    void * localpal = Load_Alloc_Data(sat_pal_file);
#else
    void * localpal = nullptr;
#endif
    // SATSEL.CPS decompression deferred - use HidPage contents where available.
#ifdef NOT_FOR_WIN95
    memcpy(satpic.data(), HidPage.Get_Buffer(), 64000);
#else
    if (PseudoSeenBuff && SysMemPage.Get_Buffer()) {
        unsigned char* dst = PseudoSeenBuff->Get_Buffer();
        unsigned char const* src = SysMemPage.Get_Buffer();
        if (dst && src) std::memcpy(dst, src, static_cast<std::size_t>(SysMemPage.Get_Width()) * static_cast<std::size_t>(SysMemPage.Get_Height()));
    }
#endif
    auto kanefinl = MixFileClass::Retrieve("KANEFINL.AUD");
    auto loopie6m = MixFileClass::Retrieve("LOOPIE6M.AUD");

    Play_Movie("NODFINAL", THEME_NONE, false);

    Hide_Mouse();
    Call_Back();
    Fade_Palette_To((unsigned char*)localpal, FADE_PALETTE_MEDIUM, Call_Back);
#ifdef NOT_FOR_WIN95
    memcpy(SeenBuff.Get_Buffer(), satpic.data(), 64000);
#endif
    Show_Mouse();

    InterpolationPaletteChanged = TRUE;
    InterpolationPalette = (unsigned char*)localpal;
    Increase_Palette_Luminance(InterpolationPalette , 30,30,30,63);
    Read_Interpolation_Palette("SATSELIN.PAL");
    Interpolate_2X_Scale(PseudoSeenBuff, &SeenBuff,"SATSELIN.PAL");

    Keyboard::Clear();
    extern int Play_Sample(void const *data, int priority, int volume, int pan = 0);
    extern bool Is_Sample_Playing(void const *data);
    if (kanefinl) Play_Sample(kanefinl,255,128);
    if (loopie6m) Play_Sample(loopie6m,255,128);

    bool mouseshown = false;
    bool done = false;
    int selection = 1;
    bool printedtext = false;
    while (!done) {
        if (!printedtext && !Is_Sample_Playing(kanefinl)) {
            printedtext = true;
            Alloc_Object(new ScorePrintClass(Text_String(TXT_SEL_TARGET), 0, 180,_tanpal));
            mouseshown = true;
            Show_Mouse();
        }
        Call_Back_Delay(1);
        if (!Keyboard::Check()) {
            if (!Is_Sample_Playing(loopie6m)) Play_Sample(loopie6m,255,128);
        } else {
            if (Is_Sample_Playing(kanefinl)) {
                Clear_KeyBuffer();
            } else {
                int key = Keyboard::Get();
                if ((key & 0x10FF) == KN_LMOUSE && !(key & KN_RLSE_BIT)) {
                    int mousex = _Kbd->MouseQX;
                    int mousey = _Kbd->MouseQY;
                    if (mousey >= 22*2 && mousey <= 177*2) {
                        done = true;
                        if (mousex <  160*2 && mousey <  100*2) selection = 2;
                        if (mousex <  160*2 && mousey >= 100*2) selection = 3;
                        if (mousex >= 160*2 && mousey >= 100*2) selection = 4;
                    }
                }
            }
        }
    }
    if (mouseshown) Hide_Mouse();
#ifdef NOT_FOR_WIN95
    // satpic vector will clean itself up
#else
    delete PseudoSeenBuff;
#endif

    for (int i = 0; i < MAXSCOREOBJS; i++) if (ScoreObjs[i]) {
        delete ScoreObjs[i];
        ScoreObjs[i] = 0;
    }

    SeenBuff.Fill_Rect(0,180*2,319*2,199*2,0);
    if (TextPrintBuffer) {
        unsigned char* buf = TextPrintBuffer->Get_Buffer();
        if (buf) std::memset(buf + (180*2) * TextPrintBuffer->Get_Width(), 0, static_cast<std::size_t>((199*2 - 180*2 + 1) * TextPrintBuffer->Get_Width()));
    }

    Hide_Mouse();
    Keyboard::Clear();

    Platform_Set_Fonts(oldfont, nullptr, FontHeight, FontYSpacing);
    FontXSpacing = oldfontxspacing;
    Free_Sample(kanefinl);
    Free_Sample(loopie6m);

    sprintf(fname,"NODEND%d",selection);
    PreserveVQAScreen = 1;
    Play_Movie(fname);

    CountDownTimerClass count;
    if (CCFileClass("TRAILER.VQA").Is_Available()) {
        Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
        Clear_KeyBuffer();
        count.Set(TIMER_SECOND*3);
        while (count.Time()) {
            Call_Back();
        }
        Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);

        Play_Movie("TRAILER");
    }

    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);
    Clear_KeyBuffer();
    count.Set(TIMER_SECOND*3);
    while (count.Time()) {
        Call_Back();
    }
    Fade_Palette_To(BlackPalette, FADE_PALETTE_MEDIUM, Call_Back);

    Play_Movie("CC2TEASE");

    delete [] (unsigned char*)localpal;
    delete TextPrintBuffer;
    BlitList.Clear();
}
#endif

