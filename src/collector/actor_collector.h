#pragma once
#include <windows.h>
#include <atomic>

class ActorCollector {
private:
    static std::atomic<bool> s_running;
    static HANDLE s_threadHandle;
    
public:
    static void Start();
    static void Stop();
    static bool IsRunning() { return s_running; }
    
private:
    static DWORD WINAPI CollectorThread(LPVOID param);
    static void CollectActorData();
};
