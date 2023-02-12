#include "VertexBuffer.h"

void VertexBuffer::add(const Mesh& mesh, const std::vector<SceneConstantBuffer>& instance_data) {
	MeshInfo info;
	info.vertex_begin = vertices.size();
	info.instance_begin = this->instance_data.size();
	info.number_of_instances = instance_data.size();
	meshes_info[mesh.name()] = info;

	for (const auto& it : instance_data) {
		this->instance_data.emplace_back(it);
	}
	for (const auto& it : mesh.vertex_data()) {
		vertices.emplace_back(it);
	}
}

HRESULT VertexBuffer::bake(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> command_list) {
	HRESULT hr = S_OK;

	// Create vertex buffer
	{
		const UINT buffer_size = vertices.size() * sizeof(VertexData);
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
		hr = device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&vertex_buffer)
		); CHECK;

		vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
		vertex_buffer_view.StrideInBytes = sizeof(VertexData);
		vertex_buffer_view.SizeInBytes = buffer_size;
	}

	// Create upload buffer
	{
		UINT64 buffer_size = 0;
		hr = get_resource_size(vertex_buffer, buffer_size); CHECK;

		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_UPLOAD);
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);
		hr = device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&upload_buffer)
		); CHECK;

		// Populate upload buffer
		UINT8* vertex_data_begin = nullptr;
		CD3DX12_RANGE read_range(0, 0);
		hr = upload_buffer->Map(
			0,
			&read_range,
			reinterpret_cast<void**>(&vertex_data_begin)
		); CHECK;
		memcpy(vertex_data_begin, vertices.data(), buffer_size);
		upload_buffer->Unmap(0, nullptr);
	}

	// We don't need barrier since we are using fence to wait for data to load
	command_list->CopyResource(vertex_buffer.Get(), upload_buffer.Get());
	//auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	//command_list->ResourceBarrier(1, &barrier);

	return S_OK;
}

D3D12_VERTEX_BUFFER_VIEW* VertexBuffer::view() {
	return &vertex_buffer_view;
}

UINT VertexBuffer::size() {
	return vertices.size();
}