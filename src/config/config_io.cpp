#include "config_io.h"
#include "esp_config.h"
#include "../aimbot/aimbot_config.h"
#include "combat_config.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <ctime>

namespace ConfigIO {
    static std::string g_path;
    static std::string g_lastSnapshot;
    static DWORD g_lastSaveTick = 0;

    static std::string BoolToStr(bool v) { return v ? "1" : "0"; }
    static bool StrToBool(const std::string& s) { return (s == "1" || s == "true" || s == "TRUE"); }

    static std::string MakeSnapshot() {
        std::ostringstream oss;
        // ESP
        oss << "[ESP]\n";
        oss << "Enabled=" << BoolToStr(ESPConfig::IsEnabled()) << "\n";
        oss << "ShowNames=" << BoolToStr(ESPConfig::ShowNames()) << "\n";
        oss << "ShowClassName=" << BoolToStr(ESPConfig::ShowClassName()) << "\n";
        oss << "ShowDistance=" << BoolToStr(ESPConfig::ShowDistance()) << "\n";
        oss << "ShowSkeleton=" << BoolToStr(ESPConfig::ShowSkeleton()) << "\n";
        oss << "SkeletonFallback=" << BoolToStr(ESPConfig::SkeletonFallback()) << "\n";
        oss << "MaxDistance=" << ESPConfig::GetMaxDistance() << "\n";
        ImVec4 tc = ESPConfig::GetTextColor();
        oss << "TextColor=" << tc.x << "," << tc.y << "," << tc.z << "," << tc.w << "\n";

        // Aimbot
        oss << "[AIMBOT]\n";
        oss << "Enabled=" << BoolToStr(AimbotConfig::IsEnabled()) << "\n";
        oss << "VisibleOnly=" << BoolToStr(AimbotConfig::VisibleOnly()) << "\n";
        oss << "FovRadius=" << AimbotConfig::GetFovRadius() << "\n";
        oss << "Smoothing=" << AimbotConfig::GetSmoothing() << "\n";
        oss << "AimKey=" << AimbotConfig::GetAimKey() << "\n";
        oss << "TargetMonsters=" << BoolToStr(AimbotConfig::TargetMonsters()) << "\n";
        oss << "TargetBoss=" << BoolToStr(AimbotConfig::TargetBoss()) << "\n";
        ImVec4 fc = AimbotConfig::GetFovColor();
        oss << "FovColor=" << fc.x << "," << fc.y << "," << fc.z << "," << fc.w << "\n";
        
        // COMBAT
        oss << "[COMBAT]\n";
        oss << "DamageEnabled=" << BoolToStr(CombatConfig::DamageEnabled()) << "\n";
        oss << "DamageMul=" << CombatConfig::DamageMultiplier() << "\n";
        oss << "DefenseEnabled=" << BoolToStr(CombatConfig::DefenseEnabled()) << "\n";
        oss << "DefenseMul=" << CombatConfig::DefenseMultiplier() << "\n";
        oss << "SpeedEnabled=" << BoolToStr(CombatConfig::SpeedEnabled()) << "\n";
        oss << "SpeedMul=" << CombatConfig::SpeedMultiplier() << "\n";
        oss << "FastRecoverEnabled=" << BoolToStr(CombatConfig::FastRecoverEnabled()) << "\n";
        oss << "F8ActionMode=" << CombatConfig::GetF8ActionMode() << "\n";
        return oss.str();
    }

    static void EnsurePath() {
        if (!g_path.empty()) return;
        char exePath[MAX_PATH] = {0};
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        std::string p = exePath;
        size_t pos = p.find_last_of("/\\");
        std::string dir = (pos == std::string::npos) ? p : p.substr(0, pos);
        g_path = dir + "\\Dri.ini";
    }

    const char* GetConfigPath() {
        EnsurePath();
        return g_path.c_str();
    }

    void SaveAll() {
        EnsurePath();
        std::ofstream ofs(g_path, std::ios::trunc);
        if (!ofs.is_open()) return;
        g_lastSnapshot = MakeSnapshot();
        ofs << g_lastSnapshot;
        ofs.close();
        g_lastSaveTick = GetTickCount();
    }

