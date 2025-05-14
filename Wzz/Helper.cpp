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



void to_json(json& j, const Vector3& v) {
    j = json::array({ v.x, v.y, v.z });
}

void to_json(json& j, const QAngle& a) {
    j = json::array({ a.x, a.y, a.z });
}

void to_json(json& j, const NadePoint& p) {
    j = json{
        {"name", p.name},
        {"GrenadeName", p.GrenadeName},
        {"POS", p.POS},
        {"Angle", p.Angle},
        {"Eyes", p.Eyes}
    };
}





void from_json(const nlohmann::json& j, Vector3& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
    v.z = j.at(2).get<float>();
}


void from_json(const nlohmann::json& j, QAngle& a) {
    a.x = j.at(0).get<float>();
    a.y = j.at(1).get<float>();
    a.z = j.at(2).get<float>();
}


void from_json(const nlohmann::json& j, NadePoint& p) {
    j.at("name").get_to(p.name);
    j.at("GrenadeName").get_to(p.GrenadeName);
    j.at("POS").get_to(p.POS);
    j.at("Angle").get_to(p.Angle);
    j.at("Eyes").get_to(p.Eyes);
}

