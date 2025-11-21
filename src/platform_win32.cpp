#include "legacy/windows_compat.h"
#include "legacy/function.h"
#include "legacy/ccdde.h"
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

// Forward declarations
void Game_Startup(HINSTANCE instance, int command_show, int screen_width, int screen_height, bool windowed);
void Game_Shutdown();
void Main_Game(int argc, char* argv[]);

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

}

int PASCAL WinMain(HINSTANCE instance, HINSTANCE, char* command_line, int command_show) {
    const std::string exe_path = Build_Executable_Name(instance);
    std::vector<std::vector<char>> arg_storage = Build_Arg_Storage(exe_path, command_line);
    std::vector<char*> argv = Materialize_Argv(arg_storage);
    const int argc = static_cast<int>(argv.size());

    Game_Startup(instance, command_show, 640, 480, true);
    Main_Game(argc, argv.data());
    Game_Shutdown();

    return 0;
}
