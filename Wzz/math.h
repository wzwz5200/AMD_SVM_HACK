#pragma once
#include <basetsd.h>
#include <cstdint>
#include <optional>
#include "memhv.h"
#include "mouse.h"
#include "ImGui/imgui.h"

struct Vector3 {
    float x, y, z;

};

struct Vector2 {
    float x, y;
};

struct PlayerPosition {
    Vector3 head;    // 头部位置
    Vector3 foot;    // 脚部位置
};



struct ViewMatrix {
    float matrix[4][4];
};

struct BoneJointData
{
    Vector3 Pos;
    char pad[0x14];
};

struct BoneJointPos
{
    Vector3 Pos;
    Vector3 ScreenPos;
    bool IsVisible = false;
};

struct Vec2 {
	float x;
	float y;

	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float x, float y) : x(x), y(y) {}


	bool operator< (const Vec2& other) {
		if (x < other.x) return true;
		if (x > other.x) return false;
		return y < other.y;
	}
};



uintptr_t GetPawnFromController(ULONG64 Client, int index);
ViewMatrix GetGameViewMatrix(ULONG64 Client);  // 改为更具描述性的名称

uintptr_t GetBaseEntity(ULONG64 Client, int index);
bool WorldToScreen(const Vector3& worldPosition, Vector3& screenPosition, const ViewMatrix& viewMatrix, int screenWidth, int screenHeight);

PlayerPosition GetPlayerPosition(ULONG64 Client, uintptr_t playerController);
std::optional<Vector3> GetEyePos(uintptr_t addr) noexcept;


int GetPlayerHealth(ULONG64 Client, uintptr_t playerController);

std::string GetName(uintptr_t playerController);

int GetEntityVisible(uintptr_t EntityPawn);

float clamp(float value, float min, float max);


void moveMouseByOffset(int offsetX, int offsetY);


void startAimbot(Vector3 headScreen,float smoothness);

void drawFovCircle(float radius);

float getDistanceToCenter(const Vector3& screenPos);
std::string GetMapName(ULONG64 Server);

std::string GetWeaponName( uintptr_t playerPawn);

