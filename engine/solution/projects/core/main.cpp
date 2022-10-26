#include "include.hpp"

int32_t main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const auto& app = std::make_unique<winapp>(800, 450);
	const auto& d3d12 = graphic_d3d12::create(app.get());

	//d3d12->create_devices();
	//d3d12->create_pipelines();

	//std::unique_ptr<descriptor_heap> m_heap_cbv;
	//std::unique_ptr<gpu_buffer<vertex>> m_vertex_buffer;
	//std::unique_ptr<gpu_buffer<uint32_t>> index_buffer;
	//std::unique_ptr<gpu_buffer<DirectX::XMMATRIX>> vertex_stream;
	//std::array< std::unique_ptr<gpu_buffer<camera_mat>>, FRAME_COUNT> m_constant_buffers;

	//const auto& device = d3d12->get_device();

	//std::vector<vertex> vertices =
	//{
	//	vertex{ vector3(-1, 1, 0), vector4(1, 0, 0, 1) },
	//	vertex{ vector3(1, 1, 0), vector4(0, 1, 0, 1) },
	//	vertex{ vector3(1,-1, 0), vector4(1, 0, 1, 1) },
	//	vertex{ vector3(-1,-1, 0), vector4(1, 0, 1, 1) },
	//};

	//std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3 };

	//std::vector<DirectX::XMMATRIX> aaaaa =
	//{
	//	DirectX::XMMatrixTranslation(-1,1,0),
	//	DirectX::XMMatrixTranslation(0,0,0),
	//	DirectX::XMMatrixTranslation(1,0,0),
	//};

	//aaaaa.resize(100);

	//m_vertex_buffer = std::make_unique<gpu_buffer<vertex>>(device, vertices.size() * sizeof(vertex));
	//m_vertex_buffer->map(vertices);

	//index_buffer = std::make_unique<gpu_buffer<uint32_t>>(device, indices.size() * sizeof(uint32_t));
	//index_buffer->map(indices);

	//vertex_stream = std::make_unique<gpu_buffer<DirectX::XMMATRIX>>(device, aaaaa.size() * sizeof(DirectX::XMMATRIX));
	//vertex_stream->map(aaaaa);

	//const auto& eye_pos = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	//const auto& dir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//const auto& up_ward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//const auto& fov_y = DirectX::XMConvertToRadians(90);
	//const auto& aspect = static_cast<float>(app->get_width()) / static_cast<float>(app->get_height());

	//camera_mat _trans{};
	//for (auto& buffer : m_constant_buffers)
	//{
	//	buffer = std::make_unique<gpu_buffer<camera_mat>>(device, sizeof(camera_mat));
	//	buffer->map(_trans);

	//	buffer->data()->view = DirectX::XMMatrixLookToLH(eye_pos, dir, up_ward);
	//	buffer->data()->proj = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, 0.0001f, 100);
	//}

	//// bind heap
	//m_heap_cbv = std::make_unique<descriptor_heap>(device, FRAME_COUNT, heap_type::cbv_srv_uav, heap_flag::shader_visible);
	//for (auto& buffer : m_constant_buffers)
	//	m_heap_cbv->create_cbv(buffer->get_resource());


	//const auto& vbv = m_vertex_buffer->get_vertex_buffer_view();
	//const auto& ibv = index_buffer->get_index_buffer_view();
	//const auto& stream_view = vertex_stream->get_vertex_buffer_view();

	//std::array<D3D12_VERTEX_BUFFER_VIEW, 2u> views = { vbv, stream_view };

	//while (app->isloop())
	//{
	//	/*  更新処理  */
	//	app->update();

	//	auto data = vertex_stream->data().get();
	//	for (auto i = 0; i < aaaaa.size(); ++i)
	//	{
	//		data[i] = DirectX::XMMatrixTranslation(rand() % 1000 / 100.f, rand() % 1000 / 100.f, 0);
	//	}

	//	d3d12->render_begin();
	//	d3d12->render_init();
	//	d3d12->set_constantbuffer(m_heap_cbv.get());
	//	//d3d12->render(vbv, ibv);
	//	d3d12->render(views, ibv);
	//	d3d12->render_end();
	//	d3d12->present();
	//}

	//d3d12->wait_gpu();

	return 0;
}