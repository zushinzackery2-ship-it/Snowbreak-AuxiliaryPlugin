#include "attr_utils.h"
#include <string>

using namespace SDK;

namespace AttrUtils {
    bool SetAttrByName(UGameAbilityComponent* ability, const char* name, float value) {
        if (!ability) return false;
        const FAbilityAttributeArray& attrs = ability->AbilityAttributes;
        bool ok = false;
        const UC::TArray<FAbilityAttributeItem>& rep = attrs.RepAttributes;
        if (rep.IsValid()) {
            int32 n = rep.Num();
            for (int32 i = 0; i < n; ++i) {
                if (!rep.IsValidIndex(i)) continue;
                const FAbilityAttributeItem& item = rep[i];
                if (!item.AbilityAttribute) continue;
                const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
                std::string nm = attrObj->GetName();
                if (_stricmp(nm.c_str(), name) == 0) {
                    FAbilityAttributeItem* mut = const_cast<FAbilityAttributeItem*>(&item);
                    mut->CurrentAttributeValue = value;
                    ok = true;
                    break;
                }
            }
        }
        if (!ok) {
            const UC::TArray<FAbilityAttributeItem>& notRep = attrs.NotRepAttributes;
            if (notRep.IsValid()) {
                int32 n2 = notRep.Num();
                for (int32 i = 0; i < n2; ++i) {
                    if (!notRep.IsValidIndex(i)) continue;
                    const FAbilityAttributeItem& item = notRep[i];
                    if (!item.AbilityAttribute) continue;
                    const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
                    std::string nm = attrObj->GetName();
                    if (_stricmp(nm.c_str(), name) == 0) {
                        FAbilityAttributeItem* mut = const_cast<FAbilityAttributeItem*>(&item);
                        mut->CurrentAttributeValue = value;
                        ok = true;
                        break;
                    }
                }
            }
        }
        return ok;
    }

    bool GetAttrValueByName(UGameAbilityComponent* ability, const char* name, float& outValue) {
        if (!ability) return false;
        const FAbilityAttributeArray& attrs = ability->AbilityAttributes;
        const UC::TArray<FAbilityAttributeItem>& rep = attrs.RepAttributes;
        if (rep.IsValid()) {
            int32 n = rep.Num();
            for (int32 i = 0; i < n; ++i) {
                if (!rep.IsValidIndex(i)) continue;
                const FAbilityAttributeItem& item = rep[i];
                if (!item.AbilityAttribute) continue;
                const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
                std::string nm = attrObj->GetName();
                if (_stricmp(nm.c_str(), name) == 0) { outValue = item.CurrentAttributeValue; return true; }
            }
        }
        const UC::TArray<FAbilityAttributeItem>& notRep = attrs.NotRepAttributes;
        if (notRep.IsValid()) {
            int32 n2 = notRep.Num();
            for (int32 i = 0; i < n2; ++i) {
                if (!notRep.IsValidIndex(i)) continue;
                const FAbilityAttributeItem& item = notRep[i];
                if (!item.AbilityAttribute) continue;
                const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
                std::string nm = attrObj->GetName();
                if (_stricmp(nm.c_str(), name) == 0) { outValue = item.CurrentAttributeValue; return true; }
            }
        }
        return false;
    }

    void SafeWriteCurAttr(FAbilityAttributeItem* item, float v) {
        if (item) item->CurrentAttributeValue = v;
    }
}
