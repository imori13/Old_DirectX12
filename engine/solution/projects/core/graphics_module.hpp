#pragma once

namespace module
{
struct graphics_createarg
{
	uint32_t frame_count;
	uint32_t render_width;
	uint32_t render_height;

	static inline const graphics_createarg& default_() noexcept
	{
		static constexpr graphics_createarg ret_val
		{
			.frame_count = 2,
			.render_width = 1280,
			.render_height = 1080,
		};

		return ret_val;
	}
};

class graphics
{
private:
	/*  シングルトンインスタンス  */
	static inline std::unique_ptr<graphics> m_instance = nullptr;

public:
	/*  モジュールを作成する  */
	static void create(const graphics_createarg& arg = graphics_createarg::default_());

	/*  モジュールを取得する  */
	static inline gsl::not_null<graphics*> get() noexcept { return m_instance.get(); }

public:
	/*  描画を開始する  */
	void begin();

	/*  描画を終了する  */
	void end();

public:
	void set_vertices(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_INDEX_BUFFER_VIEW& ibv);
	void set_stream(const D3D12_VERTEX_BUFFER_VIEW& view);
	void draw_call(uint32_t index_count, uint32_t instance_count);

private:
	void create_device(const graphics_createarg& arg);
	void create_pipelines();
	void create_cbv_heap();
	void send_resource_barrier(const D3D12_RESOURCE_STATES state);
	void present();
	void wait_gpu();

private:
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
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissor;
	std::array< std::unique_ptr<gpu_buffer<camera_mat>>, FRAME_COUNT> m_constant_buffers;
	std::unique_ptr<descriptor_heap> m_heap_cbv;
};
}
