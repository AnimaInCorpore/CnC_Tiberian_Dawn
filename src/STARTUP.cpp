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

/* $Header:   F:\projects\c&c\vcs\code\startup.cpv   2.17   16 Oct 1995 16:48:12   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***             C O N F I D E N T I A L  ---  W E S T W O O D   S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : STARTUP.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : October 3, 1994                                              *
 *                                                                                             *
 *                  Last Update : August 27, 1995 [JLB]                                        *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Delete_Swap_Files -- Deletes previously existing swap files.                              *
 *   Prog_End -- Cleans up library systems in prep for game exit.                              *
 *   main -- Initial startup routine (preps library systems).                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "legacy/function.h"
#include "legacy/ccdde.h"
#include "legacy/windows_compat.h"
#include "legacy/wwlib32.h"

#include <array>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

bool Read_Private_Config_Struct(char* profile, NewConfigType* config);
void Delete_Swap_Files(void);
void Print_Error_End_Exit(char* string);
void Print_Error_Exit(char* string);
void Read_Setup_Options(RawFileClass* config_file);

extern WinTimerClass* WindowsTimer;

bool VideoBackBufferAllowed = true;
void Check_From_WChat(char* wchat_name);
bool SpawnedFromWChat = false;

extern bool ReadyToQuit;

extern "C" {
bool __cdecl Detect_MMX_Availability(void);
void __cdecl Init_MMX(void);
}

namespace {

constexpr std::size_t kMaxArgs = 20;
constexpr unsigned kTimerSleepMs = 1000;
constexpr char kDefaultExeName[] = "CONQUER.EXE";

std::string Build_Executable_Name(HINSTANCE instance) {
	(void)instance;
	return std::string(kDefaultExeName);
}

std::vector<std::vector<char>> Build_Arg_Storage(const std::string& exe_path, const char* command_line) {
	std::vector<std::vector<char>> storage;
	storage.reserve(kMaxArgs);

	storage.emplace_back(exe_path.begin(), exe_path.end());
	storage.back().push_back('\0');

	if (!command_line) {
		return storage;
	}

	const char* scan = command_line;
	while (*scan && storage.size() < kMaxArgs) {
		while (*scan == ' ') {
			++scan;
		}
		if (*scan == '\0' || *scan == '\r') {
			break;
		}
		const char* start = scan;
		while (*scan != ' ' && *scan != '\0' && *scan != '\r') {
			++scan;
		}
		storage.emplace_back(start, scan);
		storage.back().push_back('\0');
		if (*scan == '\0' || *scan == '\r') {
			break;
		}
		++scan;
	}

	return storage;
}

std::vector<char*> Materialize_Argv(std::vector<std::vector<char>>& storage) {
	std::vector<char*> argv;
	argv.reserve(storage.size());
	for (auto& arg : storage) {
		argv.push_back(arg.data());
	}
	return argv;
}

bool Equals_NoCase(const char* lhs, const char* rhs) {
	if (!lhs || !rhs) {
		return false;
	}
	while (*lhs && *rhs) {
		const int left = std::tolower(static_cast<unsigned char>(*lhs));
		const int right = std::tolower(static_cast<unsigned char>(*rhs));
		if (left != right) {
			return false;
		}
		++lhs;
		++rhs;
	}
	return *lhs == *rhs;
}

int MessageBox(HWND, const char* text, const char* caption, unsigned flags) {
	if (caption && *caption) {
		std::fprintf(stderr, "[%s] ", caption);
	}
	if (text) {
		std::fprintf(stderr, "%s\n", text);
	}
	if (flags & MB_YESNO) {
		return IDYES;
	}
	return IDOK;
}

BOOL PostMessage(HWND window, unsigned int message, WPARAM wparam, LPARAM lparam) {
	(void)window;
	(void)message;
	(void)wparam;
	(void)lparam;
	return TRUE;
}

HWND FindWindow(const char* class_name, const char* window_name) {
	(void)class_name;
	(void)window_name;
	return nullptr;
}

BOOL SetForegroundWindow(HWND window) {
	(void)window;
	return TRUE;
}

BOOL ShowWindow(HWND window, unsigned int command) {
	(void)window;
	(void)command;
	return TRUE;
}

void Sleep(unsigned long milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Clear_Pages() {
	if (SeenBuff.Get_Width() > 0 && SeenBuff.Get_Height() > 0) {
		SeenBuff.Fill_Rect(0, 0, SeenBuff.Get_Width() - 1, SeenBuff.Get_Height() - 1, 0);
	}
	if (HidPage.Get_Width() > 0 && HidPage.Get_Height() > 0) {
		HidPage.Fill_Rect(0, 0, HidPage.Get_Width() - 1, HidPage.Get_Height() - 1, 0);
	}
}

}  // namespace


[[maybe_unused]] HINSTANCE ProgramInstance;
extern BOOL CC95AlreadyRunning;
void Move_Point(short& x, short& y, DirType dir, unsigned short distance);

void Check_Use_Compressed_Shapes(void);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE, char* command_line, int command_show) {
	CCDebugString("C&C95 - Starting up.\n");
	ProgramInstance = instance;

	SpawnedFromWChat = false;

	if (CC95AlreadyRunning) {
		HWND ccwindow = FindWindow("Command & Conquer", "Command & Conquer");
		if (ccwindow) {
			SetForegroundWindow(ccwindow);
			ShowWindow(ccwindow, SW_RESTORE);
		}
		return EXIT_SUCCESS;
	}

	if (Ram_Free(MEM_NORMAL) < 5000000) {
		std::fprintf(stderr, "Insufficient RAM available.\n");
		return EXIT_FAILURE;
	}

	const std::string exe_path = Build_Executable_Name(instance);
	std::vector<std::vector<char>> arg_storage = Build_Arg_Storage(exe_path, command_line);
	std::vector<char*> argv = Materialize_Argv(arg_storage);
	const int argc = static_cast<int>(argv.size());

	if (!Parse_Command_Line(argc, argv.data())) {
		std::puts("Run SETUP program first.");
		std::puts("");
		Kbd.Get();
		return EXIT_SUCCESS;
	}

	WindowsTimer = new WinTimerClass(60, FALSE);

	const int time_test = WindowsTimer->Get_System_Tick_Count();
	Sleep(kTimerSleepMs);
	if (WindowsTimer->Get_System_Tick_Count() == time_test) {
		MessageBox(nullptr,
		           "Error - Timer system failed to start due to system instability. You need to restart Windows.",
		           "Command & Conquer", MB_OK | MB_ICONSTOP);
		return EXIT_FAILURE;
	}

	RawFileClass cfile("CONQUER.INI");

	if (Detect_MMX_Availability()) {
		MMXAvailable = true;
	}

	Check_Use_Compressed_Shapes();

	if (Disk_Space_Available() < INIT_FREE_DISK_SPACE) {
		const int reply = MessageBox(nullptr,
		                             "Warning - you are critically low on free disk space for virtual memory and save games. Do you want to play C&C anyway?",
		                             "Command & Conquer", MB_ICONQUESTION | MB_YESNO);
		if (reply == IDNO) {
			delete WindowsTimer;
			WindowsTimer = nullptr;
			return EXIT_FAILURE;
		}
	}

	CDFileClass::Set_CD_Drive(CDList.Get_First_CD_Drive());

	if (cfile.Is_Available()) {
		char* cdata = static_cast<char*>(Load_Alloc_Data(cfile));
		if (cdata) {
			Read_Private_Config_Struct(cdata, &NewConfig);
			delete[] cdata;
		}
		Read_Setup_Options(&cfile);

		CCDebugString("C&C95 - Creating main window.\n");
		Create_Main_Window(instance, command_show, ScreenWidth, ScreenHeight);

		CCDebugString("C&C95 - Initialising audio.\n");
		SoundOn = Audio_Init(MainWindow, 16, false, 11025 * 2, 0);

		Palette = new (MEM_CLEAR) unsigned char[768];

		CCDebugString("C&C95 - Setting video mode.\n");
		bool video_success = false;
		if (ScreenHeight == 400) {
			if (Set_Video_Mode(MainWindow, ScreenWidth, ScreenHeight, 8)) {
				video_success = true;
			} else if (Set_Video_Mode(MainWindow, ScreenWidth, 480, 8)) {
				video_success = true;
				ScreenHeight = 480;
			}
		} else {
			video_success = Set_Video_Mode(MainWindow, ScreenWidth, ScreenHeight, 8);
		}

		if (!video_success) {
			CCDebugString("C&C95 - Failed to set video mode.\n");
			MessageBox(MainWindow, Text_String(TXT_UNABLE_TO_SET_VIDEO_MODE), "Command & Conquer",
			           MB_ICONEXCLAMATION | MB_OK);
			delete WindowsTimer;
			WindowsTimer = nullptr;
			delete[] Palette;
			Palette = nullptr;
			return EXIT_FAILURE;
		}

		if (ScreenHeight == 480) {
			SeenBuff.Configure(SeenBuff.Get_Graphic_Buffer(), 0, 40, ScreenWidth, 400);
			HidPage.Configure(HidPage.Get_Graphic_Buffer(), 0, 40, ScreenWidth, 400);
			ScreenHeight = 400;
		} else {
			SeenBuff.Configure(SeenBuff.Get_Graphic_Buffer(), 0, 0, ScreenWidth, ScreenHeight);
			HidPage.Configure(HidPage.Get_Graphic_Buffer(), 0, 0, ScreenWidth, ScreenHeight);
		}

		CCDebugString("C&C95 - Adjusting variables for resolution.\n");
		Options.Adjust_Variables_For_Resolution();

		WindowList[0][WINDOWWIDTH] = SeenBuff.Get_Width() >> 3;
		WindowList[0][WINDOWHEIGHT] = SeenBuff.Get_Height();

		Memory_Error = &Memory_Error_Handler;

		CCDebugString("C&C95 - Entering MMX detection.\n");
		if (MMXAvailable) {
			Init_MMX();
		}

		CCDebugString("C&C95 - Creating mouse class.\n");
		WWMouse = new WWMouseClass(&SeenBuff, 32, 32);
		MouseInstalled = TRUE;

		CCDebugString("C&C95 - Reading CONQUER.INI.\n");
		char* buffer = static_cast<char*>(Alloc(64000, MEM_NORMAL));
		if (buffer) {
			cfile.Read(buffer, cfile.Size());
			buffer[cfile.Size()] = '\0';

			char tempbuff[5] = {};
			WWGetPrivateProfileString("Intro", "PlayIntro", "Yes", tempbuff, 4, buffer);
			if (Equals_NoCase(tempbuff, "No") || SpawnedFromWChat) {
				Special.IsFromInstall = false;
			} else {
				Special.IsFromInstall = true;
			}
			SlowPalette = WWGetPrivateProfileInt("Options", "SlowPalette", 1, buffer);

			WWWritePrivateProfileString("Intro", "PlayIntro", "No", buffer);
			cfile.Write(buffer, std::strlen(buffer));

			Free(buffer);
		}

		CCDebugString("C&C95 - Checking availability of C&CSPAWN.INI packet from WChat.\n");
		if (DDEServer.Get_MPlayer_Game_Info()) {
			CCDebugString("C&C95 - C&CSPAWN.INI packet available.\n");
			Check_From_WChat(nullptr);
		} else {
			CCDebugString("C&C95 - C&CSPAWN.INI packet not arrived yet.\n");
		}

		if (Special.IsFromInstall) {
			BreakoutAllowed = false;
		}

		Memory_Error_Exit = Print_Error_End_Exit;

		CCDebugString("C&C95 - Entering main game.\n");
		Main_Game(argc, argv.data());

		Clear_Pages();

		Memory_Error_Exit = Print_Error_Exit;

		CCDebugString("C&C95 - About to exit.\n");
		ReadyToQuit = 1;

		PostMessage(MainWindow, WM_DESTROY, 0, 0);
		do {
			Keyboard::Check();
		} while (ReadyToQuit == 1);

		CCDebugString("C&C95 - Returned from final message loop.\n");
		return EXIT_SUCCESS;
	}
	std::puts("Run SETUP program first.");
	std::puts("");
	Kbd.Get();

	if (WindowsTimer) {
		delete WindowsTimer;
		WindowsTimer = nullptr;
	}

	if (Palette) {
		delete[] Palette;
		Palette = nullptr;
	}

	return EXIT_SUCCESS;
}


/***********************************************************************************************
 * Prog_End -- Cleans up library systems in prep for game exit.                                *
 *                                                                                             *
 *    This routine should be called before the game terminates. It handles cleaning up         *
 *    library systems so that a graceful return to the host operating system is achieved.      *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/20/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void __cdecl Prog_End(void) {
#ifndef DEMO
	if (GameToPlay == GAME_MODEM || GameToPlay == GAME_NULL_MODEM) {
		NullModem.Change_IRQ_Priority(0);
	}
#endif
	CCDebugString("C&C95 - About to call Sound_End.\n");
	Sound_End();
	CCDebugString("C&C95 - Returned from Sound_End.\n");
	if (WWMouse) {
		CCDebugString("C&C95 - Deleting mouse object.\n");
		delete WWMouse;
		WWMouse = nullptr;
	}
	if (WindowsTimer) {
		CCDebugString("C&C95 - Deleting windows timer.\n");
		delete WindowsTimer;
		WindowsTimer = nullptr;
	}

	if (Palette) {
		CCDebugString("C&C95 - Deleting palette object.\n");
		delete[] Palette;
		Palette = nullptr;
	}
}


/***********************************************************************************************
 * Delete_Swap_Files -- Deletes previously existing swap files.                                *
 *                                                                                             *
 *    This routine will scan through the current directory and delete any swap files it may    *
 *    find. This is used to clear out any left over swap files from previous runs (crashes)    *
 *    of the game. This routine presumes that it cannot delete the swap file that is created   *
 *    by the current run of the game.                                                          *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   08/27/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void Delete_Swap_Files(void) {
}


void Print_Error_End_Exit(char* string) {
	std::printf("%s\n", string);
	Get_Key();
	Prog_End();
	std::printf("%s\n", string);
	std::exit(1);
}


void Print_Error_Exit(char* string) {
	std::printf("%s\n", string);
	std::exit(1);
}








/***********************************************************************************************
 * Read_Setup_Options -- Read stuff in from the INI file that we need to know sooner           *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    6/7/96 4:09PM ST : Created                                                               *
 *=============================================================================================*/
