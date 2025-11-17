#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <mutex>

struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    float Length() const {
        return sqrtf(x * x + y * y + z * z);
    }
};

struct BoneData {
    Vector3 worldPos;
    Vector3 screenPos;
    bool isValid;
    std::string name;
    int index;
    
    BoneData() : worldPos(), screenPos(), isValid(false), name(), index(-1) {}
};

struct ActorInfo {
    std::string className;
    std::string objectName;
    Vector3 worldPos;
    Vector3 screenPos;
    float distanceCm;
    void* actorPtr;
    bool isValid;
    int32_t arrayIndex;
    std::vector<BoneData> bones;
    float hpCurrent;
    float hpMax;
    bool isDead;
    
    ActorInfo() : className("Unknown"), objectName("Unknown"), 
                  worldPos(), screenPos(), distanceCm(0.0f),
                  actorPtr(nullptr), isValid(false), arrayIndex(-1),
                  hpCurrent(0.0f), hpMax(0.0f), isDead(false) {}
};

class ActorDataManager {
private:
    static std::vector<ActorInfo> s_actors;
    static std::mutex s_mutex;
    
public:
    static void UpdateActors(const std::vector<ActorInfo>& actors) {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_actors = actors;
    }
    
    static std::vector<ActorInfo> GetActors() {
        std::lock_guard<std::mutex> lock(s_mutex);
        return s_actors;
    }
    
    static void Clear() {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_actors.clear();
    }
};
