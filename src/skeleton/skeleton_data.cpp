#include "skeleton_data.h"
#include "../../CppSDK/SDK/Basic.hpp"
#include "../../CppSDK/SDK/CoreUObject_classes.hpp"
#include "../../CppSDK/SDK/Engine_classes.hpp"

using namespace SDK;

void SkeletonCollector::CollectBones(ActorInfo& actorInfo, void* playerController) {
    if (actorInfo.className.find("BP_Mon") == std::string::npos && 
        actorInfo.className.find("BP_Boss") == std::string::npos) {
        return;
    }
    
    APlayerController* pc = static_cast<APlayerController*>(playerController);
    if (!pc) {
        UWorld* world = UWorld::GetWorld();
        if (!world) return;
        pc = UGameplayStatics::GetPlayerController(world, 0);
        if (!pc) return;
    }
    
    AActor* actor = static_cast<AActor*>(actorInfo.actorPtr);
    if (!actor) return;
    
    UActorComponent* baseComp = actor->GetComponentByClass(USkeletalMeshComponent::StaticClass());
    if (!baseComp) return;
    if (!baseComp->IsA(EClassCastFlags::SkeletalMeshComponent)) return;
    
    USkeletalMeshComponent* mesh = static_cast<USkeletalMeshComponent*>(baseComp);
    if (!mesh) return;
    
    int32 numBones = mesh->GetNumBones();
    if (numBones <= 0 || numBones > 10000) return;
    
    std::string targetBoneName;
    int targetBoneIndex = -1;
    
    for (int idx = 0; idx < numBones; ++idx) {
        FName boneName = mesh->GetBoneName(idx);
        if (!boneName.IsNone()) {
            std::string nameStr = boneName.GetRawString();
            std::string nameLower = nameStr;
            for (auto& c : nameLower) c = tolower(c);
            
            size_t pos = nameLower.find("head");
            if (pos != std::string::npos) {
                auto is_alnum_l = [](char ch){ return (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9'); };
                bool prevOk = (pos == 0) || !is_alnum_l(nameLower[pos - 1]);
                size_t endPos = pos + 4;
                bool nextOk = (endPos >= nameLower.size()) || !is_alnum_l(nameLower[endPos]);
                if (prevOk && nextOk) {
                    targetBoneName = nameStr;
                    targetBoneIndex = idx;
                    break;
                }
            }
        }
    }
    
    if (targetBoneIndex == -1) {
        for (int idx = 0; idx < numBones; ++idx) {
            FName boneName = mesh->GetBoneName(idx);
            if (!boneName.IsNone()) {
                std::string nameStr = boneName.GetRawString();
                if (nameStr == "FX_body02") {
                    targetBoneName = nameStr;
                    targetBoneIndex = idx;
                    break;
                }
            }
        }
    }
    
    if (targetBoneIndex != -1) {
        FName boneName = mesh->GetBoneName(targetBoneIndex);
        if (!boneName.IsNone()) {
            BoneData boneData;
            FVector pos = mesh->GetSocketLocation(boneName);
            boneData.index = targetBoneIndex;
            boneData.name = targetBoneName;
            boneData.worldPos.x = pos.X;
            boneData.worldPos.y = pos.Y;
            boneData.worldPos.z = pos.Z;
            FVector2D screen;
            if (UGameplayStatics::ProjectWorldToScreen(pc, pos, &screen, true)) {
                boneData.screenPos.x = screen.X;
                boneData.screenPos.y = screen.Y;
                boneData.screenPos.z = 1.0f;
                boneData.isValid = true;
            }
            actorInfo.bones.push_back(boneData);
        }
    }
}
