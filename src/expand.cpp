#include "legacy/function.h"

#include <cstring>

#include "legacy/ccfile.h"
#include "legacy/externs.h"
#include "legacy/list.h"
#include "legacy/textbtn.h"
#include "legacy/wwlib32.h"

bool Expansion_Present(void) {
  CCFileClass file("EXPAND.DAT");
  return file.Is_Available();
}

#ifdef NEWMENU

extern bool InMainLoop;

class EListClass : public ListClass {
 public:
  EListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down)
      : ListClass(id, x, y, w, h, flags, up, down) {}

 protected:
  void Draw_Entry(int index, int x, int y, int width, int selected) override;
};

void EListClass::Draw_Entry(int index, int x, int y, int width, int selected) {
  if (TextFlags & TPF_6PT_GRAD) {
    TextPrintType flags = TextFlags;
    if (selected) {
      flags = static_cast<TextPrintType>(flags | TPF_BRIGHT_COLOR);
      LogicPage->Fill_Rect(x, y, x + width - 1, y + LineHeight - 1, CC_GREEN_SHADOW);
    } else {
      if (!(flags & TPF_USE_GRAD_PAL)) {
        flags = static_cast<TextPrintType>(flags | TPF_MEDIUM_COLOR);
      }
    }

    Conquer_Clip_Text_Print(List[index] + sizeof(int), x, y, CC_GREEN, TBLACK, flags, width, Tabs);
  } else {
    Conquer_Clip_Text_Print(List[index] + sizeof(int), x, y, (selected ? BLUE : WHITE), TBLACK, TextFlags,
                            width, Tabs);
  }
}

static void Populate_Expansion_List(EListClass& list, int first_scenario, int last_scenario_exclusive) {
  if (!_ShapeBuffer) return;
  char* scratch = static_cast<char*>(_ShapeBuffer);
  for (int index = first_scenario; index < last_scenario_exclusive; index++) {
    char scenario_name[128]{};
    CCFileClass file;

    Set_Scenario_Name(scenario_name, index, SCEN_PLAYER_GDI, SCEN_DIR_EAST, SCEN_VAR_A);
    std::strcat(scenario_name, ".INI");
    file.Set_Name(scenario_name);
    if (file.Is_Available()) {
      file.Read(scratch, 1000);
      scratch[1000] = '\r';
      scratch[1001] = '\n';
      scratch[1002] = '\0';

      WWGetPrivateProfileString("Basic", "Name", "x", scenario_name, sizeof(scenario_name), scratch);
      char* data = new char[std::strlen(scenario_name) + 1 + sizeof(int) + 25];
      *reinterpret_cast<int*>(&data[0]) = index;
      std::strcpy(&data[sizeof(int)], "GDI: ");
      std::strcat(&data[sizeof(int)], scenario_name);
      list.Add_Item(data);
    }
  }

  for (int index = first_scenario; index < last_scenario_exclusive; index++) {
    char scenario_name[128]{};
    CCFileClass file;

    Set_Scenario_Name(scenario_name, index, SCEN_PLAYER_NOD, SCEN_DIR_EAST, SCEN_VAR_A);
    std::strcat(scenario_name, ".INI");
    file.Set_Name(scenario_name);
    if (file.Is_Available()) {
      file.Read(scratch, 1000);
      scratch[1000] = '\r';
      scratch[1001] = '\n';
      scratch[1002] = '\0';

      WWGetPrivateProfileString("Basic", "Name", "x", scenario_name, sizeof(scenario_name), scratch);
      char* data = new char[std::strlen(scenario_name) + 1 + sizeof(int) + 25];
      *reinterpret_cast<int*>(&data[0]) = index;
      std::strcpy(&data[sizeof(int)], "NOD: ");
      std::strcat(&data[sizeof(int)], scenario_name);
      list.Add_Item(data);
    }
  }
}

