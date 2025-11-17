# 尘白禁区-辅助插件

本仓库为一个基于 DXGI/D3D11 Present Hook + ImGui 的游戏叠加层示例，包含基础的 ESP 绘制与自瞄模块，用于技术研究与交流。

**⚠️ 重要声明：**
- **使用目的**：仅供学习与技术交流使用。本项目旨在展示 DX11 Hook、ImGui 集成与渲染管线调度的实现原理。
- **法律与道德**：本项目不会提供任何用于绕过 ACE 反作弊保护的内核驱动、注入器或规避方法。使用本项目代码从事任何违反游戏服务条款或法律的活动，由使用者自行承担所有法律责任。
- **CppSDK 维护**：`CppSDK/` 为固定快照，本项目不再维护与更新该目录内的任何文件。如需适配该游戏更新版本或其他引擎版本，请自行处理。

## 功能概览
- **ESP 绘制**：
  - 骨骼点绘制（可禁用，则使用回退方案）
  - 回退绘制方案（圆角方框 + 中心十字）
  - 文本信息（对象名、类名、距离、指针）
  - 视线检测（LOS：绿色表示可见，黄色表示遮挡）
- **自瞄模块**：
  - 屏幕中心获取支持 ImGui/Win32 双回退
  - 目标圆点与"十字"中心 0.5px 像素对齐
  - FOV 圆形可视化
  - 平滑度与 FOV 可调
  - 自瞄线程默认 5ms 节奏（可在代码中调整）
  - 支持骨骼模式与十字中心模式
  - 可分别过滤怪物/Boss、仅可见目标等
- **UI/配置**：ImGui 面板（Insert 打开），F2 快速开关自瞄，F8 选择战斗动作；配置支持定期持久化（节流）。

## 环境要求
- Windows 10/11 x64
- Visual Studio 2022 Community（含 Windows SDK）

## 构建（不依赖编译脚本）

- **方案 A：Visual Studio 2022 GUI**（推荐）
  - 新建项目：`动态链接库 (DLL)`，平台 `x64`，配置 `Release`。
  - 将 `src/` 与 `imgui/` 下 `.cpp/.h` 以“添加现有项”的方式加入项目（包含 `imgui/backends`）。
  - C/C++ → 语言标准：`C++17`。
  - C/C++ → 代码生成：运行库设为 `多线程 (/MT)`（静态运行库）。
  - C/C++ → 附加包含目录：
    - `$(ProjectDir)imgui`
    - `$(ProjectDir)imgui\backends`
    - `$(ProjectDir)CppSDK\SDK`
  - 链接器 → 附加依赖项：`d3d11.lib; dxgi.lib; imm32.lib`（也可依赖源码中的 `#pragma comment(lib, ...)`）。
  - 生成产物名：`Dri.dll`。

