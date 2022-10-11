#pragma once

enum class heap_type : uint8_t
{
	cbv_srv_uav = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	sampler = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
	rtv = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
	dsv = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
};

enum class heap_flag : uint8_t
{
	none = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
	shader_visible = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
};

struct descriptor_handle
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
};

class descriptor_heap
{
public:
	descriptor_heap() = default;
	descriptor_heap(gsl::not_null<ID3D12Device*> pDevice, uint32_t descriptor_count, heap_type heap_type, heap_flag heap_flag = heap_flag::none);

public:
	inline descriptor_handle at(size_t index) const noexcept { return m_descriptors.at(index); }

	inline void create_rtv(gsl::not_null<ID3D12Resource*> resource)
	{
		D3D12_RENDER_TARGET_VIEW_DESC view_desc{};
		view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MipSlice = 0;
		view_desc.Texture2D.PlaneSlice = 0;

		m_device->CreateRenderTargetView(resource, &view_desc, at(m_empty_header).cpu_handle);

		this->next();
	}

	inline void create_cbv(gsl::not_null<ID3D12Resource*> resource)
	{
		cbv_view_desc.BufferLocation = resource->GetGPUVirtualAddress();
		cbv_view_desc.SizeInBytes = gsl::narrow<UINT>(resource->GetDesc().Width);

		m_device->CreateConstantBufferView(&cbv_view_desc, at(m_empty_header).cpu_handle);

		this->next();
	}

	inline ID3D12DescriptorHeap** get_address() noexcept
	{
		return m_heap.GetAddressOf();
	}

	inline D3D12_CONSTANT_BUFFER_VIEW_DESC get_cbv_view_desc() noexcept
	{
		return cbv_view_desc;
	}

private:
	inline void next() noexcept
	{
		Expects(this != nullptr);
		Expects(0 <= m_empty_header && m_empty_header + 1 <= m_descriptor_count);

		++m_empty_header;
	}

private:
	gsl::not_null<ID3D12Device*> m_device;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
	std::vector<descriptor_handle> m_descriptors;
	descriptor_handle m_heap_handle;

private:
	uint64_t m_incriment_size;
	uint32_t m_descriptor_count;
	uint32_t m_empty_header;

private:
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view_desc;
};