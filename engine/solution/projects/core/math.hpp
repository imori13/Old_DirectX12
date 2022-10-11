#pragma once

namespace
{
	inline constexpr uint32_t LOOP_COUNT = 10u;
}

namespace math
{
	inline constexpr float pi = 3.1415927f;

	inline constexpr float sin(float x);
	inline constexpr float cos(float x);
	inline constexpr float tan(float x);
	inline constexpr float sqrt(float x);
}

namespace math
{
	inline constexpr float sin(float x)
	{
		constexpr float p = (2.0f * pi);
		x -= gsl::narrow_cast<uint32_t>(x / p) * p; // -2π～2πにする

		float sum = x, t = x;
		for(int i = 1; i <= LOOP_COUNT; i++)
		{
			t *= -(x * x) / ((2 * i + 1) * (2 * i));
			sum += t;
		}
		return sum;
	}

	inline constexpr float cos(float x)
	{
		return math::sin(pi / 2.0f - x);
	}

	inline constexpr float tan(float x)
	{
		return math::sin(x) / math::cos(x);
	}

	inline constexpr float sqrt(float x)
	{
		if(x <= 0) return 0;
		float t = x;
		for(int i = 1; i <= LOOP_COUNT; i++) t = (t + x / t) / 2.0f;
		return t;
	}
}