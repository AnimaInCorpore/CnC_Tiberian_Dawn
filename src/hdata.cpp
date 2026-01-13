#include "function.h"

/*
** These are the colors used to identify the various owners.
*/
static const int COLOR_GOOD = 180;          // GOLD
static const int COLOR_BRIGHT_GOOD = 176;   // GOLD
static const int COLOR_BAD = 123;           // RED
static const int COLOR_BRIGHT_BAD = 127;    // RED
static const int COLOR_NEUTRAL = 205;       // WHITE
static const int COLOR_BRIGHT_NEUTRAL = 202;  // WHITE

static HouseTypeClass const HouseGood(HOUSE_GOOD,
                                      "GoodGuy",
                                      TXT_GDI,
                                      "GDI",
                                      0,
                                      COLOR_GOOD,
                                      COLOR_BRIGHT_GOOD,
                                      REMAP_YELLOW,
                                      RemapYellow,
                                      'G');

static HouseTypeClass const HouseBad(HOUSE_BAD,
                                     "BadGuy",
                                     TXT_NOD,
                                     "NOD",
                                     0,
                                     COLOR_BAD,
                                     COLOR_BRIGHT_BAD,
                                     REMAP_BLUE,
                                     RemapBlue,
                                     'B');

static HouseTypeClass const HouseCivilian(HOUSE_NEUTRAL,
                                          "Neutral",
                                          TXT_CIVILIAN,
                                          "CIV",
                                          0,
                                          COLOR_NEUTRAL,
                                          COLOR_BRIGHT_NEUTRAL,
                                          REMAP_YELLOW,
                                          RemapNone,
                                          'C');

static HouseTypeClass HouseJP(HOUSE_JP,
                              "Special",
                              TXT_JP,
                              "JP",
                              0,
                              COLOR_NEUTRAL,
                              COLOR_BRIGHT_NEUTRAL,
                              REMAP_YELLOW,
                              RemapNone,
                              'J');

static HouseTypeClass const HouseMulti1(HOUSE_MULTI1,
                                        "Multi1",
                                        TXT_CIVILIAN,
                                        "MP1",
                                        0,
                                        COLOR_NEUTRAL,
                                        COLOR_BRIGHT_NEUTRAL,
                                        REMAP_AQUA,
                                        RemapBlueGreen,
                                        'M');

static HouseTypeClass const HouseMulti2(HOUSE_MULTI2,
                                        "Multi2",
                                        TXT_CIVILIAN,
                                        "MP2",
                                        0,
                                        COLOR_NEUTRAL,
                                        COLOR_BRIGHT_NEUTRAL,
                                        REMAP_ORANGE,
                                        RemapOrange,
                                        'M');

static HouseTypeClass const HouseMulti3(HOUSE_MULTI3,
                                        "Multi3",
                                        TXT_CIVILIAN,
                                        "MP3",
                                        0,
                                        COLOR_NEUTRAL,
                                        COLOR_BRIGHT_NEUTRAL,
                                        REMAP_GREEN,
                                        RemapGreen,
                                        'M');

static HouseTypeClass const HouseMulti4(HOUSE_MULTI4,
                                        "Multi4",
                                        TXT_CIVILIAN,
                                        "MP4",
                                        0,
                                        COLOR_NEUTRAL,
                                        COLOR_BRIGHT_NEUTRAL,
                                        REMAP_BLUE,
                                        RemapBlue,
                                        'M');

static HouseTypeClass const HouseMulti5(HOUSE_MULTI5,
                                        "Multi5",
                                        TXT_CIVILIAN,
                                        "MP5",
                                        0,
                                        COLOR_NEUTRAL,
                                        COLOR_BRIGHT_NEUTRAL,
                                        REMAP_YELLOW,
                                        RemapYellow,
                                        'M');

static HouseTypeClass const HouseMulti6(HOUSE_MULTI6,
                                        "Multi6",
                                        TXT_CIVILIAN,
                                        "MP6",
                                        0,
                                        COLOR_NEUTRAL,
                                        COLOR_BRIGHT_NEUTRAL,
                                        REMAP_RED,
                                        RemapRed,
                                        'M');

HouseTypeClass const* const HouseTypeClass::Pointers[HOUSE_COUNT] = {
    &HouseGood,
    &HouseBad,
    &HouseCivilian,
    &HouseJP,
    &HouseMulti1,
    &HouseMulti2,
    &HouseMulti3,
    &HouseMulti4,
    &HouseMulti5,
    &HouseMulti6,
};

HouseTypeClass::HouseTypeClass(HousesType house,
                               char const* ini,
                               int fullname,
                               char const* ext,
                               int lemon,
                               int color,
                               int bright_color,
                               PlayerColorType remapcolor,
                               unsigned char const* remap,
                               char prefix)
    : RemapTable(remap),
      RemapColor(remapcolor),
      House(house),
      IniName(ini),
      FullName(fullname),
      Lemon(lemon),
      Color(color),
      BrightColor(bright_color),
      Prefix(prefix) {
    std::strncpy(Suffix, ext, 3);
    Suffix[3] = '\0';
}

HousesType HouseTypeClass::From_Name(char const* name)
{
    if (name) {
        for (int h = static_cast<int>(HOUSE_FIRST); h < static_cast<int>(HOUSE_COUNT); ++h) {
            HousesType house = static_cast<HousesType>(h);
            if (stricmp(Pointers[house]->IniName, name) == 0) {
                return house;
            }
        }
    }
    return HOUSE_NONE;
}

void HouseTypeClass::One_Time(void)
{
    /*
    ** Change the radar color for special units; otherwise, they'll be the same
    ** color as the player!
    */
    if (Special.IsJurassic && AreThingiesEnabled) {
        HouseJP.Color = COLOR_BAD;
        HouseJP.BrightColor = COLOR_BRIGHT_BAD;
    }
}

HouseTypeClass const& HouseTypeClass::As_Reference(HousesType house)
{
    return *Pointers[house];
}

