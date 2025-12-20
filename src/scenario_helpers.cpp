#include "legacy/function.h"

#include <cstdio>
#include <cstring>

bool Restate_Mission(char const* name, int button1, int button2) {
  if (name) {
#ifdef JAPANESE
    char fname[14];
    std::strcpy(fname, name);
    std::strcat(fname, ".CPS");

    if (CCFileClass(fname).Is_Available()) {
      CCMessageBox box(TXT_NONE, true);
      return box.Process(fname, button1, button2);
    }
#else
    bool brief = true;
#ifdef NEWMENU
    char buffer[25];
    char buffer1[25];
    std::sprintf(buffer, "%s.VQA", BriefMovie);
    std::sprintf(buffer1, "%s.VQA", ActionMovie);
    CCFileClass file1(buffer);
    CCFileClass file2(buffer1);
    if (!file1.Is_Available() && !file2.Is_Available()) {
      button1 = TXT_OK;
      button2 = TXT_NONE;
      brief = false;
    }
#endif

    if (std::strlen(BriefingText)) {
      static char buff[512];
      std::strcpy(buff, BriefingText);

      const bool hidden = Get_Mouse_State();
      if (hidden) Show_Mouse();

      if (CCMessageBox(TXT_OBJECTIVE).Process(buff, button1, button2)) {
        if (hidden) Hide_Mouse();
        return true;
      }
      if (hidden) Hide_Mouse();
      if (!brief) return true;
      return false;
    }
#endif
  }
  return false;
}
