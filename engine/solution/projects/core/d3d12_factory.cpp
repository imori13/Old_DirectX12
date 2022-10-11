using namespace Microsoft::WRL;

namespace d3d12_factory
{
	ComPtr<ID3D12Device> create_device_11_0()
	{
		ComPtr<ID3D12Device> device;

		const auto& hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(device.GetAddressOf()));

		Ensures(SUCCEEDED(hr));

		return device;
	}

	ComPtr<ID3D12CommandQueue> create_command_queue(gsl::not_null<ID3D12Device*> pDevice)
	{
		ComPtr<ID3D12CommandQueue> command_queue;

		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		const auto& hr = pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(command_queue.GetAddressOf()));
		Ensures(SUCCEEDED(hr));

		return command_queue;
	}

	ComPtr<IDXGIFactory4> create_factory()
	{
		ComPtr<IDXGIFactory4> factory = nullptr;
		const auto& hr = CreateDXGIFactory1(IID_PPV_ARGS(factory.GetAddressOf()));
		Ensures(SUCCEEDED(hr));

		return factory;
	}

	ComPtr<IDXGISwapChain3> create_swapchain(gsl::not_null<ID3D12CommandQueue*> pQueue, uint32_t width, uint32_t height, uint32_t buffer_count, HWND hwnd)
	{
		// create factory
		const auto& factory = create_factory();

		// setting swapchain
		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = buffer_count;
		desc.OutputWindow = hwnd;
		desc.Windowed = true;	// window_mode?
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// create swapchain
		ComPtr<IDXGISwapChain> swapchain;
		auto hr = factory->CreateSwapChain(pQueue, &desc, swapchain.GetAddressOf());
		Ensures(SUCCEEDED(hr));

		ComPtr<IDXGISwapChain3> swapchain3;
		hr = swapchain->QueryInterface(IID_PPV_ARGS(swapchain3.GetAddressOf()));
		Ensures(SUCCEEDED(hr));

		return swapchain3;
	}

	ComPtr<ID3D12CommandAllocator> create_command_allocator(gsl::not_null<ID3D12Device*> pDevice)
	{
		ComPtr<ID3D12CommandAllocator> command_allocator;
		const auto& hr = pDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(command_allocator.GetAddressOf())
		);
		Ensures(SUCCEEDED(hr));

		return command_allocator;
	}

	ComPtr<ID3D12GraphicsCommandList> create_command_list(gsl::not_null<ID3D12Device*> pDevice, gsl::not_null<ID3D12CommandAllocator*> pCommandAllocator)
	{
		ComPtr<ID3D12GraphicsCommandList> command_list;

		const auto& hr = pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			pCommandAllocator,
			nullptr,
			IID_PPV_ARGS(command_list.GetAddressOf())
		);
		Ensures(SUCCEEDED(hr));

		return command_list;
	}

	ComPtr<ID3D12Resource> get_color_buffer(gsl::not_null<IDXGISwapChain3*> pSwapchain, uint32_t frameIndex)
	{
		ComPtr<ID3D12Resource> resource;

		const auto& hr = pSwapchain->GetBuffer(frameIndex, IID_PPV_ARGS(resource.GetAddressOf()));
		Ensures(SUCCEEDED(hr));

		return resource;
	}
}