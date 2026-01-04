#include <SDL.h>
#include <cstring>

#include "legacy/function.h"
#include "legacy/textblit.h"
#include "port_debug.h"

extern bool Is_Sample_Playing(void const* data);
extern void Stop_Sample(int handle);

#ifndef DEMO

/***********************************************************************************************
 * Choose_Side -- play the introduction movies, select house                                   *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   5/08/1995 BWG : Created.                                                                  *
 *=============================================================================================*/
void Choose_Side(void) {
  TD_Debugf("Choose_Side: begin (Screen=%dx%d)", SeenBuff.Get_Width(), SeenBuff.Get_Height());
  static unsigned char const _yellowpal[] = {0x0,  0xC9, 0xBA, 0x93, 0x61, 0xEE, 0xEE, 0x0,
                                             0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0};
  static unsigned char const _redpal[] = {0x0,  0xA8, 0xD9, 0xDA, 0xE1, 0xD4, 0xDA, 0x0,
                                          0xE1, 0x0,  0x0,  0x0,  0x0,  0x0,  0xD4, 0x0};
  static unsigned char const _graypal[] = {0x0,  0x17, 0x10, 0x12, 0x14, 0x1C, 0x12, 0x1C,
                                           0x14, 0x0,  0x0,  0x0,  0x0,  0x0,  0x1C, 0x0};

  void* anim = nullptr;
  void const* staticaud = nullptr;
  void const* oldfont = nullptr;
  void const* speechg = nullptr;
  void const* speechn = nullptr;
  void const* speech = nullptr;
  int statichandle = 0;
  int speechhandle = 0;
  int speechplaying = 0;
  int oldfontxspacing = FontXSpacing;
  int setpalette = 0;

  const int system_mb = SDL_GetSystemRAM();

  TextPrintBuffer = new GraphicBufferClass(SeenBuff.Get_Width(), SeenBuff.Get_Height(), (void*)NULL);
  TextPrintBuffer->Clear();
  BlitList.Clear();
  PseudoSeenBuff = new GraphicBufferClass(320, 200, (void*)NULL);
  int frame = 0, endframe = 255, selection = 0, lettersdone = 0;
  TD_Debugf("Choose_Side: buffers allocated");

  Hide_Mouse();
  oldfont = Set_Font(ScoreFontPtr);

  TD_Debugf("Choose_Side: calling Call_Back()");
  Call_Back();
  TD_Debugf("Choose_Side: Call_Back() returned");

  TD_Debugf("Choose_Side: retrieving audio assets");
  staticaud = MixFileClass::Retrieve("STRUGGLE.AUD");
  speechg = MixFileClass::Retrieve("GDI_SLCT.AUD");
  speechn = MixFileClass::Retrieve("NOD_SLCT.AUD");
  TD_Debugf("Choose_Side: audio retrieved (static=%s gdi=%s nod=%s)",
            staticaud ? "yes" : "no",
            speechg ? "yes" : "no",
            speechn ? "yes" : "no");

  if (Special.IsFromInstall) {
    if (system_mb >= 12) {
      VisiblePage.Clear();
      PreserveVQAScreen = 1;
      TD_Debugf("Choose_Side: playing INTRO2 (install flow)");
      Play_Movie("INTRO2", THEME_NONE, false);
    }
    BreakoutAllowed = true;
  }

  TD_Debugf("Choose_Side: opening CHOOSE.WSA");
  anim = Open_Animation("CHOOSE.WSA", NULL, 0L, (WSAOpenType)(WSA_OPEN_TO_PAGE), Palette);
  TD_Debugf("Choose_Side: Open_Animation(CHOOSE.WSA) returned %s", anim ? "non-null" : "null");
  if (!anim) {
    // If the side-select animation cannot be loaded (e.g. missing mix registration),
    // don't spin forever; keep the preselected side (defaults to GDI) and proceed.
    CCDebugString("Choose_Side: failed to load CHOOSE.WSA; defaulting to current side.\n");
    endframe = 0;
  }
  TD_Debugf("Choose_Side: CHOOSE.WSA frames=%d", Get_Animation_Frame_Count(anim));
  Call_Back();
  InterpolationPaletteChanged = TRUE;
  InterpolationPalette = Palette;
  TD_Debugf("Choose_Side: calling Read_Interpolation_Palette(SIDES.PAL)");
  Read_Interpolation_Palette("SIDES.PAL");
  TD_Debugf("Choose_Side: Read_Interpolation_Palette returned (changed=%s)",
            InterpolationPaletteChanged ? "true" : "false");

  WWMouse->Erase_Mouse(&HidPage, TRUE);
  HiddenPage.Clear();
  PseudoSeenBuff->Clear();
  SysMemPage.Clear();
  VisiblePage.Clear();
  Set_Palette(Palette);

  if (staticaud) {
    statichandle = Play_Sample(staticaud, 255, Options.Normalize_Sound(64));
  }

  CountDownTimerClass sample_timer;
  sample_timer.Set(0x3f);
  Alloc_Object(new ScorePrintClass(TXT_GDI_NAME, 0, 180, _yellowpal));
#ifdef FRENCH
  Alloc_Object(new ScorePrintClass(TXT_GDI_NAME2, 0, 187, _yellowpal));
#endif
  Alloc_Object(new ScorePrintClass(TXT_NOD_NAME, 180, 180, _redpal));

#ifdef GERMAN
  Alloc_Object(new ScorePrintClass(TXT_SEL_TRANS, 57, 190, _graypal));
#else
#ifdef FRENCH
  Alloc_Object(new ScorePrintClass(TXT_SEL_TRANS, 103, 194, _graypal));
#else
  Alloc_Object(new ScorePrintClass(TXT_SEL_TRANS, 103, 190, _graypal));
#endif
#endif
  Keyboard::Clear();

  while (Get_Mouse_State()) Show_Mouse();

  while (endframe != frame || (speechplaying && Is_Sample_Playing(speech))) {
    Animate_Frame(anim, SysMemPage, frame++);
    if (setpalette) {
      Wait_Vert_Blank();
      Set_Palette(Palette);
      setpalette = 0;
    }
    SysMemPage.Blit(*PseudoSeenBuff, 0, 22, 0, 22, 320, 156);

    if (staticaud && (!Is_Sample_Playing(staticaud) || !sample_timer.Time())) {
      if (statichandle) {
        Stop_Sample(statichandle);
      }
      statichandle = Play_Sample(staticaud, 255, 64);
      sample_timer.Set(0x3f);
    }

    Call_Back_Delay(3);

    if (!lettersdone) {
      lettersdone = true;
      for (int i = 0; i < MAXSCOREOBJS; i++)
        if (ScoreObjs[i]) lettersdone = 0;
      if (lettersdone) {
        Show_Mouse();
      }
    }
    if (frame >= Get_Animation_Frame_Count(anim)) frame = 0;
    if (Keyboard::Check() && endframe == 255) {
      if ((Keyboard::Get() & 0x10FF) == KN_LMOUSE) {
        if ((_Kbd->MouseQY > 48 * 2) && (_Kbd->MouseQY < 150 * 2)) {
          if ((_Kbd->MouseQX > 18 * 2) && (_Kbd->MouseQX < 148 * 2)) {
            Whom = HOUSE_GOOD;
            ScenPlayer = SCEN_PLAYER_GDI;
            endframe = 0;
            if (speechg) {
              speechhandle = Play_Sample(speechg, 255, Options.Normalize_Sound(255));
              (void)speechhandle;
              speechplaying = true;
              speech = speechg;
            }
          } else if ((_Kbd->MouseQX > 160 * 2) && (_Kbd->MouseQX < 300 * 2)) {
            selection = 1;
            endframe = 14;
            Whom = HOUSE_BAD;
            ScenPlayer = SCEN_PLAYER_NOD;
            if (speechn) {
              speechhandle = Play_Sample(speechn, 255, Options.Normalize_Sound(255));
              (void)speechhandle;
              speechplaying = true;
              speech = speechn;
            }
          }
        }
      }
    }
  }

  Hide_Mouse();
  Close_Animation(anim);

  PseudoSeenBuff->Fill_Rect(0, 180, 319, 199, 0);
  SeenBuff.Fill_Rect(0, 180 * 2, 319 * 2, 199 * 2, 0);
  TD_Debugf("Choose_Side: interpolating side screen to SeenBuff");
  Interpolate_2X_Scale(PseudoSeenBuff, &SeenBuff, "SIDES.PAL");
  TD_Debugf("Choose_Side: interpolate complete");
  Keyboard::Clear();
  SysMemPage.Clear();

  const bool skip_briefings = (Special.IsJurassic && AreThingiesEnabled);
  if (!skip_briefings) {
    if (Whom == HOUSE_GOOD) {
      Play_Movie("GDI1", THEME_NONE, false);
    } else {
      Play_Movie("NOD1PRE", THEME_NONE, false);
    }
  }

  for (int i = 0; i < MAXSCOREOBJS; i++)
    if (ScoreObjs[i]) {
      delete ScoreObjs[i];
      ScoreObjs[i] = 0;
    }

  if (Whom == HOUSE_GOOD) {
    VisiblePage.Clear();
    std::memset(BlackPalette, 0x01, 768);
    Set_Palette(BlackPalette);
    std::memset(BlackPalette, 0x00, 768);
  } else {
    PreserveVQAScreen = 1;
  }

  Set_Font(oldfont);
  FontXSpacing = oldfontxspacing;

  delete PseudoSeenBuff;
  delete TextPrintBuffer;
  TextPrintBuffer = NULL;
  BlitList.Clear();

  (void)selection;
  TD_Debugf("Choose_Side: end (ScenPlayer=%d Whom=%d)", static_cast<int>(ScenPlayer), static_cast<int>(Whom));
}

#endif
