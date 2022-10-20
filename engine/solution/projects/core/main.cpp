#include "include.hpp"

int32_t main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const auto& app = std::make_unique<winapp>(800, 450);

	module::graphics_createarg arg{};
	arg.render_width = app->get_width();
	arg.render_height = app->get_height();
	arg.hwnd = app->get_hwnd();
	arg.frame_count = FRAME_COUNT;
	module::graphics::create(arg);

	std::unique_ptr<gpu_buffer<basic_vertex>> m_vertex_buffer;
	std::unique_ptr<gpu_buffer<uint32_t>> index_buffer;
	std::unique_ptr<gpu_buffer<DirectX::XMMATRIX>> vertex_stream;

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

	const auto& device = module::graphics::get()->get_device();
	m_vertex_buffer = std::make_unique<gpu_buffer<basic_vertex>>(device, vertices.size() * sizeof(basic_vertex));
	m_vertex_buffer->map(vertices);

	index_buffer = std::make_unique<gpu_buffer<uint32_t>>(device, indices.size() * sizeof(uint32_t));
	index_buffer->map(indices);

	vertex_stream = std::make_unique<gpu_buffer<DirectX::XMMATRIX>>(device, aaaaa.size() * sizeof(DirectX::XMMATRIX));
	vertex_stream->map(aaaaa);


	const auto& graphicsmodule = module::graphics::get();

	graphicsmodule->set_stream(vertex_stream->get_vbv());

	while (app->isloop())
	{
		/*  更新処理  */
		app->update();

		auto data = vertex_stream->data().get();
		for (auto i = 0; i < aaaaa.size(); ++i)
		{
			data[i] = DirectX::XMMatrixTranslation(rand() % 1000 / 100.f, rand() % 1000 / 100.f, 0);
		}

		std::array<D3D12_VERTEX_BUFFER_VIEW, 2> a = { m_vertex_buffer->get_vbv(), vertex_stream->get_vbv() };

		graphicsmodule->begin();
		graphicsmodule->set_vertices(m_vertex_buffer->get_vbv(), index_buffer->get_ibv());
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

	module::graphics::get()->wait_gpu();
	//d3d12->wait_gpu();

	return 0;
}