- **方案 B：VS Developer PowerShell**（手动调用 cl，仍不需要脚本）
  - 打开“x64 Native Tools PowerShell for VS 2022”。
  - 在仓库根目录执行：
    ```powershell
    # 收集源码
    $Src = @()
    $Src += (Get-ChildItem -Recurse -Filter *.cpp -Path .\src | % FullName)
    $Src += (Get-ChildItem -Recurse -Filter imgui*.cpp -Path .\imgui | % FullName)
    $Src += (Get-ChildItem -Recurse -Filter imgui_impl_*.cpp -Path .\imgui\backends | % FullName)

    # 编译并生成 DLL（/MT 静态运行库）
    cl /LD /std:c++17 /EHsc /MT `
       /I .\imgui /I .\imgui\backends /I .\CppSDK\SDK `
       $Src d3d11.lib dxgi.lib imm32.lib `
       /Fe:Dri.dll
    ```
  - 产物：当前目录生成 `Dri.dll`（自行保管；本项目不提供注入器或任何相关工具）。

## 项目结构

```
.
├── README.md                          # 项目说明文档
├── CppSDK/                            # Unreal Engine SDK（固定快照，不再更新）
│   ├── SDK.hpp                        # 主 SDK 头文件
│   ├── SDK/                           # 引擎类和函数定义（200+ 文件）
│   ├── *.hpp                          # 工具类（Assertions、UTF-N 编码等）
│   └── ...
├── imgui/                             # ImGui 库与后端
│   ├── imgui.h / imgui.cpp            # 核心库
│   ├── imgui_*.cpp/h                  # 辅助模块
│   ├── backends/                      # D3D11、Win32 后端实现
│   └── examples/                      # 示例代码
└── src/                               # 主要源码
    ├── dllmain.cpp                    # DLL 入口点
    ├── hook/
    │   ├── dx11_hook.h/cpp            # D3D11 Present Hook 实现
    │   └── ...
    ├── renderer/
    │   ├── esp_renderer.h/cpp         # ESP 主渲染器
    │   ├── text_renderer.h/cpp        # 文本渲染
    │   └── ...
    ├── skeleton/
    │   ├── skeleton_data.h/cpp        # 骨骼数据结构
    │   ├── skeleton_renderer.h/cpp    # 骨骼与回退绘制
    │   └── ...
    ├── aimbot/
    │   ├── aimbot_core.h/cpp          # 自瞄主逻辑与线程
    │   ├── aimbot_config.h/cpp        # 自瞄配置
    │   └── ...
    ├── collector/
    │   ├── actor_collector.h/cpp      # Actor 收集与数据更新
    │   └── ...
    ├── data/
    │   ├── actor_data.h/cpp           # Actor 数据结构与管理
    │   └── ...
    ├── config/
    │   ├── esp_config.h/cpp           # ESP 配置
    │   ├── aimbot_config.h/cpp        # 自瞄配置
    │   ├── combat_config.h/cpp        # 战斗配置
    │   ├── config_io.h/cpp            # 配置持久化（INI 文件）
    │   └── combat/
    │       ├── combat_enhancer.h/cpp
    │       ├── attr_utils.h/cpp       # 属性工具
    │       └── ...
    └── ui/
        ├── ui_manager.h/cpp           # UI 管理与主窗口
        ├── aimbot_ui.h/cpp            # 自瞄配置面板
        ├── combat_ui.h/cpp            # 战斗配置面板
        └── ...
```

### 模块说明

| 模块 | 说明 |
|------|------|
| `hook/` | D3D11 Present Hook、消息处理、ImGui 初始化 |
| `renderer/` | ESP 绘制、文本渲染、颜色管理 |
| `skeleton/` | 骨骼点投影、骨骼绘制、回退方案（方框+十字） |
| `aimbot/` | 自瞄目标搜索、角度计算、平滑处理、多线程管理 |
| `collector/` | Actor 收集、位置与距离计算、骨骼投影 |
| `data/` | Actor 数据结构、屏幕坐标、生命值等信息 |
| `config/` | 各模块配置管理、INI 格式持久化 |
| `ui/` | ImGui 面板、配置编辑、实时数据显示 |

## 使用与热键
- `Insert`：打开/关闭 UI 面板
- `F2`：开关自瞄
- `F8`：触发战斗动作（示例）
- 默认自瞄按键：鼠标右键（可在 UI 中修改）

## 注意事项
- 本项目不提供注入器，注入方式自理，使用风险自负。
- 目标游戏受 ACE 反作弊保护。本项目不会提供任何用于绕过反作弊保护的内核驱动与注入器，也不提供规避思路或相关材料。
- ImGui 上下文缺失时已有判空与回退逻辑，但依然建议在合适时机注入。
- 坐标系统一为视口相对坐标；如观察到轻微像素级偏差，可在源码中加入绘制偏移配置做校准。

## 许可与声明
- **开源目的**：本项目仅为学习与技术交流目的开源，旨在展示 DX11 Hook 与 ImGui 集成等技术细节。
- **禁止用途**：任何人不得使用本项目代码从事以下活动：
  - 绕过、规避或削弱任何游戏反作弊系统（如 ACE、EAC 等）
  - 违反游戏服务条款的行为
  - 任何违反法律的活动
- **驱动与注入器**：本项目**不会提供**任何用于绕过 ACE 反作弊的内核驱动、注入器或相关工具，也不提供任何规避思路或相关材料。注入方式与使用风险由使用者自行承担。
- **CppSDK 维护**：`CppSDK/` 为固定快照，本项目不再维护与更新该目录内的任何文件。如需适配该游戏更新版本或其他引擎版本，请自行处理。
- **免责声明**：任何直接或间接由使用本项目产生的后果，由使用者自行承担。本项目作者不对任何由此产生的损害、法律纠纷或其他后果负责。
