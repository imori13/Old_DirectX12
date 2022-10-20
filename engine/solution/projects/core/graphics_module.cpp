#include "include.hpp"

using namespace Microsoft::WRL;

namespace
{
	void enable_debuglayer()
	{
		ComPtr<ID3D12Debug> pDebug;
		const auto& hr = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()));
		Ensures(SUCCEEDED(hr));

		// enable debug layer
		pDebug->EnableDebugLayer();
	}
}

namespace module
{
	void graphics::create(const graphics_createarg& arg)
	{
		/*  既にインスタンスが生成されていたらエラー  */
		Ensures(m_instance == nullptr);

		/*  インスタンスを生成する  */
		m_instance = std::make_unique<graphics>();

		m_instance->m_viewport = D3D12_VIEWPORT
		{
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(arg.render_width),
			.Height = static_cast<float>(arg.render_height),
			.MinDepth = 0,
			.MaxDepth = 1,
		};

		m_instance->m_scissor = D3D12_RECT
		{
			.left = 0,
			.top = 0,
			.right = gsl::narrow<long>(arg.render_width),
			.bottom = gsl::narrow<long>(arg.render_height),
		};
	}

	void graphics::begin()
	{
		/*  コマンドリストをクリアする  */
		m_command_allocator.at(m_frame_index)->Reset();
		m_command_list->Reset(m_command_allocator.at(m_frame_index).Get(), nullptr);

		/*  リソースバリアを変更  */
		send_resource_barrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

		/*  描画ヒープを変更  */
		const auto& handle = m_heap_rtv->at(m_frame_index).cpu_handle;
		m_command_list->OMSetRenderTargets(1, &handle, FALSE, nullptr);

		/*  描画ヒープをクリア  */
		constexpr const std::array<float, 4> clear_color = { 0.125f,0.1f,0.1f,1.0f };
		m_command_list->ClearRenderTargetView(m_heap_rtv->at(m_frame_index).cpu_handle, clear_color.data(), 0, nullptr);

		m_command_list->SetGraphicsRootSignature(m_root_signature.Get());
		m_command_list->SetPipelineState(m_pipeline.Get());
		m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_command_list->RSSetViewports(1, &m_viewport);
		m_command_list->RSSetScissorRects(1, &m_scissor);

		m_command_list->SetDescriptorHeaps(1, m_heap_cbv->get_address());
		m_command_list->SetGraphicsRootConstantBufferView(0, m_heap_cbv->get_cbv_view_desc().BufferLocation);
	}

	void graphics::end()
	{
		/*  リソースバリアを変更  */
		send_resource_barrier(D3D12_RESOURCE_STATE_PRESENT);

		/*  コマンドリストをクローズ  */
		m_command_list->Close();

		/*  コマンド送信  */
		std::array<ID3D12CommandList*, 1> ppCmdLists = { m_command_list.Get() };
		m_command_queue->ExecuteCommandLists(1, ppCmdLists.data());

		/*  スワップチェーン更新  */
		present();
	}

	void graphics::set_vertices(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_INDEX_BUFFER_VIEW& ibv)
	{
		m_command_list->IASetVertexBuffers(0, 1, &vbv);
		m_command_list->IASetIndexBuffer(&ibv);
	}

	void graphics::set_stream(const D3D12_VERTEX_BUFFER_VIEW& view)
	{
		m_command_list->IASetVertexBuffers(1, 1, &view);
	}

	void graphics::draw_call(uint32_t index_count, uint32_t instance_count)
	{
		const D3D12_VERTEX_BUFFER_VIEW& instance_heap{};

		m_command_list->IASetVertexBuffers(1, 1, &instance_heap);
		m_command_list->DrawInstanced(index_count, instance_count, 0, 0);
	}

