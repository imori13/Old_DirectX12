#include "pch.hpp"
#include "descriptor_heap.hpp"

using namespace Microsoft::WRL;

descriptor_heap::descriptor_heap(gsl::not_null<ID3D12Device*> device, uint32_t descriptor_count, heap_type heap_type, heap_flag heap_flag)
	: m_descriptor_count(descriptor_count)
	, m_descriptors(descriptor_count)
	, m_device(device)
	, m_empty_header(0)
	, m_heap_handle{}
{
	const auto& _heap_type = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(heap_type);

	// setting descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = descriptor_count;
	desc.Type = _heap_type;
	desc.Flags = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(heap_flag);
	desc.NodeMask = 0;

	// create descriptor heap
	const auto& hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_heap.GetAddressOf()));
	Ensures(SUCCEEDED(hr));

	m_incriment_size = device->GetDescriptorHandleIncrementSize(_heap_type);
	m_heap_handle.cpu_handle = m_heap->GetCPUDescriptorHandleForHeapStart();

	if(heap_flag == heap_flag::shader_visible)
	{
		m_heap_handle.gpu_handle = m_heap->GetGPUDescriptorHandleForHeapStart();
	}

	for(auto i = 0; i < m_descriptors.size(); ++i)
	{
		auto& handle = m_descriptors.at(i);
		handle.cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE{ m_heap_handle.cpu_handle.ptr + m_incriment_size * i };

		if(heap_flag != heap_flag::shader_visible) continue;

		handle.gpu_handle = D3D12_GPU_DESCRIPTOR_HANDLE{ m_heap_handle.gpu_handle.ptr + m_incriment_size * i };
	}
}