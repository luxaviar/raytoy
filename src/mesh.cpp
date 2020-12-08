#include "mesh.h"
#include "3rdparty/obj_loader.h"
#include "triangle.h"
#include "math/mat4.h"

std::shared_ptr<Mesh> Mesh::CreateBox(const Vec3f& position, const Quaternion& rotation, XFloat scale, std::shared_ptr<Material> mat) { 
    Vec3f vertices[] = {
        Vec3f(-0.5f, 0.5f, 0.5f), //0
        Vec3f(0.5f, 0.5f, 0.5f), //1
        Vec3f(-0.5f, -0.5f, 0.5f), //2
        Vec3f(0.5f, -0.5f, 0.5f), //3

        Vec3f(-0.5f, 0.5f, -0.5f), //4
        Vec3f(0.5f, 0.5f, -0.5f), //5
        Vec3f(-0.5f, -0.5f, -0.5f), //6
        Vec3f(0.5f, -0.5f, -0.5f), //7

        Vec3f(-0.5f, 0.5f, 0.5f),
        Vec3f(0.5f, 0.5f, 0.5f),
        Vec3f(0.5f, 0.5f, -0.5f),
        Vec3f(-0.5f, 0.5f, -0.5f),

        Vec3f(0.5f, -0.5f, 0.5f),
        Vec3f(-0.5f, -0.5f, 0.5f),
        Vec3f(-0.5f, -0.5f, -0.5f),
        Vec3f(0.5f, -0.5f, -0.5f),

        Vec3f(0.5f, 0.5f, 0.5f),
        Vec3f(0.5f, -0.5f, 0.5f),
        Vec3f(0.5f, 0.5f, -0.5f),
        Vec3f(0.5f, -0.5f, -0.5f),

        Vec3f(-0.5f, 0.5f, 0.5f), //0
        Vec3f(-0.5f, -0.5f, 0.5f), //2
        Vec3f(-0.5f, 0.5f, -0.5f), //4
        Vec3f(-0.5f, -0.5f, -0.5f), //6
    };

    Transform tx(position, rotation);
    for (auto& v : vertices) {
        v *= scale;
        v = tx.ApplyTransform(v);
    }

    int triangles[] = {
        0, 2, 1, //face front
        1, 2, 3,
        4, 5, 6, //face back
        5, 7, 6,
        8, 9, 11, //face top
        9, 10, 11,
        12, 13, 14, //face bottom
        12, 14, 15,
        16, 17, 18, //face right
        18, 17, 19,
        20, 22, 21, //face left
        21, 22, 23
    };
    
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(Vec3f::zero, Quaternion::identity, mat);
    for (auto v : vertices) {
        mesh->AddVertex(v, Vec3f::up, Vec2f::zero);
    }

    for (int i = 0; i < 12; ++i) {
        mesh->AddTriangle(triangles[i * 3], triangles[i * 3 + 1], triangles[i * 3 + 2]);
    }
    
    return mesh;
}

Mesh::Mesh(const Vec3f& position, const Quaternion& rotation, XFloat scale, const char* filename, std::shared_ptr<Material> mat, bool interpolate_normal) :
    Hittable(mat),
    transform_(position, rotation),
    interpolate_normal_(interpolate_normal)
{
    // Load .obj File
    objl::Loader loader;
    bool loadout = loader.LoadFile(filename);
    if (!loadout) return;

    Vec3f center(0.0);
    int total = 0;
    for(auto& mesh : loader.LoadedMeshes) {
        for (auto& vertex : mesh.Vertices) {
            center += Vec3f(vertex.Position.X, vertex.Position.Y, -vertex.Position.Z);
            ++total;
        }
    }
    center /= total;

    for(auto& mesh : loader.LoadedMeshes) {
        for (auto& vertex : mesh.Vertices) {
            Vec3f pos(vertex.Position.X, vertex.Position.Y, -vertex.Position.Z);
            pos -= center;
            pos *= scale;
            pos = transform_.ApplyTransform(pos);
            AddVertex(pos, 
                Vec3f(vertex.Normal.X, vertex.Normal.Y, -vertex.Normal.Z), 
                Vec2f(vertex.TextureCoordinate.X,
                    vertex.TextureCoordinate.Y < 0.0f ? 
                    -vertex.TextureCoordinate.Y : vertex.TextureCoordinate.Y)
            );
        }

        for (int i = 0; i < mesh.Indices.size(); i+=3) {
            auto i0 = mesh.Indices[i];
            auto i1 = mesh.Indices[i+2];
            auto i2 = mesh.Indices[i+1];

            auto& v0 = vertices_[i0];
            auto& v1 = vertices_[i1];
            auto& v2 = vertices_[i2];

            Vec3f tangent, bitangent;
            Vertex::CalcTangent(v0, v1, v2, tangent, bitangent);
            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;

            v0.bitangent += bitangent;
            v1.bitangent += bitangent;
            v2.bitangent += bitangent;
        }

        for (int i = 0; i < vertices_.size(); ++i) {
            auto& v = vertices_[i];
            Vec3f normal = v.normal;
            Vec3f tangent = v.tangent;
            Vec3f bitangent = v.bitangent;

            tangent = (tangent - (normal.Dot(tangent) * normal)).Normalize();
            float c = normal.Cross(tangent).Dot(bitangent);
            if (c < 0.0f) {
                tangent *= -1.0f;
            }

            v.tangent = tangent;
        }

        for (int i = 0; i < mesh.Indices.size(); i+=3) {
            auto i0 = mesh.Indices[i+2];
            auto i1 = mesh.Indices[i+1];
            auto i2 = mesh.Indices[i];
            AddTriangle(i0, i1, i2);
        }
    }
}

Mesh::Mesh(const Vec3f& position, const Quaternion& rotation, std::shared_ptr<Material> mat, bool interpolate_normal) : 
    Hittable(mat), transform_(position, rotation), interpolate_normal_(interpolate_normal) {

}

void Mesh::AddVertex(Vec3f pos, Vec3f normal, Vec2f uv) {
    vertices_.emplace_back(pos, normal, uv);
}

void Mesh::AddTriangle(uint32_t i0, uint32_t i1, uint32_t i2) {
    auto& v0 = vertices_[i0];
    auto& v1 = vertices_[i1];
    auto& v2 = vertices_[i2];
    auto tri = std::make_shared<Triangle>(v0, v1, v2, mat_ptr_, interpolate_normal_);

    triangles_.push_back(tri);
    if (triangles_.size() == 1) {
        bounding_box_ = tri->bounding_box();
    } else {
        bounding_box_ = AABB::Union(bounding_box_, tri->bounding_box());
    }
}

bool Mesh::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const {
    if (!root_) return false;
    return root_->Hit(r, t_min, t_max, rec);
}

void Mesh::BuildBVH() {
    if (triangles_.empty()) return;
    root_ = std::make_shared<BvhNode>(triangles_, 0, triangles_.size());
}

