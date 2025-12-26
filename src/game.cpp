#include "legacy/function.h"
#include "legacy/ccdde.h"
#include "legacy/windows_compat.h"
#include "legacy/wwlib32.h"
#include "legacy/msgbox.h"
#include "legacy/msgbox.h"
#include "legacy/audio.h"
#include "legacy/wwalloc.h"
#include "legacy/error.h"
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
#include <fstream>
#include <filesystem>
#include <limits>
#include <string>
#include <thread>
#include <vector>
#include <unordered_set>

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
        std::vector<std::filesystem::path> roots;
        if (const char* subfolder = CDFileClass::Get_CD_Subfolder()) {
            roots.emplace_back(std::filesystem::path("CD") / subfolder);
        }
        roots.emplace_back(std::filesystem::path("CD") / "CNC95");
        static const char* kDiscs[] = {"CD1", "CD2", "CD3"};
        for (auto disc : kDiscs) {
            roots.emplace_back(std::filesystem::path("CD") / "TIBERIAN_DAWN" / disc);
        }
        for (auto const& root : roots) {
            std::filesystem::path candidate = root / filename;
            if (std::filesystem::exists(candidate)) {
                return candidate;
            }
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

    std::vector<std::filesystem::path> Discover_Mix_Files() {
        static bool scanned = false;
        static std::vector<std::filesystem::path> cached;
        if (scanned) {
            return cached;
        }
        scanned = true;

        const char* cd_subfolder = CDFileClass::Get_CD_Subfolder();
        std::vector<std::filesystem::path> roots;
        roots.emplace_back(".");
        roots.emplace_back(std::filesystem::path("CD") / "CNC95");
        if (cd_subfolder && *cd_subfolder) {
            roots.emplace_back(std::filesystem::path("CD") / cd_subfolder);
        }
        static const char* kTiberianFolders[] = {"CD1", "CD2", "CD3"};
        for (auto folder : kTiberianFolders) {
            roots.emplace_back(std::filesystem::path("CD") / "TIBERIAN_DAWN" / folder);
        }

        static const char* kAllowedMixes[] = {"GENERAL.MIX", "CONQUER.MIX", "CCLOCAL.MIX",
                                              "LOCAL.MIX",   "UPDATE.MIX",  "UPDATEC.MIX",
                                              "UPDATA.MIX",  "LANGUAGE.MIX"};
        auto is_allowed = [&](const std::string& filename) {
            for (auto allowed : kAllowedMixes) {
                if (filename == allowed) return true;
            }
            return false;
        };

        std::unordered_set<std::string> seen;
        auto add_if_mix = [&](const std::filesystem::path& path) {
            if (!std::filesystem::is_regular_file(path)) return;
            const std::string ext = path.extension().string();
            if (ext == ".MIX" || ext == ".mix") {
                std::string filename = path.filename().string();
                std::transform(filename.begin(), filename.end(), filename.begin(),
                               [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
                if (is_allowed(filename) && seen.insert(filename).second) {
                    cached.push_back(path);
                }
            }
        };

        for (auto const& root : roots) {
            if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) continue;
            for (auto const& entry : std::filesystem::directory_iterator(root)) {
                add_if_mix(entry.path());
            }
        }

        return cached;
    }

    struct FontCandidate {
        std::vector<unsigned char> data;
        int height = 0;
        int width = 0;
    };

    bool Parse_Font_Header(const std::vector<unsigned char>& blob, FontCandidate& out) {
        if (blob.size() < 32) return false;

#pragma pack(push, 1)
        struct Header {
            std::uint16_t length;
            std::uint8_t compress;
            std::uint8_t data_blocks;
            std::uint16_t info_offset;
            std::uint16_t offset_offset;
            std::uint16_t width_offset;
            std::uint16_t data_offset;
            std::uint16_t height_offset;
        };
#pragma pack(pop)

        Header header{};
        std::memcpy(&header, blob.data(), sizeof(header));

        const std::uint16_t offsets[] = {header.info_offset, header.offset_offset, header.width_offset,
                                         header.data_offset, header.height_offset, header.length};
        auto monotonic = [](const std::uint16_t* arr, std::size_t n) {
            for (std::size_t i = 1; i < n; ++i) {
                if (arr[i] <= arr[i - 1]) return false;
            }
            return true;
        };
        if (!monotonic(offsets, 5)) return false;
        for (std::size_t i = 0; i < 5; ++i) {
            if (offsets[i] == 0 || offsets[i] >= blob.size()) {
                return false;
            }
        }

        const unsigned char* info_block = blob.data() + header.info_offset;
        if (header.info_offset + 6 > blob.size()) return false;
        const int max_height = info_block[4];
        const int max_width = info_block[5];
        if (max_height <= 0 || max_width <= 0) return false;

        out.data = blob;
        out.height = max_height;
        out.width = max_width;
        return true;
    }

    const void* Load_Font_By_Height(int expected_height, int expected_width) {
        FontCandidate best{};
        int best_height_diff = std::numeric_limits<int>::max();
        int best_width_diff = std::numeric_limits<int>::max();

        for (auto const& mix_path : Discover_Mix_Files()) {
            std::ifstream file(mix_path, std::ios::binary);
            if (!file) continue;

            std::uint16_t count = 0;
            std::uint32_t size = 0;
            file.read(reinterpret_cast<char*>(&count), sizeof(count));
            file.read(reinterpret_cast<char*>(&size), sizeof(size));
            if (!file || size == 0) continue;

            struct SubBlock {
                std::uint32_t crc;
                std::uint32_t offset;
                std::uint32_t length;
            };

            std::vector<SubBlock> blocks(count);
            file.read(reinterpret_cast<char*>(blocks.data()), static_cast<std::streamsize>(blocks.size() * sizeof(SubBlock)));
            const std::streamoff base = static_cast<std::streamoff>(6 + blocks.size() * sizeof(SubBlock));

            for (auto const& block : blocks) {
                if (block.length == 0 || block.offset + block.length > size) continue;
                std::vector<unsigned char> blob(static_cast<std::size_t>(block.length));
                file.seekg(base + static_cast<std::streamoff>(block.offset), std::ios::beg);
                file.read(reinterpret_cast<char*>(blob.data()), static_cast<std::streamsize>(blob.size()));
                if (!file) break;

                FontCandidate candidate{};
                if (!Parse_Font_Header(blob, candidate)) continue;

                const int height_diff = expected_height > 0 ? std::abs(candidate.height - expected_height) : 0;
                const int width_diff = expected_width > 0 ? std::abs(candidate.width - expected_width) : 0;
                if (height_diff < best_height_diff ||
                    (height_diff == best_height_diff && width_diff < best_width_diff) ||
                    (height_diff == best_height_diff && width_diff == best_width_diff &&
                     candidate.data.size() > best.data.size())) {
                    best = std::move(candidate);
                    best_height_diff = height_diff;
                    best_width_diff = width_diff;
                }
            }
        }

        if (best.data.empty()) {
            return nullptr;
        }

        auto* buffer = new unsigned char[best.data.size()];
        std::memcpy(buffer, best.data.data(), best.data.size());
        return buffer;
    }

    const void* Load_Font_File(const char* filename, int expected_height, int expected_width) {
        CCFileClass file(filename);
        if (file.Is_Available()) {
            if (void* data = Load_Alloc_Data(file)) {
                return data;
            }
        }
        return Load_Font_By_Height(expected_height, expected_width);
    }

    void Initialize_Font_Resources() {
        // Ensure the mix archives that contain fonts are registered.
        Register_Mix_If_Present("GENERAL.MIX");
        Register_Mix_If_Present("CONQUER.MIX");
        Register_Mix_If_Present("CCLOCAL.MIX");
        Register_Mix_If_Present("LOCAL.MIX");
        Register_Mix_If_Present("UPDATE.MIX");
        Register_Mix_If_Present("UPDATEC.MIX");
        Register_Mix_If_Present("UPDATA.MIX");
        Register_Mix_If_Present("LANGUAGE.MIX");

        Green12FontPtr = Load_Font_File("12GREEN.FNT", 16, 16);
        Green12GradFontPtr = Load_Font_File("12GRNGRD.FNT", 16, 16);
        MapFontPtr = Load_Font_File("8FAT.FNT", 16, 16);
        Font8Ptr = Load_Font_File(FONT8, 10, 10);
        FontPtr = Font8Ptr ? Font8Ptr : nullptr;
        Font3Ptr = Load_Font_File(FONT3, 4, 5);
        Font6Ptr = Load_Font_File(FONT6, 6, 6);
        FontLEDPtr = Load_Font_File("LED.FNT", 11, 10);
        VCRFontPtr = Load_Font_File("VCR.FNT", 16, 16);
        GradFont6Ptr = Load_Font_File("GRAD6FNT.FNT", 11, 10);
        ScoreFontPtr = Green12GradFontPtr ? Green12GradFontPtr : Load_Font_File("12GRNGRD.FNT", 16, 16);
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

	if (Palette) {
		CCDebugString("C&C95 - Deleting palette object.\n");
		delete[] Palette;
		Palette = nullptr;
	}
}
