
int32_t main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const auto& app = std::make_unique<winapp>(800, 450);
	const auto& d3d12 = std::make_unique<graphic_d3d12>(*app.get());

	d3d12->create_devices();
	d3d12->create_pipelines();
	//d3d12->create_render_objects();

	std::unique_ptr<descriptor_heap> m_heap_cbv;
	gpu_buffer<vertex> m_vertex_buffer;
	std::array<gpu_buffer<_transform>, FRAME_COUNT> m_constant_buffers;

	HRESULT hr{};
	const auto& device = d3d12->get_device();

	std::vector<vertex> vertices;

	constexpr float count = 500;
	for (auto i = 0; i < count; ++i)
	{
		static const auto& func = [](const uint32_t index, const uint32_t count) -> vector3
		{
			const auto& rad = 360.0f * index / count * math::to_rad;
			return vector3(math::cos(rad), math::sin(rad), 0);
		};

		const float length = rand() % 1000 / 1000.f;

		vertices.push_back(vertex{ vector3::zero(), vector4::one()});
		vertices.push_back(vertex{ func(i,count) * length, vector4::one() });
		vertices.push_back(vertex{ func(i + 1,count) * length, vector4::one() });
	}

	m_vertex_buffer = gpu_buffer<vertex>(device, vertices.size() * sizeof(vertex));

	m_vertex_buffer.map(vertices);

	_transform _trans{};

	// create constant buffer
	for (auto i = 0; i < m_constant_buffers.size(); ++i)
	{
		m_constant_buffers.at(i) = gpu_buffer<_transform>(device, sizeof(_transform));
		m_constant_buffers.at(i).map(gsl::make_span<_transform>(&_trans, 1));
	}

	// bind heap
	m_heap_cbv = std::make_unique<descriptor_heap>(device, FRAME_COUNT, heap_type::cbv_srv_uav, heap_flag::shader_visible);

	for (auto i = 0; i < m_constant_buffers.size(); ++i)
	{
		m_heap_cbv->create_cbv(m_constant_buffers.at(i).get_resource());
	}

	const auto& eye_pos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
	const auto& target_pos = DirectX::XMVectorZero();
	const auto& up_ward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	const auto& fov_y = DirectX::XMConvertToRadians(37.5f);
	const auto& aspect = static_cast<float>(app->get_width()) / static_cast<float>(app->get_height());

	for (auto& buffer : m_constant_buffers)
	{
		buffer.get()->world = DirectX::XMMatrixIdentity();
		buffer.get()->view = DirectX::XMMatrixLookAtRH(eye_pos, target_pos, up_ward);
		buffer.get()->proj = DirectX::XMMatrixPerspectiveFovRH(fov_y, aspect, 1.0f, 1000.0f);
	}

	const auto& vbv = m_vertex_buffer.get_vertex_buffer_view();

	while (app->isloop())
	{
		/*  更新処理  */
		{
			app->update();

			static float s_angle = 0.f;

			s_angle += 0.01f;

			auto& buffer = m_constant_buffers.at(d3d12->get_frame_index());
			buffer.get()->world = DirectX::XMMatrixIdentity() * DirectX::XMMatrixRotationZ(s_angle);
		}

		d3d12->render_begin();
		d3d12->render_init();
		d3d12->set_vertexbuffer_view(vbv);
		d3d12->set_constantbuffer(m_heap_cbv.get());
		d3d12->render();
		d3d12->render_end();
		d3d12->present();
	}

	d3d12->wait_gpu();

	return 0;
}