	void graphics::create_device(const graphics_createarg& arg)
	{
		using namespace d3d12_factory;

		HRESULT hr{};

#ifdef _DEBUG
		enable_debuglayer();
#endif

		// create device
		m_device = create_device_11_0();
		m_command_queue = create_command_queue(m_device.Get());

		// create swapchain
		{
			// create swapchain
			m_swapchain = create_swapchain(m_command_queue.Get(), m_winapp.get_width(), m_winapp.get_height(), FRAME_COUNT, m_winapp.get_hwnd());

			// get current back buffer index
			m_frame_index = m_swapchain->GetCurrentBackBufferIndex();
		}

		// create command allocator
		{
			for (auto& command_allocator : m_command_allocator)
			{
				command_allocator = create_command_allocator(m_device.Get());
			}
		}

		// create commandlist
		{
			m_command_list = create_command_list(m_device.Get(), m_command_allocator.at(m_frame_index).Get());
		}

		// create render target view
		{
			m_heap_rtv = std::make_unique<descriptor_heap>(m_device.Get(), FRAME_COUNT, heap_type::rtv);

			for (auto i = 0u; i < m_color_buffer.size(); ++i)
			{
				m_color_buffer.at(i) = get_color_buffer(m_swapchain.Get(), i);

				m_heap_rtv->create_rtv(m_color_buffer.at(i).Get());
			}
		}

		// create fence
		{
			// reset counter
			for (auto& fence_counter : m_fence_counter)
			{
				fence_counter = 0;
			}

			// create fence
			hr = m_device->CreateFence(
				m_fence_counter.at(m_frame_index),
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(m_fence.GetAddressOf())
			);
			Ensures(SUCCEEDED(hr));

			++m_fence_counter.at(m_frame_index);

			// create event
			m_fence_event = CreateEvent(nullptr, false, false, nullptr);
			Ensures(m_fence_event != nullptr);
		}

		m_command_list->Close();
	}

	void graphics::create_pipelines()
	{
		HRESULT hr{};

		D3D12_ROOT_PARAMETER param{};
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param.Descriptor.ShaderRegister = 0;
		param.Descriptor.RegisterSpace = 0;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		const auto flag =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.NumParameters = 1;
		desc.NumStaticSamplers = 0;
		desc.pParameters = &param;
		desc.pStaticSamplers = nullptr;
		desc.Flags = flag;

		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error_blob;

		// Serialize
		hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			blob.GetAddressOf(),
			error_blob.GetAddressOf());
		Ensures(SUCCEEDED(hr));

		// RootSignature
		hr = m_device->CreateRootSignature(
			0,
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			IID_PPV_ARGS(m_root_signature.GetAddressOf()));

