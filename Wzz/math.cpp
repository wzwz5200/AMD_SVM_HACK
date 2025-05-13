#include "math.h"
#include "Offset/offsets.hpp"
#include "./Offset/client_dll.hpp"
ViewMatrix GetGameViewMatrix(ULONG64 Client)
{
    return HV::Read<ViewMatrix>(Client + cs2_dumper::offsets::client_dll::dwViewMatrix);
}

uintptr_t GetPawnFromController(ULONG64 Client, int index)
{
    // Step 1: 获取 EntityList
    auto entityList = HV::Read<uintptr_t>(Client + cs2_dumper::offsets::client_dll::dwEntityList);
    if (entityList == 0)
        return 0;

    // Step 2: 获取 Controller 指针（原始函数的逻辑）
    auto controllerPage = HV::Read<uintptr_t>(entityList + 8 * (index >> 9) + 0x10);
    if (controllerPage == 0)
        return 0;

    auto playerController = HV::Read<uintptr_t>(controllerPage + 0x78 * (index & 0x1FF));
    if (playerController == 0)
        return 0;

    // Step 3: 读取 Pawn Handle（注意是 4 字节整数）
    uint32_t pawnHandle = HV::Read<uint32_t>(playerController + cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
    if ((pawnHandle & 0x7FFF) == 0x7FFF)
        return 0;

    int pawnIndex = pawnHandle & 0x7FFF;

    // Step 4: 通过 Handle 获取 Pawn 实体
    auto pawnPage = HV::Read<uintptr_t>(entityList + 8 * (pawnIndex >> 9) + 0x10);
    if (pawnPage == 0)
        return 0;

    auto pawn = HV::Read<uintptr_t>(pawnPage + 0x78 * (pawnIndex & 0x1FF));
    return pawn;
}

uintptr_t GetBaseEntity(ULONG64 Client, int index)
{
    auto entlistBase = HV::Read<uintptr_t>(Client + cs2_dumper::offsets::client_dll::dwEntityList);

    if (entlistBase == 0) {
        return 0;
    }

    auto entitylistBase = HV::Read<uintptr_t>(entlistBase + 0x8 * (index >> 9) + 16);

    if (entitylistBase == 0) {
        return 0;
    }

    //    std::cout << "J" <<std::hex << entitylistBase << std::endl;

    return HV::Read<uintptr_t>(entitylistBase + (0x78 * (index & 0x1ff)));
    
}

bool WorldToScreen(const Vector3& worldPosition, Vector3& screenPosition, const ViewMatrix& viewMatrix, int screenWidth, int screenHeight)
{
    float w = viewMatrix.matrix[3][0] * worldPosition.x +
        viewMatrix.matrix[3][1] * worldPosition.y +
        viewMatrix.matrix[3][2] * worldPosition.z +
        viewMatrix.matrix[3][3];

    if (w < 0.1f)
        return false;

    float x = viewMatrix.matrix[0][0] * worldPosition.x +
        viewMatrix.matrix[0][1] * worldPosition.y +
        viewMatrix.matrix[0][2] * worldPosition.z +
        viewMatrix.matrix[0][3];

    float y = viewMatrix.matrix[1][0] * worldPosition.x +
        viewMatrix.matrix[1][1] * worldPosition.y +
        viewMatrix.matrix[1][2] * worldPosition.z +
        viewMatrix.matrix[1][3];

    screenPosition.x = (screenWidth / 2.0f) * (1.0f + x / w);
    screenPosition.y = (screenHeight / 2.0f) * (1.0f - y / w);
    screenPosition.z = w;

    return true;
    
}

PlayerPosition GetPlayerPosition(ULONG64 Client, uintptr_t playerController)
{
    PlayerPosition pos = { {0,0,0}, {0,0,0} };

    auto playerPawn = HV::Read<uintptr_t>(playerController + cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
    if (playerPawn == 0) return pos;

    auto pawnAddress = GetBaseEntity(Client, playerPawn & 0x7FFF);
    if (pawnAddress == 0) return pos;

    // 获取基础位置
    Vector3 origin = HV::Read<Vector3>(pawnAddress + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
    Vector3 viewOffset = HV::Read<Vector3>(pawnAddress + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);

    // 计算头部和脚部位置
    pos.foot = origin;
    pos.head = {
        origin.x + viewOffset.x,
        origin.y + viewOffset.y,
        origin.z + viewOffset.z
    };

    return pos;
  
}

std::optional<Vector3> GetEyePos(uintptr_t addr) noexcept
{
    return std::optional<Vector3>();
}

int GetPlayerHealth(ULONG64 Client, uintptr_t playerController)
{
    if (playerController == 0) return 0;

    auto playerPawn = HV::Read<uintptr_t>(playerController + cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn);
    if (playerPawn == 0) return 0;

    if ((playerPawn & 0x7FFF) == 0x7FFF) return 0;

    auto pawnAddress = GetBaseEntity(Client, playerPawn & 0x7FFF);
    if (pawnAddress == 0) return 0;

    int health = HV::Read<int>(pawnAddress + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
    return (health >= 0 && health <= 100) ? health : 0;
}

std::string GetName(uintptr_t playerController)
{
    std::string name;
    uintptr_t temp = HV::Read<uintptr_t>(playerController + cs2_dumper::schemas::client_dll::CCSPlayerController::m_sSanitizedPlayerName);
    if (temp) {
        char buff[50]{};
        HV::ReadMemory(temp, (ULONG64)&buff, sizeof(buff));  // sizeof(buff) == 50
        name = std::string(buff);
    }
    else {
        name = "unknown";
    }
    return name;
}

int GetEntityVisible(uintptr_t EntityPawn)
{

    int State = HV::Read<int>(EntityPawn + 0x23D0 + 0xc);


    return State;
}

float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void moveMouseByOffset(int offsetX, int offsetY)
{
    while (offsetX != 0 || offsetY != 0)
    {
        // 每次移动限制在 [-127, 127]
        char dx = static_cast<char>(clamp(offsetX, -127.0f, 127.0f));
        char dy = static_cast<char>(clamp(offsetY, -127.0f, 127.0f));

        mouse_move(0, dx, dy, 0);

        offsetX -= dx;
        offsetY -= dy;
    }
}

void startAimbot(Vector3 headScreen, float smoothness)
{

    Vec2 screenCenter{ 2560 / 2.f, 1600 / 2.f };

    moveMouseByOffset(-(screenCenter.x - headScreen.x) / smoothness, -(screenCenter.y - headScreen.y) / smoothness);
}

void drawFovCircle(float radius)
{
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2{ 2560 / 2.f, 1600 / 2.f }, radius, ImColor(255, 255, 255, 255), 25, 2.f);
}

float getDistanceToCenter(const Vector3& screenPos)
{
    // 获取屏幕中心
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    Vec2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);

    // 计算欧几里得距离
    float dx = screenPos.x - screenCenter.x;
    float dy = screenPos.y - screenCenter.y;
    return sqrtf(dx * dx + dy * dy);
}

std::string GetMapName(ULONG64 Server)
{
    char buff[128]{};  // 增加缓冲区大小，确保能完整读取地图名

    if (HV::ReadMemory(Server + 0x14A5FC0, (ULONG64)&buff, sizeof(buff))) {
        // 强制添加字符串终止符
        buff[127] = '\0';

        // 找到第一个非打印字符或空字符的位置
        for (size_t i = 0; i < sizeof(buff); i++) {
            if (!isprint(static_cast<unsigned char>(buff[i])) || buff[i] == '\0') {
                buff[i] = '\0';
                break;
            }
        }

        // 拼接 ".opt" 并返回
        return std::string(buff) + ".opt";
    }

    return "";
}

std::string GetWeaponName(uintptr_t playerPawn)
{
    uintptr_t clipping_weapon = HV::Read<uintptr_t>(playerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_pClippingWeapon);
    if (!clipping_weapon) return "unknown";

    uintptr_t weapon_data = HV::Read<uintptr_t>(clipping_weapon + 0x10);


    // Read weapon name pointer
    uintptr_t name_ptr =HV::Read<uintptr_t>(weapon_data + 0x20);
    if (!name_ptr) return  "unknown";

    std::string current_name;
    constexpr size_t max_len = 64;  // get string
    for (size_t i = 0; i < max_len; ++i) {
        char ch = HV::Read<char>(name_ptr + i);
        if (ch == '\0') break;
        current_name += ch;
    }


    return current_name;
}

Vector3 AngleToWorldPosition(const Vector3& eyePos, const QAngle& viewAngles, float distance)
{
    Vector3 direction;

    float pitch = DEG2RAD(viewAngles.x);  // pitch（上下）
    float yaw = DEG2RAD(viewAngles.y);  // yaw（左右）

    direction.x = cos(pitch) * cos(yaw);
    direction.y = cos(pitch) * sin(yaw);
    direction.z = -sin(pitch);  // Source引擎里Pitch向下是正角度，所以要负号

    return eyePos + direction * distance;
}
