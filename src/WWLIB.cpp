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

void Game_Startup(int argc, char* argv[]);
void Game_Shutdown(void);
void Main_Game(int argc, char* argv[]);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE, char* command_line, int command_show) {
    std::vector<std::vector<char>> arg_storage;
    arg_storage.reserve(20);
    std::string exe_path = "CONQUER.EXE";
	arg_storage.emplace_back(exe_path.begin(), exe_path.end());
	arg_storage.back().push_back('\0');

	if (command_line) {
        const char* scan = command_line;
        while (*scan && arg_storage.size() < 20) {
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
            arg_storage.emplace_back(start, scan);
            arg_storage.back().push_back('\0');
            if (*scan == '\0' || *scan == '\r') {
                break;
            }
            ++scan;
        }
    }

    std::vector<char*> argv;
    argv.reserve(arg_storage.size());
    for (auto& arg : arg_storage) {
        argv.push_back(arg.data());
    }
    const int argc = static_cast<int>(argv.size());

    Game_Startup(argc, argv.data());
    Main_Game(argc, argv.data());
    Game_Shutdown();
    return 0;
}