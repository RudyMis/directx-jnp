#pragma once
#include "helper.h"
#include "dx12helper.h"
#include "VertexData.h"
#include <optional>
#include <vector>
#include <variant>
#include <array>

class Mesh {
public:
	Mesh(LPCWSTR filename);
	Mesh() = default;

	LPCWSTR name() const;

	std::vector<VertexData> vertex_data() const;

	using Vertex = DirectX::XMFLOAT3;
	using UV = DirectX::XMFLOAT2;
	using Normal = DirectX::XMFLOAT3;
	struct PointRef {
		size_t id;
		size_t normal;
		size_t uv;
	};
	using Face = std::array<PointRef, 3>;
	
private:
	HRESULT parse_file(LPCWSTR filename);
	
	static Vertex load_vertex(std::ifstream& stream);
	static UV load_uv(std::ifstream& stream);
	static Normal load_normal(std::ifstream& stream);
	static Face load_face(std::ifstream& stream);

	LPCWSTR filename;

	std::vector<Vertex> vertices;
	std::vector<UV> uvs;
	std::vector<Normal> normals;
	std::vector<Face> faces;
};

