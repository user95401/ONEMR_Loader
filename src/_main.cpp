#include "mod_utils.hpp"
#include "hooks.hpp"

using namespace cocos2d;

int ModsLoaded = 0;

void(__thiscall* LoadingLayer_loadAssets)(gd::LoadingLayer*);
void __fastcall LoadingLayer_loadAssets_H(gd::LoadingLayer* self, void*) {
    self->removeChildByTag(938);
    CCLabelTTF* ModsCountLabel = CCLabelTTF::create(("ONEMR_Loader: " + std::to_string(ModsLoaded) + " dlls loaded").c_str(), "arial", 6.000f);
    ModsCountLabel->setAnchorPoint(CCPointZero);
    ModsCountLabel->setOpacity(35);
    self->addChild(ModsCountLabel, 10, 938);
    LoadingLayer_loadAssets(self);
}

DWORD WINAPI PROCESS_ATTACH(void* hModule) {
    ModUtils::log("Crawling...");
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path())) {
        //ModUtils::log(entry.path().extension().string()); .dll
        bool loadit = false;
        if (entry.path().extension().string() == std::string(".dll")) loadit = true;//is dll
        if (strstr(entry.path().string().c_str(), "geode")) loadit = false;//in geode/unzipped
        if (strstr(entry.path().string().c_str(), "minhook")) loadit = false;//minhook.
        if (strstr(entry.path().string().c_str(), "MinHook")) loadit = false;//MinHook.
        if (GetModuleHandleA(entry.path().string().c_str())) loadit = false;
        if (loadit) {
            Sleep(10);
            HMODULE hModule = LoadLibraryA(entry.path().string().c_str());
            if (!hModule) ModUtils::log("Failed to load library \"" + entry.path().relative_path().string() + "\" .");
            else {
                ModUtils::log("Loaded library \"" + entry.path().relative_path().string() + "\"!");
                ModsLoaded++;
            }
        }
    }
    ModUtils::log("Loading libs reached end!");
    MH_SafeInitialize();
    HOOK(gd::base + 0x18C8E0, LoadingLayer_loadAssets);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, PROCESS_ATTACH, hModule, 0, 0);
    return TRUE;
}
