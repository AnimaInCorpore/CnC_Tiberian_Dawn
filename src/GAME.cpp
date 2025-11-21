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
void Check_Use_Compressed_Shapes(void);
int Ram_Free();
int Disk_Space_Available();
void Main_Game(int argc, char* argv[]);
void Prog_End();

bool VideoBackBufferAllowed = true;
void Check_From_WChat(char* wchat_name);
bool SpawnedFromWChat = false;

extern bool ReadyToQuit;

extern "C" {
bool __cdecl Detect_MMX_Availability(void);
void __cdecl Init_MMX(void);
}

namespace {
    void Clear_Pages() {
        if (SeenBuff.Get_Width() > 0 && SeenBuff.Get_Height() > 0) {
            SeenBuff.Fill_Rect(0, 0, SeenBuff.Get_Width() - 1, SeenBuff.Get_Height() - 1, 0);
        }
        if (HidPage.Get_Width() > 0 && HidPage.Get_Height() > 0) {
            HidPage.Fill_Rect(0, 0, HidPage.Get_Width() - 1, HidPage.Get_Height() - 1, 0);
        }
    }
}

void Game_Startup(int argc, char* argv[]) {
	CCDebugString("C&C95 - Starting up.\n");

	if (Ram_Free() < 5000000) {
		std::fprintf(stderr, "Insufficient RAM available.\n");
		return;
	}

	if (!Parse_Command_Line(argc, argv)) {
		std::puts("Run SETUP program first.");
		std::puts("");
		Kbd.Get();
		return;
	}

	RawFileClass cfile("CONQUER.INI");

	if (Detect_MMX_Availability()) {
		MMXAvailable = true;
	}

	Check_Use_Compressed_Shapes();

	if (Disk_Space_Available() < INIT_FREE_DISK_SPACE) {
		const int reply = MessageBox(nullptr,
		                             "Warning - you are critically low on free disk space for virtual memory and save games. Do you want to play C&C anyway?",
		                             "Command & Conquer", 0);
		if (reply == 0) {
			return;
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

		CCDebugString("C&C95 - Initialising audio.\n");
		SoundOn = Audio_Init(nullptr, 16, false, 11025 * 2, 0);

		Palette = new unsigned char[768];

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

		Memory_Error = &Memory_Error_Handler;

		CCDebugString("C&C95 - Entering MMX detection.\n");
		if (MMXAvailable) {
			Init_MMX();
		}

		CCDebugString("C&C95 - Creating mouse class.\n");
		WWMouse = new WWMouseClass(&SeenBuff, 32, 32);
		MouseInstalled = TRUE;

		CCDebugString("C&C95 - Reading CONQUER.INI.\n");
		char* buffer = static_cast<char*>(Alloc(64000, 0));
		if (buffer) {
			cfile.Read(buffer, cfile.Size());
			buffer[cfile.Size()] = '\0';

			char tempbuff[5] = {};
			WWGetPrivateProfileString("Intro", "PlayIntro", "Yes", tempbuff, 4, buffer);
			if (strcmp(tempbuff, "No") == 0 || SpawnedFromWChat) {
				Special.IsFromInstall = false;
			} else {
				Special.IsFromInstall = true;
			}
			SlowPalette = WWGetPrivateProfileInt("Options", "SlowPalette", 1, buffer);

			WWWritePrivateProfileString("Intro", "PlayIntro", "No", buffer);
			cfile.Write(buffer, std::strlen(buffer));

			Free(buffer);
		}

		if (Special.IsFromInstall) {
			BreakoutAllowed = false;
		}

		Memory_Error_Exit = Print_Error_End_Exit;

		CCDebugString("C&C95 - Entering main game.\n");
	}
}

void Game_Shutdown(void) {
    Clear_Pages();
    Memory_Error_Exit = Print_Error_Exit;
    CCDebugString("C&C95 - About to exit.\n");
    ReadyToQuit = 1;
    Prog_End();
}

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