static void Populate_Bonus_List(EListClass& list) {
  static const int kGdiScenNames[3] = {TXT_BONUS_MISSION_1, TXT_BONUS_MISSION_2, TXT_BONUS_MISSION_3};
  static const int kNodScenNames[2] = {TXT_BONUS_MISSION_4, TXT_BONUS_MISSION_5};

  for (int index = 60; index < 63; index++) {
    char scenario_name[128]{};
    CCFileClass file;

    Set_Scenario_Name(scenario_name, index, SCEN_PLAYER_GDI, SCEN_DIR_EAST, SCEN_VAR_A);
    std::strcat(scenario_name, ".INI");
    file.Set_Name(scenario_name);
    if (file.Is_Available()) {
      std::strncpy(scenario_name, Text_String(kGdiScenNames[index - 60]), sizeof(scenario_name) - 1);
      char* data = new char[std::strlen(scenario_name) + 1 + sizeof(int) + 25];
      *reinterpret_cast<int*>(&data[0]) = index;
      std::strcpy(&data[sizeof(int)], "GDI: ");
      std::strcat(&data[sizeof(int)], scenario_name);
      list.Add_Item(data);
    }
  }

  for (int index = 60; index < 62; index++) {
    char scenario_name[128]{};
    CCFileClass file;

    Set_Scenario_Name(scenario_name, index, SCEN_PLAYER_NOD, SCEN_DIR_EAST, SCEN_VAR_A);
    std::strcat(scenario_name, ".INI");
    file.Set_Name(scenario_name);
    if (file.Is_Available()) {
      std::strncpy(scenario_name, Text_String(kNodScenNames[index - 60]), sizeof(scenario_name) - 1);
      char* data = new char[std::strlen(scenario_name) + 1 + sizeof(int) + 25];
      *reinterpret_cast<int*>(&data[0]) = index;
      std::strcpy(&data[sizeof(int)], "NOD: ");
      std::strcat(&data[sizeof(int)], scenario_name);
      list.Add_Item(data);
    }
  }
}

static void Free_List_Items(EListClass& list) {
  for (int index = 0; index < list.Count(); index++) {
    delete[] const_cast<char*>(list.Get_Item(index));
  }
}

bool Expansion_Dialog(void) {
  const int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

  const int option_width = 236 * factor;
  const int option_height = 162 * factor;
  const int option_x = (320 * factor - option_width) / 2;
  const int option_y = (200 * factor - option_height) / 2;

  void const* up_button = nullptr;
  void const* down_button = nullptr;

  if (InMainLoop) {
    up_button = Hires_Retrieve("BTN-UP.SHP");
    down_button = Hires_Retrieve("BTN-DN.SHP");
  } else {
    up_button = Hires_Retrieve("BTN-UP2.SHP");
    down_button = Hires_Retrieve("BTN-DN2.SHP");
  }

  TextButtonClass ok(200, TXT_OK, static_cast<TextPrintType>(TPF_6PT_GRAD | TPF_NOSHADOW),
                     option_x + 25 * factor, option_y + option_height - 15 * factor);
  TextButtonClass cancel(201, TXT_CANCEL, static_cast<TextPrintType>(TPF_6PT_GRAD | TPF_NOSHADOW),
                         option_x + option_width - 50 * factor, option_y + option_height - 15 * factor);
  EListClass list(202, option_x + 10 * factor, option_y + 20 * factor, option_width - 20 * factor,
                  option_height - 40 * factor,
                  static_cast<TextPrintType>(TPF_6PT_GRAD | TPF_NOSHADOW), up_button, down_button);

  GadgetClass* buttons = &ok;
  cancel.Add(*buttons);
  list.Add(*buttons);

  Populate_Expansion_List(list, 20, 60);

  Set_Logic_Page(SeenBuff);
  bool display = true;
  bool process = true;
  bool okval = true;
  while (process) {
    Call_Back();

    if (AllSurfaces.SurfacesRestored) {
      AllSurfaces.SurfacesRestored = FALSE;
      display = true;
    }

    if (display) {
      display = false;
      Hide_Mouse();
      Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
      HidPage.Blit(SeenBuff);

      Dialog_Box(option_x, option_y, option_width, option_height);
      Draw_Caption(TXT_MISSION_DESCRIPTION, option_x, option_y, option_width);
      buttons->Draw_All();
      Show_Mouse();
    }

    KeyNumType input = buttons->Input();
    switch (input) {
      case KN_RETURN:
      case 200 | KN_BUTTON:
        if (!list.Current_Item()) {
          process = false;
          okval = false;
          break;
        }
        if (list.Current_Item()[sizeof(int)] == 'G') {
          ScenPlayer = SCEN_PLAYER_GDI;
        } else {
          ScenPlayer = SCEN_PLAYER_NOD;
        }
        ScenDir = SCEN_DIR_EAST;
        Whom = HOUSE_GOOD;
        Scenario = *reinterpret_cast<const int*>(list.Current_Item());
        process = false;
        okval = true;
        break;

      case KN_ESC:
      case 201 | KN_BUTTON:
        ScenPlayer = SCEN_PLAYER_GDI;
        ScenDir = SCEN_DIR_EAST;
        Whom = HOUSE_GOOD;
        if (list.Current_Item()) {
          Scenario = *reinterpret_cast<const int*>(list.Current_Item());
        }
        process = false;
        okval = false;
        break;

      default:
        break;
    }
  }

  Free_List_Items(list);
  return okval;
}

