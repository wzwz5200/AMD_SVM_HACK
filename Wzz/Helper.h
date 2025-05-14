#pragma once
#include "math.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


struct NadePoint {
    std::string name;  // ��λ����
    std::string  GrenadeName;  // Ͷ��������
    Vector3 POS;      // վλ����
    QAngle Angle;     // ��׼�Ƕ�
    Vector3 Eyes; //�������������
};

bool LoadNadeList(const std::string& filename, std::vector<NadePoint>& nadePoints);
void to_json(json& j, const Vector3& v);
void to_json(json& j, const QAngle& a);
void to_json(json& j, const NadePoint& p);
void from_json(const nlohmann::json& j, Vector3& v);
void from_json(const nlohmann::json& j, QAngle& a);
void from_json(const nlohmann::json& j, NadePoint& p);
