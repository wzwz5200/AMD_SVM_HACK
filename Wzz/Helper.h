#pragma once
#include "math.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


struct NadePoint {
    std::string name;  // 点位名称
    Vector3 POSs;      // 站位坐标
    Vector3 POSs1;     // 瞄准坐标
};

bool LoadNadeList(const std::string& filename, std::vector<NadePoint>& nadePoints);