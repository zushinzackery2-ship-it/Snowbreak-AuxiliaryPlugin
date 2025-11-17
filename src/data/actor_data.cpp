#include "actor_data.h"

// Define static members
std::vector<ActorInfo> ActorDataManager::s_actors;
std::mutex ActorDataManager::s_mutex;
