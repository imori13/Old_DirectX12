#pragma once
#include <cstdint>
#include <memory>
#include <windows.h>

class winapp
{
public:
	winapp() = default;
	winapp(uint32_t width, uint32_t height);
	~winapp();

public:
	winapp(winapp&&) = default;
	winapp& operator=(winapp&&) = default;

public:
	winapp(const winapp&) = delete;
	winapp& operator=(const winapp&) = delete;

public:
	void update() noexcept;

public:
	bool isloop() const noexcept;
	inline uint32_t get_width() const noexcept { return m_width; }
	inline uint32_t get_height() const noexcept { return m_height; }
	inline HINSTANCE get_hinstance() const noexcept { return m_hinst; }
	inline HWND get_hwnd() const noexcept { return m_hwnd; }

private:
	uint32_t m_width;
	uint32_t m_height;

	HINSTANCE m_hinst;
	HWND m_hwnd;
	MSG m_msg;
};