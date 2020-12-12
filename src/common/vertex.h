#pragma once

#include <vector>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

struct Vertex {
    Vertex() : tangent(0.0f), bitangent(0.0f) {}
    Vertex(Vec3f pos, Vec3f norm, Vec2f uv) : 
        position(pos), normal(norm), tangent(0.0f), bitangent(0.0f), texcoord(uv) {}

    Vec3f position;
    Vec3f normal;
    Vec3f tangent;
    Vec3f bitangent;
    Vec2f texcoord;

    static void CalcTangent(const Vertex & v0, const Vertex & v1, const Vertex & v2, Vec3f& tangent, Vec3f& bitangent) {
        Vec4f q1 = v1.position - v0.position;
        Vec4f q2 = v2.position - v0.position;

        Vec2f duv1 = v1.texcoord - v0.texcoord;
        Vec2f duv2 = v2.texcoord - v0.texcoord;
        XFloat du1 = duv1.u;
        XFloat dv1 = duv1.v;
        XFloat du2 = duv2.u;
        XFloat dv2 = duv2.v;
        XFloat r = 1.0f / (du1 * dv2 - du2 * dv1);

        tangent.x = r * (dv2 * q1.x - dv1 * q2.x);
        tangent.y = r * (dv2 * q1.y - dv1 * q2.y);
        tangent.z = r * (dv2 * q1.z - dv1 * q2.z);

        bitangent.x = r * (du1 * q2.x - du2 * q1.x);
        bitangent.y = r * (du1 * q2.y - du2 * q1.y);
        bitangent.z = r * (du1 * q2.z - du2 * q1.z);
    }
};
