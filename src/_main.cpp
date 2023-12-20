#include "ModUtils.hpp"
#include "HooksUtils.hpp"

using namespace cocos2d;
using namespace gd;

int ModsLoaded = 0;
std::string ModsLoadedList = "";
bool afterLoad = false;

DWORD WINAPI LoadMods(void* hModule) {
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
                ++ModsLoaded;
                ModsLoadedList = ModsLoadedList + ((ModsLoadedList == "" ? "" : ", ") + entry.path().filename().string());
            }
        }
    }
    ModUtils::log("Loading libs reached end!");
}

//0x272390
void __fastcall LoadingLayer_loadAssets(CCLayer* self, void* edx) {
    MappedHooks::getOriginal(LoadingLayer_loadAssets)(self, edx);
    if (CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo")) return self->removeChildByTag(938);
    afterLoad = true;
    self->removeChildByTag(938);
    CCLabelTTF* ModsCountLabel = CCLabelTTF::create(std::format(
        "ONEMR_Loader: {} dlls loaded{}", 
        ModsLoaded,
        ModsLoadedList == "" ? "" : ("\n" + ModsLoadedList)
    ).c_str(), "Arial", 12.000f);
    ModsCountLabel->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    ModsCountLabel->setAnchorPoint({-0.01f, -0.1f});
    ModsCountLabel->setScale(0.4f);
    //ModsCountLabel->setOpacity(28);
    ModsCountLabel->runAction(CCFadeTo::create(0.1f, 32));//wow gd 2.2 have do smth with opacity stuff
    self->addChild(ModsCountLabel, 10, 938);/**/
}

//590792
//bool __fastcall MenuLayer_init(CCLayer* self, void* edx) {
//    CCMessageBox(__FUNCTION__, __FUNCTION__);
//    MappedHooks::getOriginal(MenuLayer_init)(self, edx);
//    self->setRotation(12);
//    twoTimesBoolCallEscapeByParrentNode(self);
//    if (!afterLoad) return true;
//    else afterLoad = false;
//    CCLabelTTF* ModsCountLabel = CCLabelTTF::create(std::format("ONEMR_Loader: {} dlls loaded", ModsLoaded).c_str(), "Arial", 6.000f);
//    ModsCountLabel->setAnchorPoint(CCPointZero);
//    //ModsCountLabel->setOpacity(32);
//    ModsCountLabel->runAction(CCFadeTo::create(5.0f, 0));
//    self->addChild(ModsCountLabel, 10, 938);
//    return true;
//}

DWORD WINAPI PROCESS_ATTACH(void* hModule) {
    MH_Initialize();
    MappedHooks::registerHook(base + 2565008, LoadingLayer_loadAssets);
    //MappedHooks::registerHook(base + 0x276700, MenuLayer_init);
    LoadMods(hModule);
    if (!CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo") && !CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.AddInfo")) {
        if (MessageBoxExA(nullptr, "Enable showing info?", "ONEMR_Loader", MB_ICONQUESTION | MB_YESNO, LANG_ENGLISH) == IDYES) {
            std::remove("ONEMR_Loader.NoInfo");
            std::ofstream("ONEMR_Loader.AddInfo");
        }
        else {
            std::ofstream("ONEMR_Loader.NoInfo");
            std::remove("ONEMR_Loader.AddInfo");
        }
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0, PROCESS_ATTACH, hModule, 0, 0);
    }
    return TRUE;
}