bool Bonus_Dialog(void) {
  const int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;

  const int option_width = 236 * factor;
  const int option_height = 162 * factor;
  const int option_x = (320 * factor - option_width) / 2;
  const int option_y = (200 * factor - option_height) / 2;

  void const* up_button = nullptr;
  void const* down_button = nullptr;

  if (InMainLoop) {
    up_button = Hires_Retrieve("BTN-UP.SHP");
    down_button = Hires_Retrieve("BTN-DN.SHP");
  } else {
    up_button = Hires_Retrieve("BTN-UP2.SHP");
    down_button = Hires_Retrieve("BTN-DN2.SHP");
  }

  TextButtonClass ok(200, TXT_OK, static_cast<TextPrintType>(TPF_6PT_GRAD | TPF_NOSHADOW),
                     option_x + 25 * factor, option_y + option_height - 15 * factor);
  TextButtonClass cancel(201, TXT_CANCEL, static_cast<TextPrintType>(TPF_6PT_GRAD | TPF_NOSHADOW),
                         option_x + option_width - 50 * factor, option_y + option_height - 15 * factor);
  EListClass list(202, option_x + 10 * factor, option_y + 20 * factor, option_width - 20 * factor,
                  option_height - 40 * factor,
                  static_cast<TextPrintType>(TPF_6PT_GRAD | TPF_NOSHADOW), up_button, down_button);

  GadgetClass* buttons = &ok;
  cancel.Add(*buttons);
  list.Add(*buttons);

  Populate_Bonus_List(list);

  Set_Logic_Page(SeenBuff);
  bool display = true;
  bool process = true;
  bool okval = true;
  while (process) {
    Call_Back();

    if (AllSurfaces.SurfacesRestored) {
      AllSurfaces.SurfacesRestored = FALSE;
      display = true;
    }

    if (display) {
      display = false;
      Hide_Mouse();
      Load_Title_Screen(const_cast<char*>("HTITLE.PCX"), &HidPage, Palette);
      HidPage.Blit(SeenBuff);

      Dialog_Box(option_x, option_y, option_width, option_height);
      Draw_Caption(TXT_BONUS_MISSIONS, option_x, option_y, option_width);
      buttons->Draw_All();
      Show_Mouse();
    }

    KeyNumType input = buttons->Input();
    switch (input) {
      case KN_RETURN:
      case 200 | KN_BUTTON:
        if (!list.Current_Item()) {
          process = false;
          okval = false;
          break;
        }
        if (list.Current_Item()[sizeof(int)] == 'G') {
          ScenPlayer = SCEN_PLAYER_GDI;
        } else {
          ScenPlayer = SCEN_PLAYER_NOD;
        }
        ScenDir = SCEN_DIR_EAST;
        Whom = HOUSE_GOOD;
        Scenario = *reinterpret_cast<const int*>(list.Current_Item());
        process = false;
        okval = true;
        break;

      case KN_ESC:
      case 201 | KN_BUTTON:
        ScenPlayer = SCEN_PLAYER_GDI;
        ScenDir = SCEN_DIR_EAST;
        Whom = HOUSE_GOOD;
        if (list.Current_Item()) {
          Scenario = *reinterpret_cast<const int*>(list.Current_Item());
        }
        process = false;
        okval = false;
        break;

      default:
        break;
    }
  }

  Free_List_Items(list);
  return okval;
}

#endif  // NEWMENU
