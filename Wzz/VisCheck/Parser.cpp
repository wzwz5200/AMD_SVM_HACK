#include "Parser.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <cstring>
#include <thread>

static std::vector<unsigned char> HexStringToBytes(const std::string& hex) {
    std::string hexCleaned;
    hexCleaned.reserve(hex.length());

    std::copy_if(hex.begin(), hex.end(), std::back_inserter(hexCleaned), [](char c) {
        return !std::isspace(c);
        });

    std::vector<unsigned char> bytes;
    bytes.reserve(hexCleaned.length() / 2);

    for (size_t i = 0; i < hexCleaned.length(); i += 2) {
        std::string byteString = hexCleaned.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

Parser::Parser(const std::string& path) : DataPath(path) {
    std::thread trianglesThread(&Parser::fetchTriangles, this);
    std::thread verticesThread(&Parser::fetchVertices, this);

    trianglesThread.join();
    verticesThread.join();

    for (size_t i = 0; i < TrianglesList.size(); ++i) {
        const std::vector<Triangle>& triangles = TrianglesList[i];
        const std::vector<VCVector3>& vertices = VerticesList[i];

        std::vector<TriangleCombined> Combined;

        for (const Triangle& triangle : triangles) {
            TriangleCombined t;
            t.v0 = vertices[triangle.a];
            t.v1 = vertices[triangle.b];
            t.v2 = vertices[triangle.c];
            Combined.push_back(t);
        }

        CombinedList.push_back(Combined);
    }
}

template<typename T>
std::vector<T> Parser::ParseElements(const unsigned char* data, size_t dataSize) {
    std::vector<T> elements;
    size_t elementSize = sizeof(T);
    elements.reserve(dataSize / elementSize);

    for (size_t i = 0; i < dataSize; i += elementSize) {
        T element;
        std::memcpy(&element, data + i, elementSize);
        elements.push_back(element);
    }

    return elements;
}

template<typename T>
std::vector<std::vector<T>> Parser::ParseSection(const unsigned char* fileData, size_t fileSize, const std::string& sectionName) {
    std::vector<std::vector<T>> elementsLists;
    std::istringstream fileStream(std::string(reinterpret_cast<const char*>(fileData), fileSize));
    std::string line;
    bool inMeshSection = false;

    while (std::getline(fileStream, line)) {
        if (line.find("m_meshes") != std::string::npos) {
            inMeshSection = true;
        }

        if (inMeshSection && line.find(sectionName) != std::string::npos) {
            std::getline(fileStream, line);
            if (line.find("#[") != std::string::npos) {
                std::string hexString;
                while (std::getline(fileStream, line) && line.find("]") == std::string::npos) {
                    hexString += line;
                }
                auto bytes = HexStringToBytes(hexString);
                auto parsedElements = ParseElements<T>(bytes.data(), bytes.size());
                elementsLists.push_back(std::move(parsedElements));
            }
        }
    }

    return elementsLists;
}

std::vector<std::vector<Triangle>> Parser::GetTriangles() {
    std::ifstream file(DataPath, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

    return ParseSection<Triangle>(fileData.data(), fileSize, "m_Triangles");
}

std::vector<std::vector<VCVector3>> Parser::GetVertices() {
    std::ifstream file(DataPath, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

    return ParseSection<VCVector3>(fileData.data(), fileSize, "m_Vertices");
}
