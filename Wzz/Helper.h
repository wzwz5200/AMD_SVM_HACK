#pragma once
#include "math.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


struct NadePoint {
    std::string name;  // 点位名称
    std::string  GrenadeName;  // 投掷物名称
    Vector3 POS;      // 站位坐标
    QAngle Angle;     // 瞄准角度
    Vector3 Eyes; //人物摄像机坐标
};

bool LoadNadeList(const std::string& filename, std::vector<NadePoint>& nadePoints);
void to_json(json& j, const Vector3& v);
void to_json(json& j, const QAngle& a);
void to_json(json& j, const NadePoint& p);
void from_json(const nlohmann::json& j, Vector3& v);
void from_json(const nlohmann::json& j, QAngle& a);
void from_json(const nlohmann::json& j, NadePoint& p);
