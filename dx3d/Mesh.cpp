#include "Mesh.h"
#include <fstream>
#include <sstream>

Mesh::Mesh(LPCWSTR filename): filename(filename) {
    parse_file(filename);
}

LPCWSTR Mesh::name() const {
    return filename;
}

std::vector<VertexData> Mesh::vertex_data() const {
    std::vector<VertexData> res;
    for (const auto& face : faces) {
        for (const auto& point_ref : face) {
            VertexData data;
            data.position = vertices[point_ref.id];
            data.uv = uvs[point_ref.uv];
            res.push_back(data);
        }
    }
    return res;
}

HRESULT Mesh::parse_file(LPCWSTR filename) {
    std::ifstream input_str(filename);
    if (!input_str.is_open()) {
        return E_ABORT;
    }

    while (input_str.good()) {
        std::string type;
        input_str >> type;
        if (type == "v") {
            vertices.push_back(load_vertex(input_str));
        } else if (type == "vt") {
            uvs.push_back(load_uv(input_str));
        } else if (type == "vn") {
            normals.push_back(load_normal(input_str));
        } else if (type == "f") {
            faces.push_back(load_face(input_str));
        } else {
            // Other
        }
    }

    return S_OK;
}

Mesh::Vertex Mesh::load_vertex(std::ifstream& stream) {
    Vertex v{};
    stream >> v.x >> v.y >> v.z;
    return v;
}

Mesh::UV Mesh::load_uv(std::ifstream& stream) {
    UV uv{};
    stream >> uv.x >> uv.y;
    return uv;
}

Mesh::Normal Mesh::load_normal(std::ifstream& stream) {
    Normal normal{};
    stream >> normal.x >> normal.y >> normal.z;
    return normal;
}

Mesh::Face Mesh::load_face(std::ifstream& stream) {
    Face f{};
    char sep{};
    for (auto& ref : f) {
        stream >> ref.id >> sep >> ref.uv >> sep >> ref.normal;
        ref.id--;
        ref.uv--;
        ref.normal--;
    }
    return f;
}

