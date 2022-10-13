#pragma once
#include "winapp.hpp"
#include "vertex.hpp"
#include "d3d12_define.hpp"

struct alignas(256) camera_mat
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

class descriptor_heap;

class graphic_d3d12
{
public:
	graphic_d3d12(const winapp& winapp) noexcept;
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
	void render_begin();
	void render_init();
	void set_constantbuffer(const gsl::not_null<descriptor_heap*> heap);
	void render(const D3D12_VERTEX_BUFFER_VIEW& vbv);
	void render(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_INDEX_BUFFER_VIEW& ibv);
	void render(gsl::span<D3D12_VERTEX_BUFFER_VIEW> views, const D3D12_INDEX_BUFFER_VIEW& ibv);
	void render_end();
	void present();
	void wait_gpu();

public:
	inline const gsl::not_null<ID3D12Device*> get_device() const noexcept { return m_device.Get(); }
	inline const uint32_t get_frame_index() const noexcept { return m_frame_index; }
	
private:
	void resource_barrier(const D3D12_RESOURCE_STATES state);

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

	HANDLE m_fence_event = {};
	uint32_t m_frame_index = 0;

	std::array<uint64_t, FRAME_COUNT> m_fence_counter = {};

	D3D12_RESOURCE_STATES prev_state = D3D12_RESOURCE_STATE_PRESENT;
};