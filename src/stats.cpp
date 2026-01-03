/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "legacy/function.h"
#include "legacy/tcpip.h"
#include "legacy/externs.h"
#include "packet.h"
#include "ccdde.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>

#if !defined(_WIN32)
#include <unistd.h>
#endif

void* PacketLater = nullptr;

extern unsigned long PlanetWestwoodGameID;
extern unsigned long PlanetWestwoodStartTime;

namespace {
std::chrono::steady_clock::time_point g_game_start;
bool g_timer_running = false;

long Seconds_Since_Game_Start()
{
	if (!g_timer_running) {
		return 0;
	}
	const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - g_game_start);
	return static_cast<long>(elapsed.count());
}

long Total_Physical_Memory_Bytes()
{
#if defined(_WIN32)
	return 0;
#else
	const long page_size = sysconf(_SC_PAGESIZE);
	const long page_count = sysconf(_SC_PHYS_PAGES);
	if (page_size <= 0 || page_count <= 0) {
		return 0;
	}
	const long long total = static_cast<long long>(page_size) * static_cast<long long>(page_count);
	if (total <= 0 || total > static_cast<long long>(INT32_MAX)) {
		return 0;
	}
	return static_cast<long>(total);
#endif
}
}  // namespace

void Register_Game_Start_Time(void) {
	g_game_start = std::chrono::steady_clock::now();
	g_timer_running = true;
	PacketLater = nullptr;
	GameStatisticsPacketSent = false;
}

void Register_Game_End_Time(void) {
	if (!g_timer_running) return;
	GameStatistics.GameTimeElapsed = static_cast<int>(Seconds_Since_Game_Start());
	g_timer_running = false;
}

