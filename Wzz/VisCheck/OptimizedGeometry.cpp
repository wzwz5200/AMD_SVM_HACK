#include "OptimizedGeometry.h"
#include "Parser.h"
#include <fstream>
#include <iostream>

bool OptimizedGeometry::CreateOptimizedFile(const std::string& rawFile, const std::string& optimizedFile) {
    // ���������� Parser ��� ��������? ������ ������ �� .vphys ����?
    Parser parser(rawFile);
    meshes = parser.GetCombinedList();

    std::ofstream out(optimizedFile, std::ios::binary);
    if (!out) {
        std::cerr << "�� ������?������?���� ��� ������: " << optimizedFile << std::endl;
        return false;
    }
    // ���������� ����?����?
    size_t numMeshes = meshes.size();
    out.write(reinterpret_cast<const char*>(&numMeshes), sizeof(size_t));
    for (const auto& mesh : meshes) {
        // ���������� ����?������������??����
        size_t numTris = mesh.size();
        out.write(reinterpret_cast<const char*>(&numTris), sizeof(size_t));
        for (const auto& tri : mesh) {
            out.write(reinterpret_cast<const char*>(&tri.v0), sizeof(VCVector3));
            out.write(reinterpret_cast<const char*>(&tri.v1), sizeof(VCVector3));
            out.write(reinterpret_cast<const char*>(&tri.v2), sizeof(VCVector3));
        }
    }
    out.close();
    return true;
}

bool OptimizedGeometry::LoadFromFile(const std::string& optimizedFile) {
    std::ifstream in(optimizedFile, std::ios::binary);
    if (!in) {
        std::cerr << "�� ������?������?���������������� ����: " << optimizedFile << std::endl;
        return false;
    }
    meshes.clear();
    size_t numMeshes;
    in.read(reinterpret_cast<char*>(&numMeshes), sizeof(size_t));
    for (size_t i = 0; i < numMeshes; ++i) {
        size_t numTris;
        in.read(reinterpret_cast<char*>(&numTris), sizeof(size_t));
        std::vector<TriangleCombined> mesh;
        mesh.resize(numTris);
        for (size_t j = 0; j < numTris; ++j) {
            in.read(reinterpret_cast<char*>(&mesh[j].v0), sizeof(VCVector3));
            in.read(reinterpret_cast<char*>(&mesh[j].v1), sizeof(VCVector3));
            in.read(reinterpret_cast<char*>(&mesh[j].v2), sizeof(VCVector3));
        }
        meshes.push_back(mesh);
    }
    in.close();
    return true;
}
