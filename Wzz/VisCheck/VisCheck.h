#pragma once
#include <memory>
#include <vector>
#include "OptimizedGeometry.h"
#include "Math.hpp"


struct BVHNode {
    AABB bounds;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::vector<TriangleCombined> triangles;

    bool IsLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

class VisCheck
{
public:
    VisCheck(const std::string& optimizedGeometryFile);
    bool IsPointVisible(const VCVector3& point1, const VCVector3& point2);
    bool RayIntersectsTriangle(const VCVector3& rayOrigin, const VCVector3& rayDir,
        const TriangleCombined& triangle, float& t);

private:
    OptimizedGeometry geometry;
    std::vector<std::unique_ptr<BVHNode>> bvhNodes;
    std::unique_ptr<BVHNode> BuildBVH(const std::vector<TriangleCombined>& tris);
    bool IntersectBVH(const BVHNode* node, const VCVector3& rayOrigin, const VCVector3& rayDir, float maxDistance, float& hitDistance);
};
