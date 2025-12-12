#include "legacy/function.h"
#include "legacy/ccdde.h"
#include "legacy/windows_compat.h"
#include "legacy/wwlib32.h"
#include "legacy/msgbox.h"
#include "legacy/msgbox.h"
#include "legacy/audio_stub.h"
#include "legacy/wwalloc.h"
#include "legacy/error_stub.h"
#include "legacy/externs.h"
#include "legacy/mixfile.h"
#include "legacy/ccfile.h"
#include "legacy/defines.h"

#include <array>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

bool Read_Private_Config_Struct(char* profile, NewConfigType* config);
void Delete_Swap_Files(void);

void Read_Setup_Options(RawFileClass* config_file);
void Check_Use_Compressed_Shapes(void);
int Ram_Free();
unsigned long Disk_Space_Available();
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
    std::filesystem::path Resolve_Data_Path(const char* filename) {
        if (!filename) return {};
        std::filesystem::path direct(filename);
        if (std::filesystem::exists(direct)) {
            return direct;
        }
        std::filesystem::path cd_path = std::filesystem::path("CD") / "CNC95" / filename;
        if (std::filesystem::exists(cd_path)) {
            return cd_path;
        }
        return {};
    }

    void Register_Mix_If_Present(const char* filename) {
        auto path = Resolve_Data_Path(filename);
        if (path.empty()) return;
        // Constructing the MixFileClass registers it for later lookups.
        auto* mix = new MixFileClass(path.string().c_str());
        if (mix) {
            mix->Cache();
        }
    }

    const void* Load_Font_File(const char* filename) {
        CCFileClass file(filename);
        if (!file.Is_Available()) {
            return nullptr;
        }
        return Load_Alloc_Data(file);
    }

    void Initialize_Font_Resources() {
        // Ensure the mix archives that contain fonts are registered.
        Register_Mix_If_Present("GENERAL.MIX");
        Register_Mix_If_Present("CONQUER.MIX");
        Register_Mix_If_Present("CCLOCAL.MIX");
        Register_Mix_If_Present("UPDATE.MIX");
        Register_Mix_If_Present("UPDATEC.MIX");
        Register_Mix_If_Present("LANGUAGE.MIX");

        Green12FontPtr = Load_Font_File("12GREEN.FNT");
        Green12GradFontPtr = Load_Font_File("12GRNGRD.FNT");
        MapFontPtr = Load_Font_File("8FAT.FNT");
        Font8Ptr = Load_Font_File(FONT8);
        FontPtr = Font8Ptr ? Font8Ptr : nullptr;
        Font3Ptr = Load_Font_File(FONT3);
        Font6Ptr = Load_Font_File(FONT6);
        FontLEDPtr = Load_Font_File("LED.FNT");
        VCRFontPtr = Load_Font_File("VCR.FNT");
        GradFont6Ptr = Load_Font_File("GRAD6FNT.FNT");
        ScoreFontPtr = Green12GradFontPtr ? Green12GradFontPtr : Load_Font_File("12GRNGRD.FNT");
    }

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
		const int reply = CCMessageBox().Process("Warning - you are critically low on free disk space for virtual memory and save games. Do you want to play C&C anyway?", "Yes", "No");
		if (reply != 0) { // If "No" is chosen (meaning reply is not 0), then return. Assuming "Yes" is 0.
			return;
		}
	}

	CDFileClass::Set_CD_Drive(CDList.Get_First_CD_Drive());
    Initialize_Font_Resources();

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
		char* buffer = static_cast<char*>(Alloc(64000, MEM_NORMAL));
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
