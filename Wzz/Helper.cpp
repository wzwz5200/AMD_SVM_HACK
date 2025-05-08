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
            if (item.contains("name") && item.contains("POSs") && item.contains("POSs1")) {
                NadePoint np;
                np.name = item["name"];

                np.POSs = {
                    item["POSs"][0],
                    item["POSs"][1],
                    item["POSs"][2]
                };

                np.POSs1 = {
                    item["POSs1"][0],
                    item["POSs1"][1],
                    item["POSs1"][2]
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