#include "legacy/intro.h"
#include "legacy/externs.h"
#include "legacy/defines.h"
#include "legacy/function.h"
#include "legacy/msgbox.h"
#include "port_debug.h"

void Choose_Side(void) {
  // Ported behavior: allow the player to pick the campaign side.
  // The Win95 build also plays an interactive animation + briefing VQAs;
  // movie playback is handled separately via Play_Movie().
  const int selection = CCMessageBox(0).Process("Choose your side:", "GDI", "Nod");
  TD_Debugf("Choose_Side: selection=%d", selection);
  if (selection == 0) {
    Whom = HOUSE_GOOD;
    ScenPlayer = SCEN_PLAYER_GDI;
  } else {
    Whom = HOUSE_BAD;
    ScenPlayer = SCEN_PLAYER_NOD;
  }
  TD_Debugf("Choose_Side: Whom=%d ScenPlayer=%d", static_cast<int>(Whom), static_cast<int>(ScenPlayer));
}
