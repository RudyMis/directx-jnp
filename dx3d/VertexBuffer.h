#pragma once
#include "Mesh.h"
#include "VertexData.h"
#include <map>
#include <span>

class VertexBuffer {
public:
	void add(const Mesh& m, const std::vector<SceneConstantBuffer>& instance_data);

	HRESULT bake(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> command_list);
	D3D12_VERTEX_BUFFER_VIEW* view();
	UINT size();

private:

	struct MeshInfo {
		UINT vertex_begin;
		UINT instance_begin;
		UINT number_of_instances;
	};

	std::vector<VertexData> vertices;
	std::vector<SceneConstantBuffer> instance_data;
	std::map<LPCWSTR, MeshInfo> meshes_info;

	ComPtr<ID3D12Resource> upload_buffer;
	ComPtr<ID3D12Resource> vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
};

