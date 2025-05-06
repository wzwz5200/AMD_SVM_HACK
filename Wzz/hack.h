#pragma once
#include "tool.h"
#include "math.h"
#include "SDK.h"
#include "ray_trace.h"
void InitCheat(ULONG64 Client,ULONG64 ServerModule);

extern bool Aimbot;
extern  int current_item;
extern float FOV;

  // 默认绑定为 F 键（可以改为其他虚拟键码）




extern float Smoothness;
extern std::string MapName;
extern map_loader map;
extern  bool toggleMode;
extern bool aimbotActive; // 👈 必须定义在函数外或static区
// 热键相关
extern int aimbotHotkey;   // 默认：鼠标侧键2
extern bool aimbotToggle;
extern bool prevHotkeyState;
extern bool  waitingForKeybind;
