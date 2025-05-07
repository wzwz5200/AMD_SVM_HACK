#pragma once
#include <string>
#include <vector>
#include "Math.hpp"

class OptimizedGeometry {
public:
    // ������ ������??��?(������ ������������?
    std::vector<std::vector<TriangleCombined>> meshes;

    // ��������?���������������� ������ �� ��������?����?.opt
    bool LoadFromFile(const std::string& optimizedFile);

    // �����?���������������� ���� �� ������ .vphys ����?
    // ������? ������ ������ ?������?Parser, ?��������� ��������??optimizedFile.
    bool CreateOptimizedFile(const std::string& rawFile, const std::string& optimizedFile);
};
