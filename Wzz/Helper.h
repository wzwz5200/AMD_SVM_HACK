#pragma once
#include "math.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


struct NadePoint {
    std::string name;  // ��λ����
    Vector3 POSs;      // վλ����
    Vector3 POSs1;     // ��׼����
};

bool LoadNadeList(const std::string& filename, std::vector<NadePoint>& nadePoints);