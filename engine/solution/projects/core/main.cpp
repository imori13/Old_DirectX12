#include "include.hpp"

int32_t main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const auto& app = std::make_unique<winapp>(800, 450);
	const auto& d3d12 = std::make_unique<graphic_d3d12>(*app);

	d3d12->create_devices();
	d3d12->create_pipelines();

	std::unique_ptr<descriptor_heap> m_heap_cbv;
	std::unique_ptr<gpu_buffer<vertex>> m_vertex_buffer;
	std::unique_ptr<gpu_buffer<uint32_t>> index_buffer;
	std::array< std::unique_ptr<gpu_buffer<_transform>>, FRAME_COUNT> m_constant_buffers;

	HRESULT hr{};
	const auto& device = d3d12->get_device();

	std::vector<vertex> vertices =
	{
		vertex{ vector3(-1, 1, 0), vector4(1, 0, 0, 1) },
		vertex{ vector3(1, 1, 0), vector4(0, 1, 0, 1) },
		vertex{ vector3(1,-1, 0), vector4(1, 0, 1, 1) },
		vertex{ vector3(-1,-1, 0), vector4(1, 0, 1, 1) },
	};

	std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3 };

	m_vertex_buffer = std::make_unique<gpu_buffer<vertex>>(device, vertices.size() * sizeof(vertex));
	m_vertex_buffer->map(vertices);

	index_buffer = std::make_unique<gpu_buffer<uint32_t>>(device, indices.size() * sizeof(uint32_t));
	index_buffer->map(indices);

	_transform _trans{};
	for (auto& buffer : m_constant_buffers)
	{
		buffer = std::make_unique<gpu_buffer<_transform>>(device, sizeof(_transform));
		buffer->map(_trans);
	}

	// bind heap
	m_heap_cbv = std::make_unique<descriptor_heap>(device, FRAME_COUNT, heap_type::cbv_srv_uav, heap_flag::shader_visible);
	for (auto& buffer : m_constant_buffers)
		m_heap_cbv->create_cbv(buffer->get_resource());

	const auto& eye_pos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	const auto& target_pos = DirectX::XMVectorZero();
	const auto& up_ward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const auto& fov_y = DirectX::XMConvertToRadians(37.5f);
	const auto& aspect = static_cast<float>(app->get_width()) / static_cast<float>(app->get_height());

	for (auto& buffer : m_constant_buffers)
	{
		buffer->data()->world = DirectX::XMMatrixIdentity();
		buffer->data()->view = DirectX::XMMatrixLookAtRH(eye_pos, target_pos, up_ward);
		buffer->data()->proj = DirectX::XMMatrixPerspectiveFovRH(fov_y, aspect, 1.0f, 1000.0f);
	}

	const auto& vbv = m_vertex_buffer->get_vertex_buffer_view();
	const auto& ibv = index_buffer->get_index_buffer_view();

	while (app->isloop())
	{
		/*  更新処理  */
		{
			app->update();

			static float s_angle = 0.f;

			s_angle += 0.01f;

			auto& buffer = m_constant_buffers.at(d3d12->get_frame_index());
			buffer->data()->world = DirectX::XMMatrixIdentity() * DirectX::XMMatrixRotationZ(s_angle);
		}

		d3d12->render_begin();
		d3d12->render_init();
		d3d12->set_constantbuffer(m_heap_cbv.get());
		d3d12->render(vbv, ibv);
		d3d12->render_end();
		d3d12->present();
	}

	d3d12->wait_gpu();

	return 0;
}