void Send_Statistics_Packet(void) {
#ifndef DEMO
	PacketClass stats;
	static int packet_size;
	void* packet = nullptr;

	static char field_player_handle[5] = {"NAM?"};
	static char field_player_team[5] = {"SID?"};
	static char field_player_color[5] = {"COL?"};
	static char field_player_credits[5] = {"CRD?"};
	static char field_player_units_left[5] = {"UNL?"};
	static char field_player_infantry_left[5] = {"INL?"};
	static char field_player_planes_left[5] = {"PLL?"};
	static char field_player_buildings_left[5] = {"BLL?"};
	static char field_player_units_bought[5] = {"UNB?"};
	static char field_player_infantry_bought[5] = {"INB?"};
	static char field_player_planes_bought[5] = {"PLB?"};
	static char field_player_buildings_bought[5] = {"BLB?"};
	static char field_player_units_killed[5] = {"UNK?"};
	static char field_player_infantry_killed[5] = {"INK?"};
	static char field_player_planes_killed[5] = {"PLK?"};
	static char field_player_buildings_killed[5] = {"BLK?"};
	static char field_player_buildings_captured[5] = {"BLC?"};
	static char field_player_crates_found[5] = {"CRA?"};
	static char field_player_harvested[5] = {"HRV?"};

	static char* houses[] = {const_cast<char*>("GDI"), const_cast<char*>("NOD"), const_cast<char*>("NUT"), const_cast<char*>("JUR"),
	                         const_cast<char*>("M01"), const_cast<char*>("M02"), const_cast<char*>("M03"), const_cast<char*>("M04"),
	                         const_cast<char*>("M05"), const_cast<char*>("M06")};

	CCDebugString("C&C95 - In Send_Statistics_Packet.\n");

	Register_Game_End_Time();

	// Packet fields mirror the Win95 implementation; some legacy hardware fields are best-effort
	// (the SDL port no longer uses DirectDraw/Winsock).
	static char FIELD_PACKET_TYPE[] = "TYPE";
	static char FIELD_GAME_ID[] = "IDNO";
	static char FIELD_START_CREDITS[] = "CRED";
	static char FIELD_BASES[] = "BASE";
	static char FIELD_TIBERIUM[] = "TIBR";
	static char FIELD_CRATES[] = "CRAT";
	static char FIELD_AI_PLAYERS[] = "AIPL";
	static char FIELD_CAPTURE_THE_FLAG[] = "FLAG";
	static char FIELD_START_UNIT_COUNT[] = "UNIT";
	static char FIELD_TECH_LEVEL[] = "TECH";
	static char FIELD_SCENARIO[] = "SCEN";
	static char FIELD_COMPLETION[] = "CMPL";
	static char FIELD_START_TIME[] = "TIME";
	static char FIELD_GAME_DURATION[] = "DURA";
	static char FIELD_FRAME_RATE[] = "AFPS";
	static char FIELD_SPEED_SETTING[] = "SPED";
	static char FIELD_GAME_VERSION[] = "VERS";
	static char FIELD_COVERT_PRESENT[] = "COVT";
	static char FIELD_MEMORY[] = "MEMO";
	static char FIELD_PLAYER1_HANDLE[] = "NAM1";
	static char FIELD_PLAYER2_HANDLE[] = "NAM2";
	static char FIELD_PLAYER1_TEAM[] = "SID1";
	static char FIELD_PLAYER2_TEAM[] = "SID2";
	static char FIELD_PLAYER1_COLOR[] = "COL1";
	static char FIELD_PLAYER2_COLOR[] = "COL2";
	static char FIELD_PLAYER1_CREDITS[] = "CRD1";
	static char FIELD_PLAYER2_CREDITS[] = "CRD2";
	static char VALUE_ON[] = "ON";
	static char VALUE_OFF[] = "OFF";
	static char VALUE_NULLSVILLE[] = "Nulls-Ville";

	constexpr unsigned char PACKET_TYPE_HOST_GAME_INFO = static_cast<unsigned char>(50);
	constexpr unsigned char PACKET_TYPE_GUEST_GAME_INFO = static_cast<unsigned char>(51);

	enum {
		COMPLETION_CONNECTION_LOST,
		COMPLETION_PLAYER_1_WON,
		COMPLETION_PLAYER_1_WON_BY_RESIGNATION,
		COMPLETION_PLAYER_1_WON_BY_DISCONNECTION,
		COMPLETION_PLAYER_2_WON,
		COMPLETION_PLAYER_2_WON_BY_RESIGNATION,
		COMPLETION_PLAYER_2_WON_BY_DISCONNECTION
	};

	if (!PacketLater) {
		CCDebugString("C&C95 - PacketLater is false.\n");

		stats.Add_Field(FIELD_PACKET_TYPE, Server ? PACKET_TYPE_HOST_GAME_INFO : PACKET_TYPE_GUEST_GAME_INFO);
		stats.Add_Field(FIELD_GAME_ID, PlanetWestwoodGameID);

		stats.Add_Field(FIELD_START_CREDITS, static_cast<unsigned long>(MPlayerCredits));
		stats.Add_Field(FIELD_BASES, MPlayerBases ? VALUE_ON : VALUE_OFF);
		stats.Add_Field(FIELD_TIBERIUM, MPlayerTiberium ? VALUE_ON : VALUE_OFF);
		stats.Add_Field(FIELD_CRATES, MPlayerGoodies ? VALUE_ON : VALUE_OFF);
		stats.Add_Field(FIELD_AI_PLAYERS, MPlayerGhosts ? VALUE_ON : VALUE_OFF);
		stats.Add_Field(FIELD_CAPTURE_THE_FLAG, Special.IsCaptureTheFlag ? VALUE_ON : VALUE_OFF);
		stats.Add_Field(FIELD_START_UNIT_COUNT, static_cast<unsigned long>(MPlayerUnitCount));
		stats.Add_Field(FIELD_TECH_LEVEL, static_cast<unsigned long>(BuildLevel));

		// Scenario display name (Basic/Name in the scenario INI).
		{
			char fname[128]{};
			char namebuffer[40]{};
			std::snprintf(fname, sizeof(fname), "%s.INI", ScenarioName);
			char* abuffer = static_cast<char*>(_ShapeBuffer);
			std::memset(abuffer, '\0', static_cast<size_t>(_ShapeBufferSize));
			CCFileClass fileo;
			fileo.Set_Name(fname);
			fileo.Read(abuffer, _ShapeBufferSize - 1);
			fileo.Close();
			WWGetPrivateProfileString("Basic", "Name", VALUE_NULLSVILLE, namebuffer, static_cast<int>(sizeof(namebuffer)), abuffer);
			stats.Add_Field(FIELD_SCENARIO, namebuffer);
		}

		HouseClass* player1 = HouseClass::As_Pointer(MPlayerHouses[0]);
		HouseClass* player2 = HouseClass::As_Pointer(MPlayerHouses[1]);
		int completion = -1;

		if (ConnectionLost) {
			completion = COMPLETION_CONNECTION_LOST;
		} else if (player1 && player2) {
			if (player1->IGaveUp) {
				completion = COMPLETION_PLAYER_2_WON_BY_DISCONNECTION;
			}
			if (player2->IGaveUp) {
				completion = COMPLETION_PLAYER_1_WON_BY_DISCONNECTION;
			}

			if (player2->IsDefeated) {
				completion = COMPLETION_PLAYER_1_WON;
				if (player2->Resigned) {
					completion = COMPLETION_PLAYER_1_WON_BY_RESIGNATION;
				} else if (player2->IGaveUp) {
					completion = COMPLETION_PLAYER_1_WON_BY_DISCONNECTION;
				}
			} else if (player1->IsDefeated) {
				completion = COMPLETION_PLAYER_2_WON;
				if (player1->Resigned) {
					completion = COMPLETION_PLAYER_2_WON_BY_RESIGNATION;
				} else if (player1->IGaveUp) {
					completion = COMPLETION_PLAYER_2_WON_BY_DISCONNECTION;
				}
			}
		}

		stats.Add_Field(FIELD_COMPLETION, static_cast<char>(completion));
		stats.Add_Field(FIELD_START_TIME, static_cast<long>(PlanetWestwoodStartTime));
		stats.Add_Field(FIELD_GAME_DURATION, static_cast<long>(Seconds_Since_Game_Start()));
		stats.Add_Field(FIELD_FRAME_RATE, 0L);

		stats.Add_Field(FIELD_MEMORY, Total_Physical_Memory_Bytes());
		stats.Add_Field(FIELD_SPEED_SETTING, static_cast<char>(Options.GameSpeed));

		{
			char version[128]{};
			std::snprintf(version, sizeof(version), "%d%s", Version_Number(), VersionText);
			stats.Add_Field(FIELD_GAME_VERSION, version);
		}

		stats.Add_Field(FIELD_COVERT_PRESENT, static_cast<char>(Expansion_Present()));

		for (int house = 0; house < 2; house++) {
			HouseClass* player = HouseClass::As_Pointer(MPlayerHouses[house]);
			if (!player) {
				continue;
			}

			field_player_handle[3] = '1' + static_cast<char>(house);
			stats.Add_Field(field_player_handle, const_cast<char*>(MPlayerNames[house]));

			field_player_team[3] = '1' + static_cast<char>(house);
			stats.Add_Field(field_player_team, houses[player->ActLike]);

			field_player_color[3] = '1' + static_cast<char>(house);
			stats.Add_Field(field_player_color, static_cast<unsigned char>(player->Class->House - HOUSE_MULTI1));

			field_player_credits[3] = '1' + static_cast<char>(house);
			stats.Add_Field(field_player_credits, static_cast<unsigned long>(player->Credits + player->Tiberium));

			field_player_infantry_bought[3] = '1' + static_cast<char>(house);
			field_player_units_bought[3] = '1' + static_cast<char>(house);
			field_player_planes_bought[3] = '1' + static_cast<char>(house);
			field_player_buildings_bought[3] = '1' + static_cast<char>(house);

			player->InfantryTotals->To_Network_Format();
			player->UnitTotals->To_Network_Format();
			player->AircraftTotals->To_Network_Format();
			player->BuildingTotals->To_Network_Format();

			stats.Add_Field(field_player_infantry_bought, (void*)player->InfantryTotals->Get_All_Totals(),
			                player->InfantryTotals->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_units_bought, (void*)player->UnitTotals->Get_All_Totals(), player->UnitTotals->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_planes_bought, (void*)player->AircraftTotals->Get_All_Totals(),
			                player->AircraftTotals->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_buildings_bought, (void*)player->BuildingTotals->Get_All_Totals(),
			                player->BuildingTotals->Get_Unit_Count() * 4);

			player->InfantryTotals->To_PC_Format();
			player->UnitTotals->To_PC_Format();
			player->AircraftTotals->To_PC_Format();
			player->BuildingTotals->To_PC_Format();

			player->InfantryTotals->Clear_Unit_Total();
			player->AircraftTotals->Clear_Unit_Total();
			player->UnitTotals->Clear_Unit_Total();
			player->BuildingTotals->Clear_Unit_Total();

			for (int index = 0; index < Units.Count(); index++) {
				UnitClass const* unit = Units.Ptr(index);
				if (unit->House == player) {
					player->UnitTotals->Increment_Unit_Total(unit->Class->Type);
				}
			}

			for (int index = 0; index < Infantry.Count(); index++) {
				InfantryClass const* infantry = Infantry.Ptr(index);
				if (infantry->House == player && !infantry->Class->IsCivilian) {
					player->InfantryTotals->Increment_Unit_Total(infantry->Class->Type);
				}
			}

			for (int index = 0; index < Aircraft.Count(); index++) {
				AircraftClass const* aircraft = Aircraft.Ptr(index);
				if (aircraft->House == player && aircraft->Class->Type != AIRCRAFT_CARGO) {
					player->AircraftTotals->Increment_Unit_Total(aircraft->Class->Type);
				}
			}

			for (int index = 0; index < Buildings.Count(); index++) {
				BuildingClass const* building = Buildings.Ptr(index);
				if (building->House == player) {
					player->BuildingTotals->Increment_Unit_Total(building->Class->Type);
				}
			}

			player->InfantryTotals->To_Network_Format();
			player->UnitTotals->To_Network_Format();
			player->AircraftTotals->To_Network_Format();
			player->BuildingTotals->To_Network_Format();

			field_player_infantry_left[3] = '1' + static_cast<char>(house);
			field_player_units_left[3] = '1' + static_cast<char>(house);
			field_player_planes_left[3] = '1' + static_cast<char>(house);
			field_player_buildings_left[3] = '1' + static_cast<char>(house);

			stats.Add_Field(field_player_infantry_left, (void*)player->InfantryTotals->Get_All_Totals(),
			                player->InfantryTotals->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_units_left, (void*)player->UnitTotals->Get_All_Totals(), player->UnitTotals->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_planes_left, (void*)player->AircraftTotals->Get_All_Totals(),
			                player->AircraftTotals->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_buildings_left, (void*)player->BuildingTotals->Get_All_Totals(),
			                player->BuildingTotals->Get_Unit_Count() * 4);

			player->DestroyedInfantry->To_Network_Format();
			player->DestroyedUnits->To_Network_Format();
			player->DestroyedAircraft->To_Network_Format();
			player->DestroyedBuildings->To_Network_Format();

			field_player_infantry_killed[3] = '1' + static_cast<char>(house);
			field_player_units_killed[3] = '1' + static_cast<char>(house);
			field_player_planes_killed[3] = '1' + static_cast<char>(house);
			field_player_buildings_killed[3] = '1' + static_cast<char>(house);

			stats.Add_Field(field_player_infantry_killed, (void*)player->DestroyedInfantry->Get_All_Totals(),
			                player->DestroyedInfantry->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_units_killed, (void*)player->DestroyedUnits->Get_All_Totals(),
			                player->DestroyedUnits->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_planes_killed, (void*)player->DestroyedAircraft->Get_All_Totals(),
			                player->DestroyedAircraft->Get_Unit_Count() * 4);
			stats.Add_Field(field_player_buildings_killed, (void*)player->DestroyedBuildings->Get_All_Totals(),
			                player->DestroyedBuildings->Get_Unit_Count() * 4);

			field_player_buildings_captured[3] = '1' + static_cast<char>(house);
			player->CapturedBuildings->To_Network_Format();
			stats.Add_Field(field_player_buildings_captured, (void*)player->CapturedBuildings->Get_All_Totals(),
			                player->CapturedBuildings->Get_Unit_Count() * 4);

			field_player_crates_found[3] = '1' + static_cast<char>(house);
			player->TotalCrates->To_Network_Format();
			stats.Add_Field(field_player_crates_found, (void*)player->TotalCrates->Get_All_Totals(),
			                player->TotalCrates->Get_Unit_Count() * 4);

			field_player_harvested[3] = '1' + static_cast<char>(house);
			stats.Add_Field(field_player_harvested, static_cast<unsigned long>(player->HarvestedCredits));
		}

		CCDebugString("C&C95 - Calling Create_Comms_Packet.\n");
		packet = stats.Create_Comms_Packet(packet_size);
		CCDebugString("C&C95 - Returned from Create_Comms_Packet.\n");

		if (completion == COMPLETION_PLAYER_1_WON_BY_DISCONNECTION || completion == COMPLETION_PLAYER_2_WON_BY_DISCONNECTION) {
			PacketLater = packet;
			CCDebugString("C&C95 - Flagging to send the packet later.\n");
			return;
		}
	} else {
		CCDebugString("C&C95 - PacketLater is true.\n");
		packet = PacketLater;
		PacketLater = nullptr;
	}

	CCDebugString("C&C95 - About to send stats packet to DDE server.\n");
	while (!Send_Data_To_DDE_Server(static_cast<const char*>(packet), packet_size, DDEServerClass::DDE_PACKET_GAME_RESULTS)) {
		CCDebugString("C&C95 - Stats packet send failed.\n");
	}

	CCDebugString("C&C95 - About to delete packet memory.\n");
	delete[] static_cast<unsigned char*>(packet);

	GameStatisticsPacketSent = true;
	CCDebugString("C&C95 - Returning from Send_Statistics_Packet.\n");
#endif  // DEMO
}
