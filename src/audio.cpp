/* Ported from AUDIO.CPP -- EVA and SFX control logic. */
#include "function.h"

// Playback primitives (implemented in audio_play_stub.cpp for now)
extern int Play_Sample(void const *data, int priority, int volume, int pan = 0);
extern bool Is_Sample_Playing(void const *data);
extern void Stop_Sample_Playing(void const *data);

extern int SampleType;
#ifndef SAMPLE_NONE
#define SAMPLE_NONE 0
#endif

/* The original file contains the full sound effect and EVA queues. This
 * port preserves the decision logic and file lookup while relying on the
 * playback primitives implemented elsewhere (Play_Sample, Is_Sample_Playing,
 * Stop_Sample_Playing). Full decoding/mixing will be implemented later. */

typedef enum {
    IN_NOVAR,
    IN_JUV,
    IN_VAR,
} ContextType;

static struct {
    char const *Name;
    int Priority;
    ContextType Where;
} SoundEffectName[VOC_COUNT] = {
    /* (trimmed) keeping original initialization order for compatibility */
    {"BOMBIT1", 20, IN_NOVAR},
    {"CMON1", 20, IN_NOVAR},
    {"GOTIT1", 20, IN_NOVAR},
    {"KEEPEM1", 20, IN_NOVAR},
    {"LAUGH1", 20, IN_NOVAR},
    {"LEFTY1", 20, IN_NOVAR},
    {"NOPRBLM1",20, IN_NOVAR},
    {"ONIT1",20, IN_NOVAR},
    {"RAMYELL1",20, IN_NOVAR},
    {"ROKROLL1",20, IN_NOVAR},
    {"TUFFGUY1",20, IN_NOVAR},
    {"YEAH1",20, IN_NOVAR},
    {"YES1",20, IN_NOVAR},
    {"YO1",20, IN_NOVAR},
    {"GIRLOKAY",20, IN_NOVAR},
    {"GIRLYEAH",20, IN_NOVAR},
    {"GUYOKAY1",20, IN_NOVAR},
    {"GUYYEAH1",20, IN_NOVAR},
    {"2DANGR1",10, IN_VAR},
    {"ACKNO",10, IN_VAR},
    {"AFFIRM1",10, IN_VAR},
    {"AWAIT1",10, IN_VAR},
    {"MOVOUT1",10, IN_VAR},
    {"NEGATV1",10, IN_VAR},
    {"NOPROB",10, IN_VAR},
    {"READY",10, IN_VAR},
    {"REPORT1",10, IN_VAR},
    {"RITWAWA",10, IN_VAR},
    {"ROGER",10, IN_VAR},
    {"UGOTIT",10, IN_VAR},
    {"UNIT1",10, IN_VAR},
    {"VEHIC1",10, IN_VAR},
    {"YESSIR1",10, IN_VAR},
    {"BAZOOK1",1, IN_JUV},
    {"BLEEP2",1, IN_JUV},
    {"BOMB1",1, IN_JUV},
    {"BUTTON",1, IN_JUV},
    {"COMCNTR1",10, IN_JUV},
    {"CONSTRU2",10, IN_JUV},
    {"CRUMBLE",1, IN_JUV},
    {"FLAMER2",4, IN_JUV},
    {"GUN18",4, IN_JUV},
    {"GUN19",4, IN_JUV},
    {"GUN20",4, IN_JUV},
    {"GUN5",4, IN_JUV},
    {"GUN8",4, IN_JUV},
    {"GUNCLIP1",1, IN_JUV},
    {"HVYDOOR1",5, IN_JUV},
    {"HVYGUN10",1, IN_JUV},
    {"ION1",1, IN_JUV},
    {"MGUN11",1, IN_JUV},
    {"MGUN2",1, IN_JUV},
    {"NUKEMISL",1, IN_JUV},
    {"NUKEXPLO",1, IN_JUV},
    {"OBELRAY1",1, IN_JUV},
    {"OBELPOWR",1, IN_JUV},
    {"POWRDN1",1, IN_JUV},
    {"RAMGUN2",1, IN_JUV},
    {"ROCKET1",1, IN_JUV},
    {"ROCKET2",1, IN_JUV},
    {"SAMMOTR2",1, IN_JUV},
    {"SCOLD2",1, IN_JUV},
    {"SIDBAR1C",1, IN_JUV},
    {"SIDBAR2C",1, IN_JUV},
    {"SQUISH2",1, IN_JUV},
    {"TNKFIRE2",1, IN_JUV},
    {"TNKFIRE3",1, IN_JUV},
    {"TNKFIRE4",1, IN_JUV},
    {"TNKFIRE6",1, IN_JUV},
    {"TONE15",0, IN_JUV},
    {"TONE16",0, IN_JUV},
    {"TONE2",1, IN_JUV},
    {"TONE5",10, IN_JUV},
    {"TOSS",1, IN_JUV},
    {"TRANS1",1, IN_JUV},
    {"TREEBRN1",1, IN_JUV},
    {"TURRFIR5",1, IN_JUV},
    {"XPLOBIG4",5, IN_JUV},
    {"XPLOBIG6",5, IN_JUV},
    {"XPLOBIG7",5, IN_JUV},
    {"XPLODE",1, IN_JUV},
    {"XPLOS",4, IN_JUV},
    {"XPLOSML2",5, IN_JUV},
    {"NUYELL1",10, IN_NOVAR},
    {"NUYELL3",10, IN_NOVAR},
    {"NUYELL4",10, IN_NOVAR},
    {"NUYELL5",10, IN_NOVAR},
    {"NUYELL6",10, IN_NOVAR},
    {"NUYELL7",10, IN_NOVAR},
    {"NUYELL10",10, IN_NOVAR},
    {"NUYELL11",10, IN_NOVAR},
    {"NUYELL12",10, IN_NOVAR},
    {"YELL1",1, IN_NOVAR},
    {"MYES1",10, IN_NOVAR},
    {"MCOMND1",10, IN_NOVAR},
    {"MHELLO1",10, IN_NOVAR},
    {"MHMMM1",10, IN_NOVAR},
    {"MPLAN3",10, IN_NOVAR},
    {"MCOURSE1",10, IN_NOVAR},
    {"MYESYES1",10, IN_NOVAR},
    {"MTIBER1",10, IN_NOVAR},
    {"MTHANKS1",10, IN_NOVAR},
    {"CASHTURN",1, IN_NOVAR},
    {"BLEEP2",10, IN_NOVAR},
    {"DINOMOUT",10, IN_NOVAR},
    {"DINOYES",10, IN_NOVAR},
    {"DINOATK1",10, IN_NOVAR},
    {"DINODIE1",10, IN_NOVAR},
};

