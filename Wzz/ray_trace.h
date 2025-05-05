#pragma once

#include <vector>
#include <string>
#include "vector.h"

struct BoundingBox {
    Vector min, max;
    bool intersect(const Vector& ray_origin, const Vector& ray_end) const;
};

struct Triangle {
    Vector p1, p2, p3;
    bool intersect(Vector ray_origin, Vector ray_end) const;
};

struct KDNode {
    BoundingBox bbox;
    std::vector<Triangle> triangle;
    KDNode* left, * right = nullptr;
    int axis;

    void deleteKDTree(KDNode* node);
};

class map_loader {
public:
    std::vector<Triangle> triangles;
    KDNode* kd_tree;

    void unload();
    void load_map(std::string map_name);
    bool is_visible(Vector ray_origin, Vector ray_end);
};

bool rayIntersectsKDTree(KDNode* node, const Vector& ray_origin, const Vector& ray_end);
BoundingBox calculateBoundingBox(const std::vector<Triangle>& triangles);
KDNode* buildKDTree(std::vector<Triangle>& triangles, int depth = 0);