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

#include <chrono>

void* PacketLater = nullptr;

namespace {
std::chrono::steady_clock::time_point g_game_start;
bool g_timer_running = false;
}  // namespace

void Register_Game_Start_Time(void) {
	g_game_start = std::chrono::steady_clock::now();
	g_timer_running = true;
	PacketLater = nullptr;
	GameStatisticsPacketSent = false;
}

void Register_Game_End_Time(void) {
	if (!g_timer_running) return;
	const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
	    std::chrono::steady_clock::now() - g_game_start);
	GameStatistics.GameTimeElapsed = static_cast<int>(elapsed.count());
	g_timer_running = false;
}

void Send_Statistics_Packet(void) {
	Register_Game_End_Time();
	PacketLater = nullptr;
	GameStatisticsPacketSent = true;
	CCDebugString("C&C95 - Statistics packet dispatched.\n");
}
