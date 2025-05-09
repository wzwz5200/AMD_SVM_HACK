#include "hack.h"
#include "memhv.h"
#include "ImGui/imgui.h"
#include <vector>
#include <string>
#include <iomanip>  // std::setprecision
ImVec4 espColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

std::string  MapName;


 int current_item = 0; // 当前选中的索引

const int bonePairs[][2] = {
    {0, 1}, {1, 2}, {2, 3},      // 脊柱
    {2, 4}, {4, 5}, {5, 6},      // 左臂
    {2, 7}, {7, 8}, {8, 9},      // 右臂
    {0, 10}, {10, 11}, {11, 12}, // 左腿
    {0, 13}, {13, 14}, {14, 15}  // 右腿
};


enum BONEINDEX : DWORD
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

std::vector<std::pair<BONEINDEX, BONEINDEX>> boneConnections = {
    {head, neck_0},
    {neck_0, spine_1},
    {spine_1, spine_2},
    {spine_2, pelvis},

    {spine_1, arm_upper_L},
    {arm_upper_L, arm_lower_L},
    {arm_lower_L, hand_L},

    {spine_1, arm_upper_R},
    {arm_upper_R, arm_lower_R},
    {arm_lower_R, hand_R},

    {pelvis, leg_upper_L},
    {leg_upper_L, leg_lower_L},
    {leg_lower_L, ankle_L},

    {pelvis, leg_upper_R},
    {leg_upper_R, leg_lower_R},
    {leg_lower_R, ankle_R},
};


BONEINDEX boneValues2[] = {
    pelvis, spine_2, spine_1, neck_0, head,
    arm_upper_L, arm_lower_L, hand_L,
    arm_upper_R, arm_lower_R, hand_R,
    leg_upper_L, leg_lower_L, ankle_L,
    leg_upper_R, leg_lower_R, ankle_R
};

Vector3 POSs = {

   1656.28, 1972, 4.711

};

Vector3 POSs1 = {
   1653.22f,
   1947.47f,
   4.48587f
};

Vector3 POSs2 = {
   1312.78f,
   1567.32f,
   223.004f
};

Vector3 POSs3 = {
   1316.26f,
   1539.72f,
   222.963f
};


void UpdateAimbotToggle() {
    bool currentKeyState = (GetAsyncKeyState(aimbotHotkey) & 0x8000);

    if (currentKeyState && !prevHotkeyState) {
        aimbotToggle = !aimbotToggle;
    }

    prevHotkeyState = currentKeyState;
}


const int bonePairCount = sizeof(bonePairs) / sizeof(bonePairs[0]);
void DrawSkeleton(const std::vector<BoneJointPos>& bones, ImU32 color)
{
    for (int i = 0; i < bonePairCount; ++i)
    {
        const auto& boneA = bones[bonePairs[i][0]];
        const auto& boneB = bones[bonePairs[i][1]];

        if (boneA.IsVisible && boneB.IsVisible)
        {
            ImVec2 a(boneA.Pos.x, boneA.Pos.y);
            ImVec2 b(boneB.Pos.x, boneB.Pos.y);
            ImGui::GetBackgroundDrawList()->AddLine(a, b, color, 1.5f);
        }
    }
}
static bool lastAimbotState = false;  // 记录上一次的Aimbot状态