char const * Speech[VOX_COUNT] = {
    "ACCOM1","FAIL1","BLDG1","CONSTRU1","UNITREDY","NEWOPT1","DEPLOY1","GDIDEAD1",
    "NODDEAD1","CIVDEAD1","NOCASH1","BATLCON1","REINFOR1","CANCEL1","BLDGING1","LOPOWER1",
    "NOPOWER1","MOCASH1","BASEATK1","INCOME1","ENEMYA","NUKE1","NOBUILD1","PRIBLDG1",
    "NODCAPT1","GDICAPT1","IONCHRG1","IONREDY1","NUKAVAIL","NUKLNCH1","UNITLOST","STRCLOST",
    "NEEDHARV","SELECT1","AIRREDY1","NOREDY1","TRANSSEE","TRANLOAD","ENMYAPP1","SILOS1",
    "ONHOLD1","REPAIR1","ESTRUCX","GSTRUC1","NSTRUC1","ENMYUNIT",
};

static VoxType CurrentVoice = VOX_NONE;

void Sound_Effect(VocType voc, COORDINATE coord, int variation)
{
    unsigned distance;
    CELL cell_pos;
    int pan_value;

    if (!Options.Volume || voc == VOC_NONE || !SoundOn || SampleType == SAMPLE_NONE) {
        return;
    }
    if (coord) {
        cell_pos = Coord_Cell(coord);
    }

    distance = 0xFF;
    pan_value = 0;
    if (coord && !Map.In_View(cell_pos)) {
        /* Map.Cell_Distance can be missing on the Map stub; use a conservative default. */
        distance = MAP_CELL_W;
        distance = Cardinal_To_Fixed(MAP_CELL_W, distance);
        distance = MIN(distance, 0xFFu);
        distance ^= 0xFF;

        distance /= 2;
        distance = MAX(distance, 25);

        pan_value  = Cell_X(cell_pos);
        pan_value -= Coord_XCell(Map.TacticalCoord) + (Lepton_To_Cell(Map.TacLeptonWidth) >> 1);
        if (ABS(pan_value) > Lepton_To_Cell(Map.TacLeptonWidth >> 1)) {
            pan_value *= 0x8000;
            pan_value /= (MAP_CELL_W >> 2);
            pan_value = Bound(pan_value, -0x7FFF, 0x7FFF);
        } else {
            pan_value = 0;
        }
    }

    Sound_Effect(voc, (VolType)Fixed_To_Cardinal(distance, Options.Volume), variation, pan_value);
}

