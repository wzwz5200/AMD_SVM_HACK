#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <cstring>
#include <thread>
#include <future>
#include <memory>

#include "Math.hpp"

class Parser
{
private:
    std::string DataPath;

    std::vector<std::vector<Triangle>> TrianglesList;
    std::vector<std::vector<VCVector3>> VerticesList;
    std::vector<std::vector<TriangleCombined>> CombinedList;

    std::vector<std::vector<Triangle>> GetTriangles();
    std::vector<std::vector<VCVector3>> GetVertices();

    template<typename T>
    std::vector<T> ParseElements(const unsigned char* data, size_t dataSize);

    template<typename T>
    std::vector<std::vector<T>> ParseSection(const unsigned char* fileData, size_t fileSize, const std::string& sectionName);

    void fetchTriangles() {
        TrianglesList = GetTriangles();
    }

    void fetchVertices() {
        VerticesList = GetVertices();
    }

public:
    Parser(const std::string& path);

    const std::vector<std::vector<TriangleCombined>>& GetCombinedList() const {
        return CombinedList;
    }
};
