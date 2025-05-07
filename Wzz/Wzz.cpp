// Wzz.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include "memhv.h"


#include <Windows.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <iostream>
#include <chrono>
#include <mutex>
#include "hack.h"
#include "SimpleOV.hpp"


std::mutex m;
ULONG64 ClientModule = 0;
ULONG64   ServerModule = 0;
//void ThreadBench(int id)
//{
//    while (true)
//    {
//        UINT64 totalOk = 0;
//        UINT64 totalFail = 0;
//
//        auto t1 = std::chrono::high_resolution_clock::now();
//        for (int i = 0; i < 100000; i++)
//        {
//            int offset = rand() % 20 + 1;
//            offset += 0x48;
//            volatile int readValue = HV::Read<int>(MainModule + offset);
//            volatile int readConfirm = HV::Read<int>(MainModule + offset);
//            if (readValue == readConfirm && readValue != 0)
//                totalOk++;
//            else
//            {
//                totalFail++;
//                m.lock();
//                printf("[!] Invalid read: %x %x\n", readValue, readConfirm);
//                m.unlock();
//            }
//        }
//        auto t2 = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//        m.lock();
//        printf("[+] Ok: %llu Fail: %llu In: %llu\n", totalOk, totalFail, duration);
//        m.unlock();
//    }
//}


VisCheck* g_VisChecker = nullptr;
std::string GetKeyName(int vk)
{
    UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC) << 16;
    char keyName[64] = { 0 };
    if (GetKeyNameTextA(scanCode, keyName, sizeof(keyName)))
        return keyName;
    else {
        // 鼠标侧键或未识别键，自定义处理
        switch (vk) {
        case VK_XBUTTON1: return "Mouse Button 4";
        case VK_XBUTTON2: return "Mouse Button 5";
        case VK_LBUTTON: return "Left Mouse";
        case VK_RBUTTON: return "Right Mouse";
        case VK_MBUTTON: return "Middle Mouse";
        default: return "Unknown";
        }
    }
}

std::pair<ULONG64, ULONG64> initHV() {

    printf("[>] Checking presence...\n");
    bool status = HV::CheckPresence();
    if (!status)
    {
        printf("[!] Hypervisor not running\n");
        getchar();
      
    }

    printf("[>] Instructing hypervisor to protect itself...\n");
    status = HV::Protect();
    if (!status)
    {
        printf("[!] Hypervisor self-protection failed\n");
        getchar();
        
    }

    printf("[>] Searching for target process...\n");
    UINT32 targetProcessId = LookupProcessId(L"cs2.exe");
    if (!targetProcessId)
    {
        printf("[!] Process not found\n");
        getchar();
     
    }

    printf("[+] Process has PID of %u\n", targetProcessId);

    printf("[>] Attaching to process...\n");
    status = HV::AttachToProcess(targetProcessId);
    if (!status)
    {
        printf("[!] Failed to attach\n");
        getchar();
       
    }

    printf("[+] Current process: EPROCESS -> 0x%llx CR3 -> 0x%llx\n", HV::Data::CurrentEPROCESS, HV::Data::CurrentDirectoryBase);
    printf("[+] Target process: EPROCESS -> 0x%llx CR3 -> 0x%llx\n", HV::Data::TargetEPROCESS, HV::Data::TargetDirectoryBase);

    printf("[>] Getting module base address...\n");
    ClientModule = GetModule(targetProcessId, L"client.dll");

    ServerModule = GetModule(targetProcessId, L"server.dll");
    if (!ClientModule)
    {
        printf("[!] Failed to get module base address\n");
        getchar();
        
    }

    printf("[+] Module is at 0x%llx\n", ClientModule);

    printf("[>] Reading module header...\n");


    return { ClientModule,ServerModule };

}

bool ESP = false;
bool Mouse = false;
bool AImbot = false;
bool Aimbot = false;

int aimbotHotkey = VK_XBUTTON2; // 默认热键：鼠标侧键2
bool aimbotToggle = false;
bool prevHotkeyState = false;
bool waitingForKeybind = false; // ⬅️ 你漏掉了这个
 bool toggleMode = true;

 bool aimbotActive = false;
enum BONEINDEX1 : DWORD
{
    head = 6,
    neck_0 = 5,
    spine_1 = 4,
    spine_2 = 2,
    pelvis = 0,
    arm_upper_L = 8,
    arm_lower_L = 9,
    hand_L = 10,
    arm_upper_R = 13,
    arm_lower_R = 14,
    hand_R = 15,
    leg_upper_L = 22,
    leg_lower_L = 23,
    ankle_L = 24,
    leg_upper_R = 25,
    leg_lower_R = 26,
    ankle_R = 27,
};

