#include "hack.h"
#include "memhv.h"
#include "ImGui/imgui.h"
#include <vector>
#include <string>
#include <iomanip>  // std::setprecision
#include <thread>
#include <future>
#include <mutex>

#include <vector>
#include <atomic>
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


struct EntityDrawData {
    std::string name;
    Vector3 headScreen;
    Vector3 footScreen;
    std::vector<BoneJointPos> bones;
    int entityTeam;
    int health;
    bool isTarget = false;
    Vector3 aimPosScreen;
};

void InitCheat(ULONG64 Client, ULONG64 ServerModule) {
    // 基础设置
    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    static uintptr_t cachedLocalPlayer = 0;
    static uintptr_t cachedGameEntitySystem = 0;
    static int cachedMaxIndex = 0;
    static DWORD lastUpdate = 0;
    DWORD now = GetTickCount();

    if (now - lastUpdate > 500) {
        cachedLocalPlayer = HV::Read<uintptr_t>(Client + cs2_dumper::offsets::client_dll::dwLocalPlayerPawn);
        cachedGameEntitySystem = HV::Read<uintptr_t>(Client + cs2_dumper::offsets::client_dll::dwGameEntitySystem);
        cachedMaxIndex = HV::Read<int>(cachedGameEntitySystem + cs2_dumper::offsets::client_dll::dwGameEntitySystem_highestEntityIndex);
        lastUpdate = now;
    }

    uintptr_t LocalPlayer = cachedLocalPlayer;
    int localTeam = HV::Read<int>(LocalPlayer + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
    ViewMatrix viewMatrix = GetGameViewMatrix(Client);
    VCVector3 localPos = HV::Read<VCVector3>(LocalPlayer + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
    Vector3 localEyes = HV::Read<Vector3>(LocalPlayer + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_vecLastClipCameraPos);
    auto drawList = ImGui::GetBackgroundDrawList();

    

    drawFovCircle(FOV);

    std::string Wname = GetWeaponName(LocalPlayer); //投掷物辅助
    if (Helper)
    {


        for (const auto& nade : nade) {
            if (Wname == nade.GrenadeName)
            {
                GHelperbool(nade.POS, nade.name.c_str(), viewMatrix, screenWidth, screenHeight);
             //   GHelperbool(nade.POSs1, nade.name.c_str(), viewMatrix, screenWidth, screenHeight);
                AngleHelper(nade.Eyes, nade.Angle, nade.name.c_str(), viewMatrix, screenWidth, screenHeight);

            }

              // std::cout << Wname << std::endl;
        }

    }
    std::vector<std::future<std::optional<EntityDrawData>>> futures;
    std::mutex drawMutex;

    for (int i = 0; i < 64; i++) {
        futures.push_back(std::async(std::launch::async, [=]() -> std::optional<EntityDrawData> {
            uintptr_t entity = GetBaseEntity(Client, i);
            if (!entity) return std::nullopt;

            uintptr_t pawn = GetPawnFromController(Client, i);
            if (!pawn) return std::nullopt;

            int team = HV::Read<int>(entity + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
            if (team == localTeam) return std::nullopt;

            int life = HV::Read<int>(pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_lifeState);
            if (life != 256) return std::nullopt;

            uintptr_t scene = HV::Read<uintptr_t>(pawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
            if (!scene) return std::nullopt;

            uintptr_t boneMatrix = HV::Read<uintptr_t>(scene + cs2_dumper::schemas::client_dll::CSkeletonInstance::m_modelState + 0x80);
            if (!boneMatrix) return std::nullopt;

            BoneJointData boneArray[30]{};
            HV::ReadMemory(boneMatrix, (ULONG64)&boneArray, sizeof(boneArray));

            std::vector<BoneJointPos> bonePosList;
            for (int j = 0; j < 30; ++j) {
                Vector3 screenPos;
                bool isVisible = WorldToScreen(boneArray[j].Pos, screenPos, viewMatrix, screenWidth, screenHeight);
                bonePosList.push_back({ boneArray[j].Pos, screenPos, isVisible });
            }
            Vector3  PosScreen;

            Vector3 eyePos = { 1647.953369, 1966.045410, 63.965004 };
            QAngle angles = { -17.204796, - 128.857727, 0.000000 };  // pitch, yaw

            Vector3 point = AngleToWorldPosition(eyePos, angles, 500.0f);
            WorldToScreen(point, PosScreen, viewMatrix, screenWidth, screenHeight);
            drawList->AddText(ImVec2(PosScreen.x, PosScreen.y),
                IM_COL32(255, 255, 255, 255), u8"目标");

            PlayerPosition pos = GetPlayerPosition(Client, entity);
            Vector3 headScreen, footScreen;
            if (!WorldToScreen(pos.head, headScreen, viewMatrix, screenWidth, screenHeight) ||
                !WorldToScreen(pos.foot, footScreen, viewMatrix, screenWidth, screenHeight)) {
                return std::nullopt;
            }

            Vector3 aimScreen;
            bool aimVisible = WorldToScreen(boneArray[6].Pos, aimScreen, viewMatrix, screenWidth, screenHeight);

            bool isTarget = false;
            if (Aimbot && aimVisible && g_VisChecker && g_VisChecker->IsPointVisible(localPos, VCVector3{ pos.head.x, pos.head.y, pos.head.z })) {
                float dist = getDistanceToCenter(aimScreen);
                if (dist < FOV) {
                    isTarget = true;
                }
            }

            EntityDrawData data;
            data.name = GetName(entity);
            data.headScreen = headScreen;
            data.footScreen = footScreen;
            data.bones = bonePosList;
            data.entityTeam = team;
            data.health = 100;
            data.isTarget = isTarget;
            data.aimPosScreen = aimScreen;
            return data;


           
     
            }));


    }




    Vector3 bestAimTarget{};
    int closest = 999999;
    bool foundTarget = false;

    for (auto& f : futures) {

        if (auto result = f.get(); result.has_value()) {
            const auto& data = result.value();
            float height = data.footScreen.y - data.headScreen.y;
            float width = height * 0.5f;

            drawList->AddRect(ImVec2(data.headScreen.x - width / 2, data.headScreen.y),
                ImVec2(data.headScreen.x + width / 2, data.footScreen.y),
                ImGui::ColorConvertFloat4ToU32(espColor), 0.0f, 0, 1.5f);

            drawList->AddText(ImVec2(data.headScreen.x, data.headScreen.y - 15),
                IM_COL32(255, 255, 255, 255), data.name.c_str());

            for (int i = 1; i < data.bones.size(); ++i) {
                if (data.bones[i - 1].IsVisible && data.bones[i].IsVisible) {
                    drawList->AddLine(ImVec2(data.bones[i - 1].ScreenPos.x, data.bones[i - 1].ScreenPos.y),
                        ImVec2(data.bones[i].ScreenPos.x, data.bones[i].ScreenPos.y),
                        IM_COL32(0, 255, 0, 255), 1.5f);
                }
            }

            if (data.isTarget) {
                int dist = getDistanceToCenter(data.aimPosScreen);
                if (dist < closest) {
                    closest = dist;
                    bestAimTarget = data.aimPosScreen;
                    foundTarget = true;
                }
            }
        }
    }


    bool keyPressed = (GetAsyncKeyState(aimbotHotkey) & 0x1);
    bool keyHeld = (GetAsyncKeyState(aimbotHotkey) & 0x8000);
    if (toggleMode) {
        if (keyPressed) aimbotActive = !aimbotActive;
    }
    else {
        aimbotActive = keyHeld;
    }

    if (Aimbot && foundTarget && aimbotActive) {
        startAimbot(bestAimTarget, Smoothness);
        std::cout << "H111";
    }

    if (RecordHelper && !hasRecorded) {
        std::cout << "RecordHelper" << std::endl;

        QAngle Angles = HV::Read<QAngle>(Client + cs2_dumper::offsets::client_dll::dwViewAngles);
        uintptr_t scene = HV::Read<uintptr_t>(LocalPlayer + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
        if (!scene) return;

        uintptr_t boneMatrix = HV::Read<uintptr_t>(scene + cs2_dumper::schemas::client_dll::CSkeletonInstance::m_modelState + 0x80);
        if (!boneMatrix) return;

        BoneJointData boneArray[30]{};
        HV::ReadMemory(boneMatrix, (ULONG64)&boneArray, sizeof(boneArray));

        std::vector<BoneJointPos> bonePosList;
        for (int j = 0; j < 30; ++j) {
            Vector3 screenPos;
            bool isVisible = WorldToScreen(boneArray[j].Pos, screenPos, viewMatrix, screenWidth, screenHeight);
            bonePosList.push_back({ boneArray[j].Pos, screenPos, isVisible });
        }
        NadePoint newPoint = {
            u8"默认",
            Wname,
            boneArray[24].Pos,                       // 站位坐标
            Angles,     // 角度
            localEyes                                // 摄像机坐标
        };

        // 准备保存点位的容器
        std::vector<NadePoint> nadePoints;

        // 尝试读取旧数据
        std::ifstream inFile("nadepoints.json");
        if (inFile.is_open()) {
            try {
                nlohmann::json oldJson;
                inFile >> oldJson;
                inFile.close();

                nadePoints = oldJson.get<std::vector<NadePoint>>();
            }
            catch (...) {
                inFile.close(); 
            }
        }

        // 添加新数据
        nadePoints.push_back(newPoint);

        // 保存到文件
        nlohmann::json j = nadePoints;
        std::ofstream outFile("nadepoints.json");
        outFile << j.dump(2); // 
        outFile.close();
        hasRecorded = true;  
    }

   

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

void AngleHelper(const Vector3& EyePosition, QAngle angles, std::string Name, const ViewMatrix& viewMatrix, int screenWidth, int screenHeight)
{

  Vector3 point = AngleToWorldPosition(EyePosition, angles, 700.0f);

    Vector3 screenPos;
    if (WorldToScreen(point, screenPos, viewMatrix, screenWidth, screenHeight)) {
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