int Sound_Effect(VocType voc, VolType volume, int variation, signed short pan_value)
{
    char name[_MAX_FNAME+_MAX_EXT];

    if (!Options.Volume || voc == VOC_NONE || !SoundOn || SampleType == SAMPLE_NONE) {
        return(-1);
    }

    char const * ext = ".AUD";
    if (Special.IsJuvenile && SoundEffectName[voc].Where == IN_JUV) {
        ext = ".JUV";
    } else {
        if (SoundEffectName[voc].Where == IN_VAR) {
            if (variation < 0) {
                if (ABS(variation) % 2) {
                    ext = ".V00";
                } else {
                    ext = ".V02";
                }
            } else {
                if (variation % 2) {
                    ext = ".V01";
                } else {
                    ext = ".V03";
                }
            }
        }
    }
    _makepath(name, NULL, NULL, SoundEffectName[voc].Name, ext);
    void const * ptr = MixFileClass::Retrieve(name);

    if (ptr) {
        return(Play_Sample(ptr, Fixed_To_Cardinal(SoundEffectName[voc].Priority, (int)volume), (int)volume, pan_value));
    }
    return(-1);
}

void Speak(VoxType voice)
{
    if (Options.Volume && SampleType != 0 && voice != VOX_NONE && voice != SpeakQueue && voice != CurrentVoice && SpeakQueue == VOX_NONE) {
        SpeakQueue = voice;
    }
}

void Speak_AI(void)
{
    static VoxType _last = VOX_NONE;
    if (SampleType == 0) return;

    if (!Is_Sample_Playing(SpeechBuffer)) {
        CurrentVoice = VOX_NONE;
        if (SpeakQueue != VOX_NONE) {
            if (SpeakQueue != _last) {
                char name[_MAX_FNAME+_MAX_EXT];

                _makepath(name, NULL, NULL, Speech[SpeakQueue], ".AUD");
                if (CCFileClass(name).Read(SpeechBuffer, SPEECH_BUFFER_SIZE)) {
                    Play_Sample(SpeechBuffer, 254, Options.Volume);
                }
                _last = SpeakQueue;
            } else {
                Play_Sample(SpeechBuffer, 254, Options.Volume);
            }
            SpeakQueue = VOX_NONE;
        }
    }
}

void Stop_Speaking(void)
{
    SpeakQueue = VOX_NONE;
    if (SampleType != 0) {
        Stop_Sample_Playing(SpeechBuffer);
    }
}

bool Is_Speaking(void)
{
    Speak_AI();
    if (SampleType != 0 && (SpeakQueue != VOX_NONE || Is_Sample_Playing(SpeechBuffer))) {
        return(true);
    }
    return(false);
}