const char* boneNames[] = {
    u8"骨盆",         // pelvis
    u8"脊椎2",        // spine_2
    u8"脊椎1",        // spine_1
    u8"颈部",         // neck_0
    u8"头部",         // head
    u8"左上臂",       // arm_upper_L
    u8"左下臂",       // arm_lower_L
    u8"左手",         // hand_L
    u8"右上臂",       // arm_upper_R
    u8"右下臂",       // arm_lower_R
    u8"右手",         // hand_R
    u8"左大腿",       // leg_upper_L
    u8"左小腿",       // leg_lower_L
    u8"左脚踝",       // ankle_L
    u8"右大腿",       // leg_upper_R
    u8"右小腿",       // leg_lower_R
    u8"右脚踝",       // ankle_R
};


BONEINDEX1 boneValues[] = {
    pelvis, spine_2, spine_1, neck_0, head,
    arm_upper_L, arm_lower_L, hand_L,
    arm_upper_R, arm_lower_R, hand_R,
    leg_upper_L, leg_lower_L, ankle_L,
    leg_upper_R, leg_lower_R, ankle_R
};


float FOV = 50.0f;
float Smoothness = 1.0f;

int hotkey1 = VK_INSERT;



static bool showWindow = false; // 默认显示窗口


int main()
{
   // map.load_map("dust2");

    auto [Client, Server] = initHV();

    printf("[+] Client is at 0x%llx\n", Client);


  

    overlay::SetupWindow();
    if (!(overlay::CreateDeviceD3D(overlay::Window)))
        return 1;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();


   
    // 构建纹理（可选：你也可以提前构建一次后保存）

    ImGui::StyleColorsLight;

    while (!overlay::ShouldQuit)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1)  // &1 确保只触发一次
        {
            showWindow = !showWindow;  // 切换显示状态
        }
        overlay::Render();

        ImGuiIO& io = ImGui::GetIO();

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 10.0f;    // 窗口圆角


        // 暗色主题配色
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f); // 更深背景
        colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);

        colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);

        colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.2f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);

        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.25f, 0.35f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.35f, 0.45f, 1.00f);

        colors[ImGuiCol_CheckMark] = ImVec4(0.49f, 0.49f, 0.49f, 0.78f); // 高亮蓝色勾
        colors[ImGuiCol_SliderGrab] = ImVec4(0.49f, 0.49f, 0.49f, 0.78f);
      //  colors[ImGuiCol_SliderGrab] = ImVec4(0.3f, 0.5f, 0.9f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 0.78f);

        colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);

        //ImGui::ShowDemoWindow();

        float fps = 1.0f / io.DeltaTime;
        ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once); 
        if (ESP) {
            InitCheat(Client, Server);
            // 或者你可以考虑把 InitCheat 改个名字，更清晰一点
        }


        if (showWindow) {

            ImGui::Begin(u8"WZ辅助", nullptr, ImGuiWindowFlags_NoResize);
            ImGui::Text(u8"FPS: %.2f", fps);
            ImGui::Checkbox(u8"透视", &ESP);
            ImGui::Checkbox(u8"自瞄", &Aimbot);
            if (Aimbot)
            {
                //   const char* items[] = { u8"选项1", u8"选项2", u8"选项3" };

                ImGui::Text(u8"自瞄键位: ");
                ImGui::SameLine();

                if (ImGui::Button(u8"等待用户按键")) {
                    waitingForKeybind = true;
                }

                if (waitingForKeybind) {
                    for (int vk = 1; vk < 256; vk++) {
                        if (GetAsyncKeyState(vk) & 0x8000) {
                            aimbotHotkey = vk;
                            waitingForKeybind = false;
                            break;
                        }
                    }
                }

                ImGui::Checkbox(u8"持续自瞄", &toggleMode); // 勾选=切换模式，取消=长按模式
                // 显示当前热键名称（可选）
                ImGui::Text(u8"当前绑定键位: %s", GetKeyName(aimbotHotkey).c_str());



                ImGui::Combo("Aim Bone", &current_item, boneNames, IM_ARRAYSIZE(boneNames));
                if (ImGui::Button(u8"载入地图数据"))
                {
                    g_VisChecker = new VisCheck("de_mirage.opt");  // 创建对象
                 

                }

                // 显示滑块
                ImGui::PushItemWidth(200); // 设置滑动条宽度
                ImGui::SliderFloat(u8"平滑度", &Smoothness, 0.0f, 10.0f);
                ImGui::SliderFloat("FOV", &FOV, 0.0f, 500.0f);
                ImGui::PopItemWidth();
            }


            ImGui::End();
        }



        overlay::EndRender();
    }

    overlay::CloseOverlay();

    getchar();
    return EXIT_SUCCESS;
}