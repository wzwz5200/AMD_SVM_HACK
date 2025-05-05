#include "ray_trace.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define INRANGE(x,a,b)      (x >= a && x <= b) 
#define getBits( x )        (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define get_byte( x )       (getBits(x[0]) << 4 | getBits(x[1]))

bool BoundingBox::intersect(const Vector& ray_origin, const Vector& ray_end) const {
    Vector dir = ray_end - ray_origin;
    dir = dir.Normalize();

    float t1 = (min.x - ray_origin.x) / dir.x;
    float t2 = (max.x - ray_origin.x) / dir.x;
    float t3 = (min.y - ray_origin.y) / dir.y;
    float t4 = (max.y - ray_origin.y) / dir.y;
    float t5 = (min.z - ray_origin.z) / dir.z;
    float t6 = (max.z - ray_origin.z) / dir.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tmax < 0) {
        return false;
    }

    if (tmin > tmax) {
        return false;
    }

    return true;
}

bool Triangle::intersect(Vector ray_origin, Vector ray_end) const {
    const float EPSILON = 0.0000001f;
    Vector edge1, edge2, h, s, q;
    float a, f, u, v, t;
    edge1 = p2 - p1;
    edge2 = p3 - p1;
    h = CrossProduct(ray_end - ray_origin, edge2);
    a = edge1.Dot(h);

    if (a > -EPSILON && a < EPSILON)
        return false;

    f = 1.0 / a;
    s = ray_origin - p1;
    u = f * s.Dot(h);

    if (u < 0.0 || u > 1.0)
        return false;

    q = CrossProduct(s, edge1);
    v = f * (ray_end - ray_origin).Dot(q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    t = f * edge2.Dot(q);

    if (t > EPSILON && t < 1.0)
        return true;

    return false;
}

void KDNode::deleteKDTree(KDNode* node) {
    if (node == nullptr) return;
    deleteKDTree(node->left);
    deleteKDTree(node->right);
    delete node;
}

bool rayIntersectsKDTree(KDNode* node, const Vector& ray_origin, const Vector& ray_end) {
    if (node == nullptr) return false;

    if (!node->bbox.intersect(ray_origin, ray_end)) {
        return false;
    }

    if (node->triangle.size() > 0) {
        bool hit = false;
        for (const auto& tri : node->triangle) {
            if (tri.intersect(ray_origin, ray_end)) {
                hit = true;
            }
        }
        return hit;
    }

    bool hit_left = rayIntersectsKDTree(node->left, ray_origin, ray_end);
    bool hit_right = rayIntersectsKDTree(node->right, ray_origin, ray_end);

    return hit_left || hit_right;
}

BoundingBox calculateBoundingBox(const std::vector<Triangle>& triangles) {
    BoundingBox box;
    box.min = box.max = triangles[0].p1;
    for (const auto& tri : triangles) {
        for (const auto& p : { tri.p1, tri.p2, tri.p3 }) {
            box.min.x = std::min(box.min.x, p.x);
            box.min.y = std::min(box.min.y, p.y);
            box.min.z = std::min(box.min.z, p.z);
            box.max.x = std::max(box.max.x, p.x);
            box.max.y = std::max(box.max.y, p.y);
            box.max.z = std::max(box.max.z, p.z);
        }
    }
    return box;
}

KDNode* buildKDTree(std::vector<Triangle>& triangles, int depth) {
    if (triangles.empty()) return nullptr;

    KDNode* node = new KDNode();
    node->bbox = calculateBoundingBox(triangles);
    node->axis = depth % 3;

    if (triangles.size() <= 3) {
        node->triangle = triangles;
        return node;
    }

    auto comparator = [axis = node->axis](const Triangle& a, const Triangle& b) {
        float a_center, b_center;
        switch (axis) {
        case 0:
            a_center = (a.p1.x + a.p2.x + a.p3.x) / 3;
            b_center = (b.p1.x + b.p2.x + b.p3.x) / 3;
            break;
        case 1:
            a_center = (a.p1.y + a.p2.y + a.p3.y) / 3;
            b_center = (b.p1.y + b.p2.y + b.p3.y) / 3;
            break;
        case 2:
            a_center = (a.p1.z + a.p2.z + a.p3.z) / 3;
            b_center = (b.p1.z + b.p2.z + b.p3.z) / 3;
            break;
        }
        return a_center < b_center;
        };

    std::nth_element(triangles.begin(), triangles.begin() + triangles.size() / 2, triangles.end(), comparator);

    std::vector<Triangle> left_triangles(triangles.begin(), triangles.begin() + triangles.size() / 2);
    std::vector<Triangle> right_triangles(triangles.begin() + triangles.size() / 2, triangles.end());

    node->left = buildKDTree(left_triangles, depth + 1);
    node->right = buildKDTree(right_triangles, depth + 1);

    return node;
}

void map_loader::unload() {
    kd_tree->deleteKDTree(kd_tree);
}

void map_loader::load_map(std::string map_name) {
    auto begin = std::chrono::steady_clock::now();

    std::ifstream in(map_name + ".tri", std::ios::in | std::ios::binary);

    in.seekg(0, std::ios::end);
    std::streamsize fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::size_t num_elements = fileSize / sizeof(Triangle);
    triangles.resize(num_elements);
    if (!in.read(reinterpret_cast<char*>(triangles.data()), fileSize)) {
        throw std::runtime_error("Failed to read file: " + map_name + ".tri");
    }

    in.close();

    kd_tree = buildKDTree(triangles);
    std::vector<Triangle>().swap(triangles);

    auto i_end = std::chrono::steady_clock::now();
    std::cout << "[MAP] Loaded {" << map_name << "} " << std::chrono::duration<double, std::milli>(i_end - begin).count() << "ms" << std::endl;
}

bool map_loader::is_visible(Vector ray_origin, Vector ray_end) {
    return !rayIntersectsKDTree(kd_tree, ray_origin, ray_end);
}