    static void ParseVec4(const std::string& s, ImVec4& out) {
        float a=0,b=0,c=0,d=0;
        if (std::sscanf(s.c_str(), "%f,%f,%f,%f", &a,&b,&c,&d) == 4) {
            out = ImVec4(a,b,c,d);
        }
    }

    void LoadAll() {
        EnsurePath();
        std::ifstream ifs(g_path);
        if (!ifs.is_open()) return;
        std::string line;
        std::string section;
        while (std::getline(ifs, line)) {
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            if (line.front()=='[' && line.back()==']') { section = line; continue; }
            size_t eq = line.find('=');
            if (eq==std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq+1);
            if (section=="[ESP]") {
                if (key=="Enabled") ESPConfig::SetEnabled(StrToBool(val));
                else if (key=="ShowNames") ESPConfig::SetShowNames(StrToBool(val));
                else if (key=="ShowClassName") ESPConfig::SetShowClassName(StrToBool(val));
                else if (key=="ShowDistance") ESPConfig::SetShowDistance(StrToBool(val));
                else if (key=="ShowSkeleton") ESPConfig::SetShowSkeleton(StrToBool(val));
                else if (key=="SkeletonFallback") ESPConfig::SetSkeletonFallback(StrToBool(val));
                else if (key=="MaxDistance") ESPConfig::SetMaxDistance(static_cast<float>(atof(val.c_str())));
                else if (key=="TextColor") { ImVec4 c; ParseVec4(val, c); ESPConfig::SetTextColor(c);}            
            } else if (section=="[AIMBOT]") {
                if (key=="Enabled") AimbotConfig::SetEnabled(StrToBool(val));
                else if (key=="VisibleOnly") AimbotConfig::SetVisibleOnly(StrToBool(val));
                else if (key=="FovRadius") AimbotConfig::SetFovRadius(static_cast<float>(atof(val.c_str())));
                else if (key=="Smoothing") AimbotConfig::SetSmoothing(static_cast<float>(atof(val.c_str())));
                else if (key=="AimKey") AimbotConfig::SetAimKey(atoi(val.c_str()));
                else if (key=="TargetMonsters") AimbotConfig::SetTargetMonsters(StrToBool(val));
                else if (key=="TargetBoss") AimbotConfig::SetTargetBoss(StrToBool(val));
                else if (key=="FovColor") { ImVec4 c; ParseVec4(val, c); AimbotConfig::SetFovColor(c);}            
            } else if (section=="[COMBAT]") {
                if (key=="DamageEnabled") CombatConfig::SetDamageEnabled(StrToBool(val));
                else if (key=="DamageMul") CombatConfig::SetDamageMultiplier(static_cast<float>(atof(val.c_str())));
                else if (key=="DefenseEnabled") CombatConfig::SetDefenseEnabled(StrToBool(val));
                else if (key=="DefenseMul") CombatConfig::SetDefenseMultiplier(static_cast<float>(atof(val.c_str())));
                else if (key=="SpeedEnabled") CombatConfig::SetSpeedEnabled(StrToBool(val));
                else if (key=="SpeedMul") CombatConfig::SetSpeedMultiplier(static_cast<float>(atof(val.c_str())));
                else if (key=="FastRecoverEnabled") CombatConfig::SetFastRecoverEnabled(StrToBool(val));
                else if (key=="F8ActionMode") CombatConfig::SetF8ActionMode(atoi(val.c_str()));
            }
        }
        ifs.close();
    }

    void Initialize() {
        EnsurePath();
        LoadAll();
        g_lastSnapshot = MakeSnapshot();
        g_lastSaveTick = GetTickCount();
    }

    void MarkDirty() {
        // not needed with snapshot strategy
    }

    void SaveIfNeeded() {
        std::string now = MakeSnapshot();
        if (now != g_lastSnapshot) {
            DWORD nowTick = GetTickCount();
            if (nowTick - g_lastSaveTick > 300) { // throttle 300ms
                SaveAll();
            }
        }
    }
}
