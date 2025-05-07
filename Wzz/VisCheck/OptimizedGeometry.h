#pragma once
#include <string>
#include <vector>
#include "Math.hpp"

class OptimizedGeometry {
public:
    // Каждый элемен??ме?(список треугольнико?
    std::vector<std::vector<TriangleCombined>> meshes;

    // Загружае?оптимизированные данные из бинарног?файл?.opt
    bool LoadFromFile(const std::string& optimizedFile);

    // Создаё?оптимизированный файл из сырого .vphys файл?
    // извлек? нужные данные ?помощь?Parser, ?сохраняет результа??optimizedFile.
    bool CreateOptimizedFile(const std::string& rawFile, const std::string& optimizedFile);
};
