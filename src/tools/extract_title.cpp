#include "legacy/mixfile.h"
#include "legacy/cdfile.h"
#include <fstream>
#include <iostream>

int main() {
    const char* cd_subfolder = CDFileClass::Get_CD_Subfolder();

    auto register_mix = [cd_subfolder](const char* filename) {
        if (!filename) return;
        // Construct candidate paths similarly to Load_Title_Screen
        std::vector<std::string> paths;
        auto add = [&](const std::string& p){ paths.push_back(p); };
        add(std::string(filename));
        add(std::string("CD/") + filename);
        add(std::string("CD/CNC95/") + filename);
        add(std::string("CD/TIBERIAN_DAWN/CD1/") + filename);
        add(std::string("CD/TIBERIAN_DAWN/CD2/") + filename);
        add(std::string("CD/TIBERIAN_DAWN/CD3/") + filename);
        for (auto const& p : paths) {
            std::ifstream f(p, std::ios::binary);
            if (f) {
                MixFileClass* m = new MixFileClass(p.c_str());
                (void)m;
                std::cout << "registered mix " << p << "\n";
            }
        }
    };

    register_mix("GENERAL.MIX");
    register_mix("CONQUER.MIX");
    register_mix("CCLOCAL.MIX");
    register_mix("LOCAL.MIX");

    const char* target_names[] = {"HTITLE.PCX", "TITLE.PCX", "TITLE.CPS"};
    for (auto name : target_names) {
        void* realptr = nullptr;
        MixFileClass* mix = nullptr;
        long offset = 0, size = 0;
        if (MixFileClass::Offset(name, &realptr, &mix, &offset, &size) && realptr) {
            std::string outname = std::string("extracted_") + name;
            std::ofstream out(outname, std::ios::binary);
            out.write(reinterpret_cast<char*>(realptr), size);
            out.close();
            std::cout << "Wrote " << outname << " (" << size << " bytes) from "
                      << (mix && mix->Filename ? mix->Filename : "?") << "\n";
            return 0;
        }
    }

    std::cerr << "Could not locate title art in registered mixes or on disk." << std::endl;
    return 1;
}
