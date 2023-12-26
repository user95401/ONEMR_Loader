#include "ModUtils.hpp"
#include "HooksUtils.hpp"
#include<fstream>

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
        if (strstr(entry.path().string().c_str(), "minhook")) loadit = false;//minhook
        if (strstr(entry.path().string().c_str(), "MinHook")) loadit = false;//MinHook
        if (strstr(entry.path().string().c_str(), "websockets.dll")) loadit = false;//websockets.dll
        if (strstr(entry.path().string().c_str(), "msvcp")) loadit = false;//msvcp
        if (strstr(entry.path().string().c_str(), "msvcr")) loadit = false;//msvcr
        if (GetModuleHandleA(entry.path().string().c_str())) loadit = false;
        if (loadit) {
            //Sleep(10);justwhy
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
    afterLoad = true;
    MappedHooks::getOriginal(LoadingLayer_loadAssets)(self, edx);
    if (!CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo") && !CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.AddInfo")) {
        std::string fileinfo = (
            "u can delete that file, so message box with confirm appears again" "\n"
            "if exists ONEMR_Loader.NoInfo => dont show loaded mods info in corner" "\n"
            "if exists ONEMR_Loader.AddInfo => dont show confirm message box"
            );
        if (MessageBoxExA(nullptr, "Enable showing info?", "ONEMR_Loader", MB_ICONQUESTION | MB_YESNO, LANG_ENGLISH) == IDYES) {
            std::remove("ONEMR_Loader.NoInfo");
            std::ofstream("ONEMR_Loader.AddInfo") << fileinfo;
        }
        else {
            std::ofstream("ONEMR_Loader.NoInfo") << fileinfo;
            std::remove("ONEMR_Loader.AddInfo");
        }
    }
    if (CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo")) return self->removeChildByTag(938);
    self->removeChildByTag(938);
    CCLabelTTF* ModsCountLabel = CCLabelTTF::create(std::format(
        "ONEMR_Loader: {} dlls loaded{}", 
        ModsLoaded,
        ModsLoadedList == "" ? "" : ("\n" + ModsLoadedList)
    ).c_str(), "Arial", 12.000f);
    ModsCountLabel->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    ModsCountLabel->setAnchorPoint({-0.01f, -0.1f});
    ModsCountLabel->setScale(0.4f);
    ModsCountLabel->setOpacity(68);
    ModsCountLabel->setBlendFunc({ GL_SRC_ALPHA, GL_ONE }/*that is additive blend*/);
    self->addChild(ModsCountLabel, 10, 938);/**/
}

//0x276700
bool __fastcall MenuLayer_init(CCLayer* self, void* edx) {
    MappedHooks::getOriginal(MenuLayer_init)(self, edx);
    //afterLoad
    if (!afterLoad) return true;
    afterLoad = false;
    //ONEMR_Loader.NoInfo
    if (CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo")) return true;
    //geode thook origin call bug
    twoTimesBoolCallEscapeByParrentNode(self);
    //add label
    CCLabelTTF* ModsCountLabel = CCLabelTTF::create(std::format(
        "ONEMR_Loader: {} dlls loaded{}",
        ModsLoaded,
        ModsLoadedList == "" ? "" : ("\n" + ModsLoadedList)
    ).c_str(), "Arial", 12.000f);
    ModsCountLabel->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    ModsCountLabel->setAnchorPoint({ -0.01f, -0.1f });
    ModsCountLabel->setScale(0.4f);
    ModsCountLabel->setOpacity(68);
    ModsCountLabel->setBlendFunc({ GL_SRC_ALPHA, GL_ONE }/*that is additive blend*/);
    ModsCountLabel->runAction(CCEaseExponentialIn::create(CCFadeTo::create(3.0f, 0)));
    self->addChild(ModsCountLabel, 10, 938);
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        LoadMods(hModule);
        MH_Initialize();
        MappedHooks::registerHook(base + 2565008, LoadingLayer_loadAssets);
        MappedHooks::registerHook(base + 0x276700, MenuLayer_init);
    }
    return TRUE;
}