		// パイプラインステート
		const std::vector<D3D12_INPUT_ELEMENT_DESC> elements =
		{
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			},

			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				0
			},

			{
				"WORLD",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
				1
			},
			{
				"WORLD",
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
				1
			},
			{
				"WORLD",
				2,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
				1
			},
			{
				"WORLD",
				3,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
				1
			},
		};

		/*  ラスタライザの設定  */
		auto descRS = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
		descRS.CullMode = D3D12_CULL_MODE_NONE;

		/*  ブレンドステートの設定  */
		const auto& descBS = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());

		ComPtr<ID3DBlob> vs_blob;
		ComPtr<ID3DBlob> ps_blob;

		// 頂点シェーダの読み込み
		hr = D3DReadFileToBlob(L"simple_vs.cso", vs_blob.GetAddressOf());
		Ensures(SUCCEEDED(hr));

		hr = D3DReadFileToBlob(L"simple_ps.cso", ps_blob.GetAddressOf());
		Ensures(SUCCEEDED(hr));

		// パイプラインステートの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_pipeline_state{};
		desc_pipeline_state.InputLayout = D3D12_INPUT_LAYOUT_DESC{ elements.data(),gsl::narrow<UINT>(elements.size()) };
		desc_pipeline_state.pRootSignature = m_root_signature.Get();
		desc_pipeline_state.VS = CD3DX12_SHADER_BYTECODE(vs_blob.Get());
		desc_pipeline_state.PS = CD3DX12_SHADER_BYTECODE(ps_blob.Get());
		desc_pipeline_state.RasterizerState = descRS;
		desc_pipeline_state.BlendState = descBS;
		desc_pipeline_state.DepthStencilState.DepthEnable = false;
		desc_pipeline_state.DepthStencilState.StencilEnable = false;
		desc_pipeline_state.SampleMask = UINT_MAX;
		desc_pipeline_state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc_pipeline_state.NumRenderTargets = 1;
		gsl::at(desc_pipeline_state.RTVFormats, 0) = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc_pipeline_state.DSVFormat = DXGI_FORMAT_UNKNOWN;
		desc_pipeline_state.SampleDesc.Count = 1;
		desc_pipeline_state.SampleDesc.Quality = 0;

		hr = m_device->CreateGraphicsPipelineState(&desc_pipeline_state, IID_PPV_ARGS(m_pipeline.GetAddressOf()));
		Ensures(SUCCEEDED(hr));
	}

	void graphics::create_cbv_heap()
	{
		const auto& eye_pos = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
		const auto& dir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const auto& up_ward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		const auto& fov_y = DirectX::XMConvertToRadians(90);
		//const auto& aspect = static_cast<float>(app->get_width()) / static_cast<float>(app->get_height());
		const auto& aspect = static_cast<float>(1280) / static_cast<float>(1080);

		camera_mat _trans{};
		for (auto& buffer : m_constant_buffers)
		{
			buffer = std::make_unique<gpu_buffer<camera_mat>>(m_device.Get(), sizeof(camera_mat));
			buffer->map(_trans);

			buffer->data()->view = DirectX::XMMatrixLookToLH(eye_pos, dir, up_ward);
			buffer->data()->proj = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, 0.0001f, 100);
		}

		// bind heap
		m_heap_cbv = std::make_unique<descriptor_heap>(m_device.Get(), FRAME_COUNT, heap_type::cbv_srv_uav, heap_flag::shader_visible);
		for (auto& buffer : m_constant_buffers)
			m_heap_cbv->create_cbv(buffer->get_resource());
	}

	void graphics::send_resource_barrier(const D3D12_RESOURCE_STATES state)
	{
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_color_buffer.at(m_frame_index).Get();
		barrier.Transition.StateBefore = prev_state;
		barrier.Transition.StateAfter = state;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		/*  リソースバリア  */
		m_command_list->ResourceBarrier(1, &barrier);

		/*  状態の更新  */
		prev_state = state;
	}

	void graphics::present()
	{
		/*  画面に表示  */
		constexpr uint32_t interval = 1;
		m_swapchain->Present(interval, 0);

		const auto currentValue = m_fence_counter.at(m_frame_index);

		/*  シグナル処理 */
		m_command_queue->Signal(m_fence.Get(), currentValue);

		/*  バックバッファ番号を更新  */
		m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

		/*  次のフレームの描画準備がまだであれば待機する  */
		if (m_fence->GetCompletedValue() < m_fence_counter.at(m_frame_index))
		{
			m_fence->SetEventOnCompletion(m_fence_counter.at(m_frame_index), m_fence_event);
			WaitForSingleObjectEx(m_fence_event, INFINITE, false);
		}

		/* 次のフレームのフェンスカウンターを増やす */
		m_fence_counter.at(m_frame_index) = currentValue + 1;
	}

	void graphics::wait_gpu()
	{
		m_command_queue->Signal(m_fence.Get(), m_fence_counter.at(m_frame_index));

		m_fence->SetEventOnCompletion(m_fence_counter.at(m_frame_index), m_fence_event);

		WaitForSingleObjectEx(m_fence_event, INFINITE, false);

		++m_fence_counter.at(m_frame_index);
	}
}
