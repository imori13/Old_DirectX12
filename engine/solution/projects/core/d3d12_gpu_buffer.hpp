#pragma once
#include "vertex.hpp"

template<class T> class gpu_buffer
{
public:
	gpu_buffer() noexcept
		: m_buffer_size(0)
		, m_resource(nullptr)
		, m_ptr(nullptr)
	{
	}

	~gpu_buffer()
	{
		unmap();
	}

	gpu_buffer(gsl::not_null<ID3D12Device*> pDevice, size_t buffer_size)
		: m_buffer_size(buffer_size)
		, m_resource(nullptr)
		, m_ptr(nullptr)
	{
		/*  ヒープ設定  */
		const auto& prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		/*  リソース設定  */
		const auto& desc = CD3DX12_RESOURCE_DESC::Buffer(m_buffer_size);

		// create resource
		const auto& hr = pDevice->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_resource.GetAddressOf())
		);
		Ensures(SUCCEEDED(hr));
	}

public:
	inline void map(const gsl::span<T> span)
	{
		/*  マップに失敗したか  */
		Ensures(SUCCEEDED(m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_ptr))));

		/*  マップするデータとサイズは同じか  */
		Expects(span.size_bytes() == m_buffer_size);

		/*  指定されたデータのメモリをコピーする  */
		memcpy_s(m_ptr, m_buffer_size, span.data(), m_buffer_size);
	}

	inline void map(const T& value)
	{
		/*  マップに失敗したか  */
		Ensures(SUCCEEDED(m_resource->Map(0, nullptr, reinterpret_cast<void**>(&m_ptr))));

		/*  マップするデータとサイズは同じか  */
		Expects(sizeof(value) == m_buffer_size);

		/*  指定されたデータのメモリをコピーする  */
		memcpy_s(m_ptr, m_buffer_size, &value, m_buffer_size);
	}

	inline void unmap() const noexcept
	{
		m_resource->Unmap(0, nullptr);
	}

	inline gsl::not_null<T*> data() { return m_ptr; }

	/*  頂点バッファビューを取得  */
	inline D3D12_VERTEX_BUFFER_VIEW get_vertex_buffer_view() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = m_resource->GetGPUVirtualAddress();
		vbv.SizeInBytes = gsl::narrow<UINT>(m_buffer_size);
		vbv.StrideInBytes = gsl::narrow<UINT>(sizeof(T));
		return vbv;
	}

	/*  インデックスバッファビューを取得  */
	inline D3D12_INDEX_BUFFER_VIEW get_index_buffer_view() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv{};
		ibv.BufferLocation = m_resource->GetGPUVirtualAddress();
		ibv.SizeInBytes = gsl::narrow<UINT>(m_buffer_size);
		ibv.Format = DXGI_FORMAT_R32_UINT;
		return ibv;
	}

public:
	inline gsl::not_null<ID3D12Resource*> get_resource() { return m_resource.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
	T* m_ptr;

	size_t m_buffer_size;
};