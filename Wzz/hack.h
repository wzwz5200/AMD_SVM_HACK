#pragma once
#include "tool.h"
#include "math.h"
#include "Helper.h"
#include "SDK.h"
#include "VisCheck/VisCheck.h"
#include "position.h"
void InitCheat(ULONG64 Client,ULONG64 ServerModule);

void GHelperbool(const Vector3& worldPosition,std::string Name, const ViewMatrix& viewMatrix, int screenWidth, int screenHeight);
void AngleHelper(const Vector3& EyePosition, QAngle angles, std::string Name, const ViewMatrix& viewMatrix, int screenWidth, int screenHeight);
extern  std::vector<NadePoint> nade;
extern bool Aimbot;
extern bool Helper;
extern bool RecordHelper;
extern bool hasRecorded;  // 是否已经执行记录逻辑
extern  int current_item;
extern float FOV;

  // 默认绑定为 F 键（可以改为其他虚拟键码）

extern VisCheck* g_VisChecker;

extern float Smoothness;
extern std::string MapName;
extern  bool toggleMode;
extern bool aimbotActive; // 👈 必须定义在函数外或static区
// 热键相关
extern int aimbotHotkey;   // 默认：鼠标侧键2
extern bool aimbotToggle;
extern bool prevHotkeyState;
extern bool  waitingForKeybind;
