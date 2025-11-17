#pragma once
#include <windows.h>
#include <string>
#include "../../CppSDK/SDK/Engine_classes.hpp"
#include "../../CppSDK/SDK/Game_classes.hpp"

class CombatConfig {
private:
    static bool s_damageEnabled;
    static float s_damageMultiplier;
    static bool s_defenseEnabled;
    static float s_defenseMultiplier;
    static bool s_speedEnabled;
    static float s_speedMultiplier;
    static bool s_fastRecoverEnabled;
    static int  s_f8ActionMode; // 0: direct win(solo), 1: add coop point(multi)

public:
    static void Initialize();

    static bool DamageEnabled() { return s_damageEnabled; }
    static bool* GetDamageEnabledPtr() { return &s_damageEnabled; }
    static void SetDamageEnabled(bool v) { s_damageEnabled = v; }

    static float DamageMultiplier() { return s_damageMultiplier; }
    static float* GetDamageMultiplierPtr() { return &s_damageMultiplier; }
    static void SetDamageMultiplier(float v) { s_damageMultiplier = v; }

    static bool DefenseEnabled() { return s_defenseEnabled; }
    static bool* GetDefenseEnabledPtr() { return &s_defenseEnabled; }
    static void SetDefenseEnabled(bool v) { s_defenseEnabled = v; }

    static float DefenseMultiplier() { return s_defenseMultiplier; }
    static float* GetDefenseMultiplierPtr() { return &s_defenseMultiplier; }
    static void SetDefenseMultiplier(float v) { s_defenseMultiplier = v; }

    static bool SpeedEnabled() { return s_speedEnabled; }
    static bool* GetSpeedEnabledPtr() { return &s_speedEnabled; }
    static void SetSpeedEnabled(bool v) { s_speedEnabled = v; }

    static float SpeedMultiplier() { return s_speedMultiplier; }
    static float* GetSpeedMultiplierPtr() { return &s_speedMultiplier; }
    static void SetSpeedMultiplier(float v) { s_speedMultiplier = v; }

    static void ApplyToBPGirl(SDK::AActor* actor);
    static void TriggerDirectWin();

    // F8 Action selector (0: solo win, 1: coop point)
    static int  GetF8ActionMode() { return s_f8ActionMode; }
    static void SetF8ActionMode(int m) { s_f8ActionMode = (m == 1 ? 1 : 0); }
    static void TriggerF8Action();
    static void TriggerCoopPoint();

    static bool FastRecoverEnabled() { return s_fastRecoverEnabled; }
    static void SetFastRecoverEnabled(bool v) { s_fastRecoverEnabled = v; }
    static bool* GetFastRecoverEnabledPtr() { return &s_fastRecoverEnabled; }
};