void InitCheat(ULONG64 Client,ULONG64 ServerModule)
{
    
   
    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    static uintptr_t cachedLocalPlayer = 0;
    static uintptr_t cachedGameEntitySystem = 0;
    static int cachedMaxIndex = 0;
    static DWORD lastUpdate = 0;
    BONEINDEX selectedBone = boneValues2[current_item];
    DWORD now = GetTickCount();
    if (now - lastUpdate > 500) {
        cachedLocalPlayer = HV::Read<uintptr_t>(Client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn); 
        cachedGameEntitySystem = HV::Read<uintptr_t>(Client + cs2_dumper::offsets::client_dll::dwGameEntitySystem);
        cachedMaxIndex = HV::Read<int>(cachedGameEntitySystem + cs2_dumper::offsets::client_dll::dwGameEntitySystem_highestEntityIndex);
        lastUpdate = now;
    }

    uintptr_t LocalPlayer = cachedLocalPlayer;
    int MaxIndex = cachedMaxIndex;

    int localTeam = HV::Read<int>(LocalPlayer + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum); // m_iTeamNum

    ViewMatrix viewMatrix = GetGameViewMatrix(Client);
    auto drawList = ImGui::GetBackgroundDrawList();

    VCVector3  Localset = HV::Read<VCVector3>(LocalPlayer + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
    // FOV圆与锁定变量
    drawFovCircle(FOV);
    Vector3 targetScreenPosFin{ 0,0 };
    int closestToCrosshair{ 999999 };
    bool hasTarget = false;
   
    std::string Wname = GetWeaponName(LocalPlayer); //投掷物辅助
    if (Helper)
    {
  

        for (const auto& nade : nade) {
            if (Wname == nade.GrenadeName)
            {
                GHelperbool(nade.POSs, nade.name.c_str(), viewMatrix, screenWidth, screenHeight);
                GHelperbool(nade.POSs1, nade.name.c_str(), viewMatrix, screenWidth, screenHeight);
               
            }
           
            //   std::cout << nade.GrenadeName << std::endl;
        }

    }
      

    
    

    for (int i = 0; i < 32; i++) {
      
        uintptr_t entity = GetBaseEntity(Client, i);
        if (!entity) continue;

        uintptr_t entityPwn = GetPawnFromController(Client, i);
        if (!entityPwn) continue;

     
        int entityTeam = HV::Read<int>(entity + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
        if (entityTeam == localTeam) continue;
        int Life = HV::Read<int>(entityPwn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_lifeState);
        if (Life != 256) continue;
  
    //    int State = GetEntityVisible(entityPwn); // 是否可见

        //骨骼数据获取 //deBug模式 将entityPwn替换为LocalPlayer
        uintptr_t GameSceneNode = HV::Read<uintptr_t>(entityPwn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
        if (!GameSceneNode) continue;

        uintptr_t boneMatrix = HV::Read<uintptr_t>(GameSceneNode + cs2_dumper::schemas::client_dll::CSkeletonInstance::m_modelState + 0x80);
        if (!boneMatrix) continue;

        BoneJointData BoneArray[30]{};
        HV::ReadMemory(boneMatrix, (ULONG64)&BoneArray, sizeof(BoneArray));

        std::vector<BoneJointPos> BonePosList;
        for (int i = 0; i < 30; i++) {
            Vector3 screenPos;
            bool isVisible = false;

            if (WorldToScreen(BoneArray[i].Pos, screenPos, viewMatrix, screenWidth, screenHeight))
                isVisible = true;

            BonePosList.push_back({ BoneArray[i].Pos, screenPos, isVisible });
        }

        for (int i = 1; i < BonePosList.size(); ++i) {
            const auto& prev = BonePosList[i - 1];
            const auto& curr = BonePosList[i];
            if (prev.IsVisible && curr.IsVisible) {
                drawList->AddLine(ImVec2(prev.ScreenPos.x, prev.ScreenPos.y),
                    ImVec2(curr.ScreenPos.x, curr.ScreenPos.y),
                    IM_COL32(0, 255, 0, 255), 1.5f);
            }
        }

        std::string Name = GetName(entity);
        PlayerPosition pos = GetPlayerPosition(Client, entity);
        Vector3 GG = {
            -504,
            -661,
            184

        };
        Vector3 headScreen, footScreen ,g;
        if (!WorldToScreen(pos.head, headScreen, viewMatrix, screenWidth, screenHeight) ||
            !WorldToScreen(pos.foot, footScreen, viewMatrix, screenWidth, screenHeight))
            continue;
        WorldToScreen(GG, g, viewMatrix, screenWidth, screenHeight);
        drawList->AddText(ImVec2(headScreen.x, headScreen.y - 15),
            IM_COL32(255, 255, 255, 255), Name.c_str());


        float height = footScreen.y - headScreen.y;
        float width = height * 0.5f;
        drawList->AddRect(ImVec2(headScreen.x - width / 2, headScreen.y),
            ImVec2(headScreen.x + width / 2, footScreen.y),
            ImGui::ColorConvertFloat4ToU32(espColor), 0.0f, 0, 1.5f);
        drawList->AddText(ImVec2(headScreen.x, headScreen.y - 15),
            IM_COL32(255, 255, 255, 255), Name.c_str());

        //drawList->AddText(ImVec2(g.x, g.y - 15),
        //    IM_COL32(255, 255, 255, 255), "J222J");

        int BOne = 27;
     
     /*   std::string name = "拱门烟(跳投)";
        std::string grenadeName = "weapon_smokegrenade";*/

        // 输出 JSON 样式
       /* std::cout << std::fixed << std::setprecision(3);
        std::cout << "[\n  {\n";
        std::cout << "    \"name\": \"" << name << "\",\n";
        std::cout << "    \"GrenadeName\": \"" << Wname << "\",\n";
        std::cout << "    \"POSs\": ["
            << BonePosList[BOne].Pos.x << ", "
            << BonePosList[BOne].Pos.y << ", "
            << BonePosList[BOne].Pos.z << "],\n";
        std::cout << "    \"POSs1\": [0.0, 0.0, 0.0]\n";
        std::cout << "  }\n]\n";*/

  
        if (Aimbot && !lastAimbotState)
        {
            MapName = GetMapName(ServerModule);
            std::cout << "W";

        }
         lastAimbotState = Aimbot;  // 更新状态
     
         


        if (Aimbot && BonePosList[selectedBone].IsVisible&& g_VisChecker != nullptr) {
        

            float dist = getDistanceToCenter(BonePosList[selectedBone].ScreenPos );
            if (dist < FOV && dist < closestToCrosshair && g_VisChecker->IsPointVisible(Localset, VCVector3{ pos.head.x , pos.head.y, pos.head.z }))
            {
                closestToCrosshair = dist;
                targetScreenPosFin = BonePosList[selectedBone].ScreenPos;
                hasTarget = true;
            
            }


        }
   
    
    
       
    
    
    
    
    }

    
   // std::cout <<  current_item<< std::endl;
    bool keyPressed = (GetAsyncKeyState(aimbotHotkey) & 0x1);
    bool keyHeld = (GetAsyncKeyState(aimbotHotkey) & 0x8000);

    if (toggleMode) {
        if (keyPressed) {
            aimbotActive = !aimbotActive;
        }
    }
    else {
        aimbotActive = keyHeld;
    }

    if (Aimbot && hasTarget && aimbotActive) {
        startAimbot(targetScreenPosFin, Smoothness);
    }

    // 最终只锁一个最近敌人
   
}



void DrawTransparentSquareFromDiagonalPoints(const ImVec2& point1, const ImVec2& point2, ImU32 color, float thickness = 1.0f)
{
    // 获取ImGui的背景绘制列表
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

    // 计算四个角点
    ImVec2 corners[4] = {
        ImVec2(point1.x, point1.y),  // 左上
        ImVec2(point2.x, point1.y),  // 右上
        ImVec2(point2.x, point2.y),  // 右下
        ImVec2(point1.x, point2.y)   // 左下
    };

    // 设置透明度（这里使用传入color的alpha通道）
    ImU32 fill_color = color & 0x00FFFFFF;  // 清除原有alpha
    fill_color |= (color >> 24) / 2 << 24;   // 使用半透明

    // 填充正方形
    draw_list->AddQuadFilled(corners[0], corners[1], corners[2], corners[3], fill_color);

    // 绘制边框
    draw_list->AddQuad(corners[0], corners[1], corners[2], corners[3], color, thickness);
}

void GHelperbool(const Vector3& worldPosition,std::string Name,const ViewMatrix& viewMatrix, int screenWidth, int screenHeight)
{

    Vector3 g;
    Vector3 screenPos;
    if (WorldToScreen(worldPosition, screenPos, viewMatrix, screenWidth, screenHeight)) {
        auto drawList = ImGui::GetForegroundDrawList();

        float radius = 8.0f;
        ImVec2 center(screenPos.x, screenPos.y);

        // 圆形底色
        ImU32 fillColor = IM_COL32(0, 122, 255, 180);     // 半透明蓝色
        ImU32 borderColor = IM_COL32(255, 255, 255, 255); // 白色描边

        drawList->AddCircleFilled(center, radius, fillColor, 12);
        drawList->AddCircle(center, radius, borderColor, 12, 1.5f);

        // 半透明标签背景
        std::string labelText = Name;
        ImVec2 textSize = ImGui::CalcTextSize(labelText.c_str());
        ImVec2 textPos = ImVec2(center.x - textSize.x / 2, center.y - radius - textSize.y - 4);
        ImVec2 bgMin = ImVec2(textPos.x - 4, textPos.y - 2);
        ImVec2 bgMax = ImVec2(textPos.x + textSize.x + 4, textPos.y + textSize.y + 2);


        drawList->AddRectFilled(bgMin, bgMax, IM_COL32(0, 0, 0, 160), 4.0f);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), labelText.c_str());
    }

}



