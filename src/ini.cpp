/*
 * Scenario INI loading/saving.
 *
 * Full scenario parsing requires the map/object/mission stack; for now we keep
 * the scenario filename generation behavior and stub the heavyweight routines
 * so the portable build stays linkable while upstream modules are ported.
 */

#include "function.h"

#include "ccfile.h"
#include "house.h"

#include <cstdlib>
#include <cstdio>

static int Random_Pick(int low, int high)
{
    if (high < low) {
        int tmp = low;
        low = high;
        high = tmp;
    }

    if (high == low) {
        return low;
    }

    return low + (std::rand() % (high - low + 1));
}

void Set_Scenario_Name(char* buf, int scenario, ScenarioPlayerType player, ScenarioDirType dir, ScenarioVarType var)
{
    char c_player;
    char c_dir;
    char c_var;
    ScenarioVarType i;
    char fname[_MAX_FNAME + _MAX_EXT];

    switch (player) {
        case SCEN_PLAYER_GDI:
            c_player = HouseTypeClass::As_Reference(HOUSE_GOOD).Prefix;
            break;

        case SCEN_PLAYER_NOD:
            c_player = HouseTypeClass::As_Reference(HOUSE_BAD).Prefix;
            break;

        case SCEN_PLAYER_JP:
            c_player = HouseTypeClass::As_Reference(HOUSE_JP).Prefix;
            break;

        default:
            c_player = HouseTypeClass::As_Reference(HOUSE_MULTI1).Prefix;
            break;
    }

    switch (dir) {
        case SCEN_DIR_EAST:
            c_dir = 'E';
            break;

        case SCEN_DIR_WEST:
            c_dir = 'W';
            break;

        default:
        case SCEN_DIR_NONE:
            c_dir = (Random_Pick(0, 1) == 0) ? 'W' : 'E';
            break;
    }

    if (var == SCEN_VAR_NONE) {
        for (i = SCEN_VAR_FIRST; i < SCEN_VAR_COUNT; i = static_cast<ScenarioVarType>(static_cast<int>(i) + 1)) {
            ::snprintf(fname, sizeof(fname), "SC%c%02d%c%c.INI", c_player, scenario, c_dir, static_cast<char>('A' + i));
            if (!CCFileClass(fname).Is_Available()) {
                break;
            }
        }

        if (i == SCEN_VAR_FIRST) {
            c_var = 'X';
        } else {
            c_var = static_cast<char>('A' + Random_Pick(0, static_cast<int>(i) - 1));
        }
    } else {
        switch (var) {
            case SCEN_VAR_A:
                c_var = 'A';
                break;
            case SCEN_VAR_B:
                c_var = 'B';
                break;
            case SCEN_VAR_C:
                c_var = 'C';
                break;
            case SCEN_VAR_D:
                c_var = 'D';
                break;
            default:
                c_var = 'L';
                break;
        }
    }

    ::snprintf(buf, static_cast<size_t>(_MAX_FNAME + _MAX_EXT), "SC%c%02d%c%c", c_player, scenario, c_dir, c_var);
}

bool Read_Scenario_Ini(char* /*root*/, bool /*fresh*/)
{
    return false;
}

void Write_Scenario_Ini(char* /*root*/)
{
}