void Read_Setup_Options(RawFileClass* config_file) {
	const int buffer_size = config_file->Size() + 1;
	char* buffer = new char[buffer_size];

	if (config_file->Is_Available()) {
		config_file->Read(buffer, config_file->Size());
		buffer[config_file->Size()] = '\0';

		VideoBackBufferAllowed = WWGetPrivateProfileInt("Options", "VideoBackBuffer", 1, buffer);
		AllowHardwareBlitFills = WWGetPrivateProfileInt("Options", "HardwareFills", 1, buffer);
		ScreenHeight = WWGetPrivateProfileInt("Options", "Resolution", 0, buffer) ? 480 : 400;
		IsV107 = WWGetPrivateProfileInt("Options", "Compatibility", 0, buffer);

		const int socket = WWGetPrivateProfileInt("Options", "Socket", 0, buffer);
		if (socket > 0) {
			const int expanded_socket = socket + 0x4000;
			if (expanded_socket >= 0x4000 && expanded_socket < 0x8000) {
				Ipx.Set_Socket(expanded_socket);
			}
		}

		char netbuf[512];
		std::memset(netbuf, 0, sizeof(netbuf));
		char* netptr = WWGetPrivateProfileString("Options", "DestNet", nullptr, netbuf, sizeof(netbuf), buffer);

		if (netptr && std::strlen(netbuf)) {
			NetNumType net;
			NetNodeType node;

			int i = 0;
			char* p = std::strtok(netbuf, ".");
			int x;
			while (p) {
				std::sscanf(p, "%x", &x);
				if (i < 4) {
					net[i] = static_cast<char>(x);
				} else {
					node[i - 4] = static_cast<char>(x);
				}
				i++;
				p = std::strtok(nullptr, ".");
			}

			if (i >= 4) {
				IsBridge = 1;
				std::memset(node, 0xff, 6);
				BridgeNet = IPXAddressClass(net, node);
			}
		}
	}

	delete[] buffer;
}
