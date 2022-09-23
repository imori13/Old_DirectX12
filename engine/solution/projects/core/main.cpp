
int32_t main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const auto& app = std::make_unique<winapp>(800, 450);
	const auto& d3d12 = std::make_unique<graphic_d3d12>(*app.get());

	while(app->isloop())
	{
		app->update();
		d3d12->update();
		d3d12->render();
	}

	return 0;
}