#pragma once
#include "winapp.hpp"
#include "vertex.hpp"
#include "gpu_buffer.hpp"

namespace
{
	constexpr uint32_t FRAME_COUNT = 2;
}

struct alignas(256) _transform
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

class descriptor_heap;

class graphic_d3d12
{
public:
	graphic_d3d12(const winapp& winapp);
	~graphic_d3d12() noexcept(false);

public:
	graphic_d3d12(graphic_d3d12&&) = default;
	graphic_d3d12& operator=(graphic_d3d12&&) = default;

public:
	graphic_d3d12(const graphic_d3d12&) = delete;
	graphic_d3d12& operator=(const graphic_d3d12&) = delete;

public:
	void create_devices();
	void create_pipelines();
	void create_render_objects();
	void update();
	void render_begin();
	void render();
	void render_end();
	void wait_gpu();

private:
	const winapp& m_winapp;

	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_command_queue;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapchain;

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, FRAME_COUNT> m_color_buffer;
	std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> m_command_allocator;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
	std::unique_ptr<descriptor_heap> m_heap_rtv;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline;
	std::unique_ptr<descriptor_heap> m_heap_cbv;
	gpu_buffer<vertex> m_vertex_buffer;
	std::array<gpu_buffer<_transform>, FRAME_COUNT> m_constant_buffers;

	HANDLE m_fence_event;
	uint32_t m_frame_index;

	std::array<uint64_t, FRAME_COUNT> m_fence_counter;
};