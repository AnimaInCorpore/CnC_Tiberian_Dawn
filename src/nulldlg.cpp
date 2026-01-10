/*
**	Command & Conquer(tm)
**
**	Portable build note:
**	The original `NULLDLG.CPP` depends on the legacy Win32 serial/modem stack
**	and multiplayer globals that are not ported yet. Keep a stub translation
**	unit here so the CMake + SDL 1.2 build remains stable while those
**	dependencies are brought over.
*/

#include "function.h"
#include "wincomm.h"

int Init_Null_Modem(SerialSettingsType* /*settings*/)
{
	return 0;
}

void Shutdown_Modem(void) {}

void Modem_Signoff(void) {}

int Test_Null_Modem(void)
{
	return 0;
}

void Destroy_Null_Connection(int /*id*/, int /*error*/) {}

GameType Select_Serial_Dialog(void)
{
	return GAME_NORMAL;
}

int Com_Scenario_Dialog(void)
{
	return 0;
}

void Advanced_Modem_Settings(SerialSettingsType* /*settings*/) {}
