#pragma once
#include <string>

namespace ConfigIO {
    void Initialize();
    void LoadAll();
    void SaveAll();
    void MarkDirty();
    void SaveIfNeeded();
    const char* GetConfigPath();
}
