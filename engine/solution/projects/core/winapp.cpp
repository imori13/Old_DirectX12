#include "pch.hpp"
#include "winapp.hpp"

namespace
{
	LRESULT CALLBACK window_proc(HWND hwnd, uint32_t msg, WPARAM wp, LPARAM lp) noexcept
	{
		switch(msg)
		{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;

			default:
				break;
		}

		return DefWindowProc(hwnd, msg, wp, lp);
	}

	constexpr auto s_class_name = TEXT("imori_engine");
	constexpr auto s_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
}

winapp::winapp(uint32_t width, uint32_t height)
	: m_width(width)
	, m_height(height)
	, m_hinst(nullptr)
	, m_hwnd(nullptr)
	, m_msg{}
{
	auto& hinst = m_hinst;
	auto& hwnd = m_hwnd;

	hinst = GetModuleHandle(nullptr);

	WNDCLASSEX window_class{};
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = window_proc;
	window_class.hIcon = LoadIcon(hinst, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(hinst, IDC_ARROW);
	window_class.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	window_class.lpszMenuName = nullptr;
	window_class.lpszClassName = s_class_name;
	window_class.hIconSm = LoadIcon(hinst, IDI_APPLICATION);

	RegisterClassEx(&window_class);

	RECT rc = RECT{ 0, 0, gsl::narrow<LONG>(width), gsl::narrow<LONG>(height) };

	AdjustWindowRect(&rc, s_style, false);

	hwnd = CreateWindowEx(
		0,
		s_class_name,
		s_class_name,
		s_style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hinst,
		nullptr
	);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);
	SetFocus(hwnd);
}
winapp::~winapp()
{
	if(m_hinst != nullptr)
	{
		UnregisterClass(s_class_name, m_hinst);
	}
}

void winapp::update() noexcept
{
	if(PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}
}