#pragma once

#include <vector>
#include <array>
#include <stdint.h>
#include <memory.h>
#include "math/mat4.h"
#include "math/quat.h"
#include "hittable.h"
#include "common/vertex.h"
#include "common/transform.h"
#include "bvh.h"
#include "triangle.h"

class BvhNode;

class Mesh : public Hittable {
public:
    static std::shared_ptr<Mesh> CreateBox(const Vec3f& position, const Quaternion& rotation, XFloat scale, std::shared_ptr<Material> mat);

    Mesh(const Vec3f& position, const Quaternion& rotaton, XFloat scale, const char* filename, std::shared_ptr<Material> mat, bool interpolate_normal = true);
    Mesh(const Vec3f& position, const Quaternion& rotaton, std::shared_ptr<Material> mat, bool interpolate_normal = true);

    const std::vector<Vertex>& vertices() const { return vertices_; }
    const std::vector<std::shared_ptr<Hittable>>& triangles() const { return triangles_; }

    void AddVertex(Vec3f pos, Vec3f normal, Vec2f uv);
    void AddTriangle(uint32_t idx1, uint32_t idx2, uint32_t idx3);

    bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const override;
    // XFloat PDF(const Vec3f& o, const Vec3f& v) const override;
    // Vec3f Sample(const Vec3f& o) const override;
    void BuildBVH() override;

private:
    std::vector<Vertex> vertices_;
    std::vector<std::shared_ptr<Hittable>> triangles_;
    std::shared_ptr<BvhNode> root_;
    Transform transform_;
    bool interpolate_normal_;
};

