#include "legacy/function.h"

#include "legacy/externs.h"

void Load_Recording_Values(void) {
  // The Win95 build reads the existing multiplayer/profile settings first, then
  // overwrites the pertinent values from the recording. The SDL port currently
  // uses the recording as the authoritative source of these values.

  RecordFile.Read(&GameToPlay, sizeof(GameToPlay));
  RecordFile.Read(&ModemGameToPlay, sizeof(ModemGameToPlay));
  RecordFile.Read(&BuildLevel, sizeof(BuildLevel));
  RecordFile.Read(MPlayerName, sizeof(MPlayerName));
  RecordFile.Read(&MPlayerPrefColor, sizeof(MPlayerPrefColor));
  RecordFile.Read(&MPlayerColorIdx, sizeof(MPlayerColorIdx));
  RecordFile.Read(&MPlayerHouse, sizeof(MPlayerHouse));
  RecordFile.Read(&MPlayerLocalID, sizeof(MPlayerLocalID));
  RecordFile.Read(&MPlayerCount, sizeof(MPlayerCount));
  RecordFile.Read(&MPlayerBases, sizeof(MPlayerBases));
  RecordFile.Read(&MPlayerCredits, sizeof(MPlayerCredits));
  RecordFile.Read(&MPlayerTiberium, sizeof(MPlayerTiberium));
  RecordFile.Read(&MPlayerGoodies, sizeof(MPlayerGoodies));
  RecordFile.Read(&MPlayerGhosts, sizeof(MPlayerGhosts));
  RecordFile.Read(&MPlayerUnitCount, sizeof(MPlayerUnitCount));
  RecordFile.Read(MPlayerID, sizeof(MPlayerID));
  RecordFile.Read(MPlayerHouses, sizeof(MPlayerHouses));
  RecordFile.Read(&Seed, sizeof(Seed));
  RecordFile.Read(&Scenario, sizeof(Scenario));
  RecordFile.Read(&ScenPlayer, sizeof(ScenPlayer));
  RecordFile.Read(&ScenDir, sizeof(ScenDir));
  RecordFile.Read(&Whom, sizeof(Whom));
  RecordFile.Read(&Special, sizeof(SpecialClass));
  RecordFile.Read(&Options, sizeof(GameOptionsClass));
  RecordFile.Read(&FrameSendRate, sizeof(FrameSendRate));
  RecordFile.Read(&CommProtocol, sizeof(CommProtocol));
}

