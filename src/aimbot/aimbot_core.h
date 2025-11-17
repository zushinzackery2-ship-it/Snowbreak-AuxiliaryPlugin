#pragma once
#include "../data/actor_data.h"
#include "../../imgui/imgui.h"
#include <vector>
#include <mutex>
#include <atomic>

struct Vector2D {
    float x;
    float y;
    Vector2D() : x(0), y(0) {}
    Vector2D(float _x, float _y) : x(_x), y(_y) {}
};

struct AimTarget {
    Vector3 worldPos;
    Vector2D screenPos;
    float distanceToCenter;
    std::string name;
    bool isValid;
    
    AimTarget() : worldPos(), screenPos(), distanceToCenter(99999.0f), name(), isValid(false) {}
};

class AimbotCore {
private:
    static std::atomic<bool> s_running;
    static HANDLE s_threadHandle;
    static AimTarget s_currentTarget;
    static std::mutex s_targetMutex;
    
    static DWORD WINAPI AimbotThread(LPVOID param);
    static void ProcessAimbot();
    static Vector2D CalculateAimAngles(const Vector3& cameraPos, const Vector3& targetPos);
    static AimTarget FindBestTarget();
    static bool IsValidTarget(const ActorInfo& actor);
    static Vector3 GetTargetPosition(const ActorInfo& actor);
    
public:
    static void Start();
    static void Stop();
    static bool IsRunning() { return s_running.load(); }
    static AimTarget GetCurrentTarget();
    static void RenderFOV(ImDrawList* drawList, float screenW, float screenH);
};
