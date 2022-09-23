#pragma once


namespace d3d12_factory
{
	Microsoft::WRL::ComPtr<ID3D12Device> create_device_11_0();
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> create_command_queue(gsl::not_null<ID3D12Device*> pDevice);
	Microsoft::WRL::ComPtr<IDXGIFactory4> create_factory();
	Microsoft::WRL::ComPtr<IDXGISwapChain3> create_swapchain(gsl::not_null<ID3D12CommandQueue*> pQueue, uint32_t width, uint32_t height, uint32_t buffer_count, HWND hwnd);
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> create_command_allocator(gsl::not_null<ID3D12Device*> pDevice);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> create_command_list(gsl::not_null<ID3D12Device*> pDevice, gsl::not_null<ID3D12CommandAllocator*> pCommandAllocator);
	Microsoft::WRL::ComPtr<ID3D12Resource> get_color_buffer(gsl::not_null<IDXGISwapChain3*> pSwapchain, uint32_t frameIndex);
};

