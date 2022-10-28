#pragma once
#include "winapp.hpp"
#include "vertex.hpp"
#include "d3d12_define.hpp"
#include "d3d12_gpu_buffer.hpp"

struct alignas(256) camera_mat
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

class descriptor_heap;

class graphic_d3d12
{
	friend std::unique_ptr<graphic_d3d12> std::make_unique<graphic_d3d12>();
	friend struct std::default_delete<graphic_d3d12>;

	static inline std::unique_ptr<graphic_d3d12> m_instance = nullptr;

	explicit graphic_d3d12() = default;
	~graphic_d3d12() = default;

public:
	static inline graphic_d3d12* create(gsl::not_null<winapp*> winapp)
	{
		Expects(m_instance == nullptr);

		m_instance = std::make_unique<graphic_d3d12>();
		m_instance->m_winapp = winapp;

		return m_instance.get();
	}

public:
	void create_devices();
	void create_pipelines();
	void create_cbv();
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
	winapp* m_winapp;

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
	std::array< std::unique_ptr<gpu_buffer<camera_mat>>, FRAME_COUNT> m_constant_buffers;


	HANDLE m_fence_event = {};
	uint32_t m_frame_index = 0;

	std::array<uint64_t, FRAME_COUNT> m_fence_counter = {};

	D3D12_RESOURCE_STATES prev_state = D3D12_RESOURCE_STATE_PRESENT;
};