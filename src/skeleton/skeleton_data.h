#pragma once
#include "../data/actor_data.h"

class SkeletonCollector {
public:
    static void CollectBones(struct ActorInfo& actorInfo, void* playerController);
};
