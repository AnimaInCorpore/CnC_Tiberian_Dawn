#include "function.h"

GraphicPageClass* LogicPage = NULL;
GraphicBufferClass SeenBuff;

SurfaceManager AllSurfaces = {false};

GameType GameToPlay = GAME_NORMAL;
bool InMainLoop = false;
bool GameActive = false;
bool Debug_Instant_Build = false;
int ScenarioInit = 0;

int FontHeight = 8;
int FontXSpacing = 1;
int FontYSpacing = 1;

void const* FontPtr = NULL;
void const* Green12FontPtr = NULL;
void const* Green12GradFontPtr = NULL;
void const* MapFontPtr = NULL;
void const* VCRFontPtr = NULL;
void const* GradFont6Ptr = NULL;
void const* Font3Ptr = NULL;
void const* Font6Ptr = NULL;
void const* Font8Ptr = NULL;
void const* FontLEDPtr = NULL;

int WindowList[256][4];

VoxType SpeakQueue = VOX_NONE;
HouseClass* PlayerPtr = NULL;
unsigned Frame = 0;

BuildingCollection Buildings;
MapClass Map;
SpecialClass Special;
bool AreThingiesEnabled = false;
int Scenario = 0;
bool Debug_Map = false;
