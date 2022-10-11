﻿#include "pch.hpp"
#include "graphic_d3d12.hpp"
#include "descriptor_heap.hpp"
#include "gpu_buffer.hpp"
#include "d3d12_factory.hpp"

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

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissor{};
}

graphic_d3d12::graphic_d3d12(const winapp& winapp)
	: m_winapp(winapp)
{

}

graphic_d3d12::~graphic_d3d12() noexcept(false)
{
	wait_gpu();

	if (m_fence_event != nullptr)
	{
		CloseHandle(m_fence_event);
		m_fence_event = nullptr;
	}
}

void graphic_d3d12::create_devices()
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

void graphic_d3d12::create_pipelines()
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
	std::array<D3D12_INPUT_ELEMENT_DESC, FRAME_COUNT> elements{};
	elements.at(0).SemanticName = "POSITION";
	elements.at(0).SemanticIndex = 0;
	elements.at(0).Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements.at(0).InputSlot = 0;
	elements.at(0).AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	elements.at(0).InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements.at(0).InstanceDataStepRate = 0;

	elements.at(1).SemanticName = "COLOR";
	elements.at(1).SemanticIndex = 0;
	elements.at(1).Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements.at(1).InputSlot = 0;
	elements.at(1).AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	elements.at(1).InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements.at(1).InstanceDataStepRate = 0;

	// ラスタライザーステート
	D3D12_RASTERIZER_DESC descRS{};
	descRS.FillMode = D3D12_FILL_MODE_SOLID;
	descRS.CullMode = D3D12_CULL_MODE_NONE;
	descRS.FrontCounterClockwise = false;
	descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descRS.DepthClipEnable = false;
	descRS.MultisampleEnable = false;
	descRS.AntialiasedLineEnable = false;
	descRS.ForcedSampleCount = 0;
	descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC descRTBS{
		false,false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	// ブレンドステートの設定
	D3D12_BLEND_DESC descBS{};
	descBS.AlphaToCoverageEnable = false;
	descBS.IndependentBlendEnable = false;
	for (auto i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		gsl::at(descBS.RenderTarget, i) = descRTBS;
	}

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
	desc_pipeline_state.VS = D3D12_SHADER_BYTECODE{ vs_blob->GetBufferPointer(),vs_blob->GetBufferSize() };
	desc_pipeline_state.PS = D3D12_SHADER_BYTECODE{ ps_blob->GetBufferPointer(),ps_blob->GetBufferSize() };
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

	// Setting Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(m_winapp.get_width());
	viewport.Height = static_cast<float>(m_winapp.get_height());
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	scissor.left = 0;
	scissor.right = m_winapp.get_width();
	scissor.top = 0;
	scissor.bottom = m_winapp.get_height();
}

void graphic_d3d12::render_begin()
{
	// start commandt
	m_command_allocator.at(m_frame_index)->Reset();
	m_command_list->Reset(m_command_allocator.at(m_frame_index).Get(), nullptr);

	resource_barrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

	const auto& aa = m_heap_rtv->at(m_frame_index).cpu_handle;
	m_command_list->OMSetRenderTargets(1, &aa, FALSE, nullptr);

	constexpr const std::array<float, 4> clear_color = { 0.125f,0.1f,0.1f,1.0f };

	m_command_list->ClearRenderTargetView(m_heap_rtv->at(m_frame_index).cpu_handle, clear_color.data(), 0, nullptr);
}

void graphic_d3d12::render_init()
{
	m_command_list->SetGraphicsRootSignature(m_root_signature.Get());
	m_command_list->SetPipelineState(m_pipeline.Get());
	m_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_command_list->RSSetViewports(1, &viewport);
	m_command_list->RSSetScissorRects(1, &scissor);
}

void graphic_d3d12::set_vertexbuffer_view(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	m_command_list->IASetVertexBuffers(0, 1, &vbv);

	m_draw_vertex_count = vbv.SizeInBytes / vbv.StrideInBytes;
}

void graphic_d3d12::set_constantbuffer(const gsl::not_null<descriptor_heap*> heap)
{
	m_command_list->SetDescriptorHeaps(1, heap->get_address());
	m_command_list->SetGraphicsRootConstantBufferView(0, heap->get_cbv_view_desc().BufferLocation);
}

void graphic_d3d12::render()
{
	m_command_list->DrawInstanced(m_draw_vertex_count, 1, 0, 0);
}

void graphic_d3d12::render_end()
{
	resource_barrier(D3D12_RESOURCE_STATE_PRESENT);

	// close command
	m_command_list->Close();

	// exture
	std::array<ID3D12CommandList*, 1> ppCmdLists = { m_command_list.Get() };
	m_command_queue->ExecuteCommandLists(1, ppCmdLists.data());
}

void graphic_d3d12::present()
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

void graphic_d3d12::wait_gpu()
{
	m_command_queue->Signal(m_fence.Get(), m_fence_counter.at(m_frame_index));

	m_fence->SetEventOnCompletion(m_fence_counter.at(m_frame_index), m_fence_event);

	WaitForSingleObjectEx(m_fence_event, INFINITE, false);

	++m_fence_counter.at(m_frame_index);
}

void graphic_d3d12::resource_barrier(const D3D12_RESOURCE_STATES state)
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
