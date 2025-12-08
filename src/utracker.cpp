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

#include "legacy/utracker.h"

#include <algorithm>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <cstring>

UnitTrackerClass::UnitTrackerClass(int unit_count)
    : UnitTotals(new long[unit_count]), UnitCount(unit_count), InNetworkFormat(0) {
	Clear_Unit_Total();
}

UnitTrackerClass::~UnitTrackerClass(void) { delete[] UnitTotals; }

void UnitTrackerClass::Increment_Unit_Total(int unit_type) {
	if (!UnitTotals || unit_type < 0 || unit_type >= UnitCount) return;
	++UnitTotals[unit_type];
}

void UnitTrackerClass::Decrement_Unit_Total(int unit_type) {
	if (!UnitTotals || unit_type < 0 || unit_type >= UnitCount) return;
	--UnitTotals[unit_type];
}

long* UnitTrackerClass::Get_All_Totals(void) { return UnitTotals; }

void UnitTrackerClass::Clear_Unit_Total(void) {
	if (!UnitTotals) return;
	std::fill(UnitTotals, UnitTotals + UnitCount, 0);
}

void UnitTrackerClass::To_Network_Format(void) {
	if (!UnitTotals || InNetworkFormat) return;
	for (int i = 0; i < UnitCount; ++i) {
		UnitTotals[i] = static_cast<long>(htonl(static_cast<unsigned long>(UnitTotals[i])));
	}
	InNetworkFormat = 1;
}

void UnitTrackerClass::To_PC_Format(void) {
	if (!UnitTotals || !InNetworkFormat) return;
	for (int i = 0; i < UnitCount; ++i) {
		UnitTotals[i] = static_cast<long>(ntohl(static_cast<unsigned long>(UnitTotals[i])));
	}
	InNetworkFormat = 0;
}
