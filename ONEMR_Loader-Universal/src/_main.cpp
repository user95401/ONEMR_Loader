#include "ModUtils.hpp"
#include "HooksUtils.hpp"

#include "imgui-hook.hpp"

using namespace cocos2d;
using namespace gd;

#include <fstream>

int ModsLoaded = 0;
std::string ModsLoadedList = "";
bool afterLoad = false;

void CreateInfoSettingFile(bool AddInfo) {
    std::string fileinfo = (
        "u can delete that file, so message box with confirm appears again" "\n"
        "if exists ONEMR_Loader.NoInfo => dont show loaded mods info in corner" "\n"
        "if exists ONEMR_Loader.AddInfo => dont show confirm message box"
        );
    if (AddInfo) {
        std::remove("ONEMR_Loader.NoInfo");
        std::ofstream("ONEMR_Loader.AddInfo") << fileinfo;
    }
    else {
        std::remove("ONEMR_Loader.AddInfo");
        std::ofstream("ONEMR_Loader.NoInfo") << fileinfo;
    }
}

namespace ImGuiMod {
    bool Hiden;
    void HideUI() {
        Hiden = true;
    };
    namespace ModsLoadedText {
        bool Showing;
        bool Hiden;
        void Show() {
            Hiden = false;
        };
        void Hide() {
            Hiden = true;
        };
        int DelayedHideMs;
        int FadeOutRate;
        ImColor FadeOutCol;
        void FadeOutAnim() {
            /// <summary>
            /// SSSSSSSSSUUUCKKS
            /// </summary>
            //ImGuiStyle* Style = &ImGui::GetStyle();
            for (int io = 255; io > -1; io = io - FadeOutRate) {
                Sleep(1);
                FadeOutCol = ImColor(255, 255, 255, io);
            }
            FadeOutCol = ImColor(255, 255, 255, 0);
        }
        DWORD WINAPI FadeOutInitThread(void* hModule) {
            Sleep(DelayedHideMs);
            FadeOutAnim();
            return S_OK;
        }
        void FadeOut(int Delay, int Rate, void* hModule) {
            DelayedHideMs = Delay;
            FadeOutRate = Rate;
            FadeOutCol = ImColor(255, 255, 255, 255);
            CreateThread(nullptr, 0, FadeOutInitThread, hModule, 0, nullptr);
        };
        void RenderIt() {
            if (Hiden) return;
            //ModsLoadedText key
            if (ModsLoadedText::Showing)
                ModsLoadedText::FadeOutCol = ImColor(255, 255, 254, 255);
            else if (ModsLoadedText::FadeOutCol == ImColor(255, 255, 254, 255))
                ImGuiMod::ModsLoadedText::FadeOut(100, 10, nullptr);
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
            ImGui::Begin("BottomRightText", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
            if(!FadeOutCol) FadeOutCol = ImColor(255, 255, 255, 255);
            ImGui::PushStyleColor(ImGuiCol_Text, FadeOutCol.Value);
            ImGui::Text(std::format("{}: {} libs loaded\n{}", ModUtils::GetModName(), ModsLoaded, ModsLoadedList).c_str());
            ImGui::PopStyleColor();
            ImGui::End();
        };
    }
    namespace ShowInfoDialog {

        bool ShowPopup;
        void Show() {
            ShowPopup = true;
        };

        void RenderIt() {

            if (ShowPopup) {
                ImGui::OpenPopup("Enable showing info?");
                ShowPopup = false;
            }

            ImGuiIO& io = ImGui::GetIO();
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ModalWindowDimBg, ImColor(0,0,0,0).Value);
            ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y), ImGuiCond_Always, ImVec2(-0.02f, 1.4f));
            if (ImGui::BeginPopupModal(
                "Enable showing info?", NULL, 
                ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | 
                ImGuiWindowFlags_::ImGuiWindowFlags_NoMove
            )) {
                std::string info = (
                    "Will creaded file:" "\n"
                    "ONEMR_Loader.AddInfo => don't show confirm message box" "\n"
                    "ONEMR_Loader.NoInfo => don't show loaded mods info in corner"
                    );
                ImGui::Text(info.c_str());

                ImGui::Separator();

                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) ImGui::CloseCurrentPopup();

                if (ImGui::Button("Yes", {80, 19}) || (GetAsyncKeyState(VK_RETURN) & 0x8000)) {
                    CreateInfoSettingFile(true);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("No", { 60, 19 })) {
                    CreateInfoSettingFile(false);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();

                ModsLoadedText::Showing = true;
            }
        };

    }
    void RenderUI() {
        if (Hiden) return;
        ShowInfoDialog::RenderIt();
        ModsLoadedText::RenderIt();
        //ModsLoadedText key
        if ((GetAsyncKeyState(VK_TAB) & 0x8000))
            ModsLoadedText::Showing = true;
        else if (ModsLoadedText::FadeOutCol == ImColor(255, 255, 254, 255))
            ModsLoadedText::Showing = false;
    }
    void Init() {
        //setup
        ImGuiHook::setRenderFunction(RenderUI);
        MH_Initialize();
        ImGuiHook::setupHooks(HooksUtils::CreateHook);
    }
};

DWORD WINAPI LoadMods(void* hModule) {
    //ModsLoadedText
    if (CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo")) ImGuiMod::ModsLoadedText::Hide();
    else ImGuiMod::ModsLoadedText::FadeOut(5000, 10, hModule);
    //ShowInfoDialog
    if (
        !CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.NoInfo")
        &&
        !CCFileUtils::sharedFileUtils()->isFileExist("ONEMR_Loader.AddInfo")
        )
    {
        ImGuiMod::ShowInfoDialog::Show();
    }
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
    return S_OK;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        ImGuiMod::Init();
        LoadMods(hModule);
    }
    return TRUE;
}









