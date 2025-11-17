#include "combat_enhancer.h"
#include "../combat_config.h"
#include "attr_utils.h"
#include <string>

using namespace SDK;

namespace {
    AGameCharacter* g_lastEnhancedGirl = nullptr;
    float g_baseTimeDilation = 1.0f;
    float g_baseFireRes = 0.0f;
    float g_baseIceRes = 0.0f;
    float g_baseThunderRes = 0.0f;
    float g_baseSuperRes = 0.0f;
    float g_baseWeaponShoot = 0.0f;
}

namespace CombatEnhancer {

void UpdateForActor(AActor* actor) {
    if (!actor || !actor->Class) return;
    std::string cls = actor->Class->GetName();
    if (cls.find("BP_Girl") == std::string::npos) return;
    if (!actor->IsA(AGameCharacter::StaticClass())) return;

    UWorld* world = UWorld::GetWorld();
    if (!world) return;

    ACharacter* playerChar = UGameplayStatics::GetPlayerCharacter(world, 0);
    AGameCharacter* controlled = (playerChar && playerChar->IsA(AGameCharacter::StaticClass())) ? static_cast<AGameCharacter*>(playerChar) : nullptr;

    // restore previous when switching controlled character
    if (g_lastEnhancedGirl && g_lastEnhancedGirl != controlled) {
        UAbilityComponent* prevAbilRaw = g_lastEnhancedGirl->Ability;
        UGameAbilityComponent* prevAbility = static_cast<UGameAbilityComponent*>(prevAbilRaw);
        if (prevAbility) {
            AttrUtils::SetAttrByName(prevAbility, "FireResistance", g_baseFireRes);
            AttrUtils::SetAttrByName(prevAbility, "IceResistance", g_baseIceRes);
            AttrUtils::SetAttrByName(prevAbility, "ThunderResistance", g_baseThunderRes);
            AttrUtils::SetAttrByName(prevAbility, "SuperpowersResistance", g_baseSuperRes);
            AttrUtils::SetAttrByName(prevAbility, "WeaponShootSpeedPer", g_baseWeaponShoot);
        }
        g_lastEnhancedGirl->CustomTimeDilation = g_baseTimeDilation;
        g_lastEnhancedGirl = nullptr;
    }

    if (actor != controlled) return;
    if (!(CombatConfig::DefenseEnabled() || CombatConfig::SpeedEnabled() || CombatConfig::FastRecoverEnabled())) return;

    AGameCharacter* gc = static_cast<AGameCharacter*>(actor);
    UAbilityComponent* abilRaw = gc->Ability;
    UGameAbilityComponent* ability = static_cast<UGameAbilityComponent*>(abilRaw);
    if (!ability) return;

    if (g_lastEnhancedGirl != gc) {
        g_baseTimeDilation = gc->CustomTimeDilation;
        float tmp;
        if (AttrUtils::GetAttrValueByName(ability, "FireResistance", tmp)) g_baseFireRes = tmp; else g_baseFireRes = 0.0f;
        if (AttrUtils::GetAttrValueByName(ability, "IceResistance", tmp)) g_baseIceRes = tmp; else g_baseIceRes = 0.0f;
        if (AttrUtils::GetAttrValueByName(ability, "ThunderResistance", tmp)) g_baseThunderRes = tmp; else g_baseThunderRes = 0.0f;
        if (AttrUtils::GetAttrValueByName(ability, "SuperpowersResistance", tmp)) g_baseSuperRes = tmp; else g_baseSuperRes = 0.0f;
        if (AttrUtils::GetAttrValueByName(ability, "WeaponShootSpeedPer", tmp)) g_baseWeaponShoot = tmp; else g_baseWeaponShoot = 0.0f;
        g_lastEnhancedGirl = gc;
    }

    if (CombatConfig::SpeedEnabled()) gc->CustomTimeDilation = CombatConfig::SpeedMultiplier(); else gc->CustomTimeDilation = g_baseTimeDilation;

    float defDiff = CombatConfig::DefenseMultiplier() - 1.0f; if (defDiff < 0.0f) defDiff = 0.0f;
    if (CombatConfig::DefenseEnabled()) {
        AttrUtils::SetAttrByName(ability, "FireResistance", defDiff);
        AttrUtils::SetAttrByName(ability, "IceResistance", defDiff);
        AttrUtils::SetAttrByName(ability, "ThunderResistance", defDiff);
        AttrUtils::SetAttrByName(ability, "SuperpowersResistance", defDiff);
    } else {
        AttrUtils::SetAttrByName(ability, "FireResistance", g_baseFireRes);
        AttrUtils::SetAttrByName(ability, "IceResistance", g_baseIceRes);
        AttrUtils::SetAttrByName(ability, "ThunderResistance", g_baseThunderRes);
        AttrUtils::SetAttrByName(ability, "SuperpowersResistance", g_baseSuperRes);
    }

    float spdDiff = CombatConfig::SpeedMultiplier() - 1.0f; if (spdDiff < 0.0f) spdDiff = 0.0f;
    if (CombatConfig::SpeedEnabled()) AttrUtils::SetAttrByName(ability, "WeaponShootSpeedPer", spdDiff); else AttrUtils::SetAttrByName(ability, "WeaponShootSpeedPer", g_baseWeaponShoot);

    if (CombatConfig::FastRecoverEnabled()) {
        const FAbilityAttributeArray& attrs = ability->AbilityAttributes;
        auto addHealthDirect = [&](const UC::TArray<FAbilityAttributeItem>& arr)->bool{
            if (!arr.IsValid()) return false;
            int32 n = arr.Num();
            for (int32 i = 0; i < n; ++i) {
                if (!arr.IsValidIndex(i)) continue;
                const FAbilityAttributeItem& item = arr[i];
                if (!item.AbilityAttribute) continue;
                const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
                std::string nm = attrObj->GetName();
                if (_stricmp(nm.c_str(), "Health") == 0) {
                    float cur = item.CurrentAttributeValue;
                    float mx = item.MaxValue;
                    if (cur >= mx) return true;
                    float delta = (mx - cur) >= 1.0f ? 1.0f : (mx - cur);
                    float nv = cur + delta;
                    FAbilityAttributeItem* mut = const_cast<FAbilityAttributeItem*>(&item);
                    AttrUtils::SafeWriteCurAttr(mut, nv);
                    return true;
                }
            }
            return false;
        };
        if (!addHealthDirect(attrs.RepAttributes)) addHealthDirect(attrs.NotRepAttributes);
    }
}

void Reset() {
    g_lastEnhancedGirl = nullptr;
    g_baseTimeDilation = 1.0f;
    g_baseFireRes = 0.0f;
    g_baseIceRes = 0.0f;
    g_baseThunderRes = 0.0f;
    g_baseSuperRes = 0.0f;
    g_baseWeaponShoot = 0.0f;
}

} // namespace CombatEnhancer
