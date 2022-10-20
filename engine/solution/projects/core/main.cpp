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

	std::unique_ptr<gpu_buffer<basic_vertex>> m_vertex_buffer;
	std::unique_ptr<gpu_buffer<uint32_t>> index_buffer;
	std::unique_ptr<gpu_buffer<DirectX::XMMATRIX>> vertex_stream;

	const auto& device = d3d12->get_device();

	std::vector<basic_vertex> vertices =
	{
		{ vector3(-1, 1, 0), vector4(1, 0, 0, 1) },
		{ vector3(1, 1, 0), vector4(0, 1, 0, 1) },
		{ vector3(1,-1, 0), vector4(1, 0, 1, 1) },
		{ vector3(-1,-1, 0), vector4(1, 0, 1, 1) },
	};

	std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3 };

	std::vector<DirectX::XMMATRIX> aaaaa =
	{
		DirectX::XMMatrixTranslation(-1,1,0),
		DirectX::XMMatrixTranslation(0,0,0),
		DirectX::XMMatrixTranslation(1,0,0),
	};

	aaaaa.resize(100);

	m_vertex_buffer = std::make_unique<gpu_buffer<basic_vertex>>(device, vertices.size() * sizeof(basic_vertex));
	m_vertex_buffer->map(vertices);

	index_buffer = std::make_unique<gpu_buffer<uint32_t>>(device, indices.size() * sizeof(uint32_t));
	index_buffer->map(indices);

	vertex_stream = std::make_unique<gpu_buffer<DirectX::XMMATRIX>>(device, aaaaa.size() * sizeof(DirectX::XMMATRIX));
	vertex_stream->map(aaaaa);

	using namespace module;

	graphics::create();

	const auto& graphicsmodule = graphics::get();

	graphicsmodule->set_stream(vertex_stream->get_vertex_buffer_view());

	while (app->isloop())
	{
		/*  更新処理  */
		app->update();

		auto data = vertex_stream->data().get();
		for (auto i = 0; i < aaaaa.size(); ++i)
		{
			data[i] = DirectX::XMMatrixTranslation(rand() % 1000 / 100.f, rand() % 1000 / 100.f, 0);
		}

		graphicsmodule->begin();
		graphicsmodule->set_vertices(m_vertex_buffer->get_vertex_buffer_view(), index_buffer->get_index_buffer_view());
		graphicsmodule->draw_call(6, 100);
		graphicsmodule->end();

		//d3d12->render_begin();
		//d3d12->render_init();
		//d3d12->set_constantbuffer(m_heap_cbv.get());
		////d3d12->render(vbv, ibv);
		//d3d12->render(views, ibv);
		//d3d12->render_end();
		//d3d12->present();
	}

	d3d12->wait_gpu();

	return 0;
}