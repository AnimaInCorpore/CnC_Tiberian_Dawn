/* Minimal port of the intro chooser so the Intro menu button works.
 * The original Choose_Side() provides a complex interactive flow; for
 * now play the configured intro movie and return to the menu.
 */

#include "legacy/intro.h"
#include "legacy/function.h"
#include "legacy/externs.h"

void Choose_Side(void) {
  // If IntroMovie is configured, use it, otherwise fall back to the
  // classic INTRO2 movie used by the original.
  if (IntroMovie[0]) {
    Play_Movie(IntroMovie);
  } else {
    Play_Movie("INTRO2", THEME_NONE, false);
  }
}
