#include "Helper.h"
bool LoadNadeList(const std::string& filename, std::vector<NadePoint>& nadePoints) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        for (const auto& item : j) {
            if (item.contains("name") &&
                item.contains("GrenadeName") &&
                item.contains("POS") &&
                item.contains("Angle") &&
                item.contains("Eyes"))
            {
                NadePoint np;
                np.name = item["name"].get<std::string>();
                np.GrenadeName = item["GrenadeName"].get<std::string>();

                // Vector3 POS
                np.POS = {
                    item["POS"][0].get<float>(),
                    item["POS"][1].get<float>(),
                    item["POS"][2].get<float>()
                };

                // QAngle POSs1
                np.Angle = {
                    item["Angle"][0].get<float>(),
                    item["Angle"][1].get<float>(),
                    item["Angle"][2].get<float>()
                };

                // Vector3 Eyes
                np.Eyes = {
                    item["Eyes"][0].get<float>(),
                    item["Eyes"][1].get<float>(),
                    item["Eyes"][2].get<float>()
                };

                nadePoints.push_back(np);
            }
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "解析 JSON 失败: " << e.what() << std::endl;
        return false;
    }
}