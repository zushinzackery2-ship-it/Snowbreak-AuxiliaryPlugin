#pragma once
#include <Windows.h>
#include "../../../CppSDK/SDK/Game_classes.hpp"

namespace AttrUtils {
    bool SetAttrByName(SDK::UGameAbilityComponent* ability, const char* name, float value);
    bool GetAttrValueByName(SDK::UGameAbilityComponent* ability, const char* name, float& outValue);
    void SafeWriteCurAttr(SDK::FAbilityAttributeItem* item, float v);
}
