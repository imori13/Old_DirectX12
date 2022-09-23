#pragma once

namespace
{
	constexpr uint8_t VEC2_X = 0u;
	constexpr uint8_t VEC2_Y = 1u;
	constexpr uint8_t VEC2_COUNT = 2u;
}

namespace math
{
	template<typename T> class basic_vector2
	{
	public:
		explicit inline constexpr basic_vector2();
		explicit inline constexpr basic_vector2(T value);
		explicit inline constexpr basic_vector2(T x, T y);

	public:
		inline constexpr T* data();
		inline constexpr T& x();
		inline constexpr T& y();

	public:
		inline constexpr const T* data() const;
		inline constexpr const T& x() const;
		inline constexpr const T& y() const;

	public:
		inline constexpr basic_vector2& operator+=(const basic_vector2& other);
		inline constexpr basic_vector2& operator-=(const basic_vector2& other);
		inline constexpr basic_vector2& operator*=(float value);
		inline constexpr basic_vector2& operator/=(float value);

	public:
		inline constexpr basic_vector2 operator+(const basic_vector2& other) const;
		inline constexpr basic_vector2 operator-(const basic_vector2& other) const;
		inline constexpr basic_vector2 operator*(float value) const;
		inline constexpr basic_vector2 operator/(float value) const;

	public:
		inline constexpr void normalize();

	public:
		inline constexpr T length_square() const;
		inline constexpr T length() const;
		inline constexpr basic_vector2 normalized() const;
		inline constexpr float dot(basic_vector2 other) const;
		inline constexpr float cross(basic_vector2 other) const;

	private:
		std::array<T, VEC2_COUNT> _;
	};

	/*  -----  usingêÈåæ  -----------------------------------  */

	using vector2 = basic_vector2<float>;
	using vector2f = basic_vector2<float>;
	using vector2d = basic_vector2<double>;
	using vector2i = basic_vector2<int32_t>;
	using vector2u = basic_vector2<uint32_t>;

	/*  -----  inlineíËã`  -----------------------------------  */

	template<typename T> inline constexpr basic_vector2<T>::basic_vector2() : basic_vector2(0) {}
	template<typename T> inline constexpr basic_vector2<T>::basic_vector2(T value) : basic_vector2(value, value) {}
	template<typename T> inline constexpr basic_vector2<T>::basic_vector2(T x, T y) : _{x,y} {}

	template<typename T> inline constexpr T* basic_vector2<T>::data() { return _.data(); }
	template<typename T> inline constexpr T& basic_vector2<T>::x() { return _.at(VEC2_X); }
	template<typename T> inline constexpr T& basic_vector2<T>::y() { return _.at(VEC2_Y); }

	template<typename T> inline constexpr const T* basic_vector2<T>::data() const { return _.data(); }
	template<typename T> inline constexpr const T& basic_vector2<T>::x() const { return _.at(VEC2_X); }
	template<typename T> inline constexpr const T& basic_vector2<T>::y() const { return _.at(VEC2_Y); }

	template<typename T> inline constexpr basic_vector2<T>& basic_vector2<T>::operator+=(const basic_vector2& other)
	{
		x() += other.x();
		y() += other.y();
		return *this;
	}

	template<typename T> inline constexpr basic_vector2<T>& basic_vector2<T>::operator-=(const basic_vector2& other)
	{
		x() -= other.x();
		y() -= other.y();
		return *this;
	}

	template<typename T> inline constexpr basic_vector2<T>& basic_vector2<T>::operator*=(float value)
	{
		x() *= value;
		y() *= value;
		return *this;
	}

	template<typename T> inline constexpr basic_vector2<T>& basic_vector2<T>::operator/=(float value)
	{
		return *this *= 1.0f / value;
	}

	template<typename T> inline constexpr basic_vector2<T> basic_vector2<T>::operator+(const basic_vector2& other) const { return basic_vector2(x() + other.x(), y() + other.y()); }
	template<typename T> inline constexpr basic_vector2<T> basic_vector2<T>::operator-(const basic_vector2& other) const { return basic_vector2(x() - other.x(), y() - other.y()); }
	template<typename T> inline constexpr basic_vector2<T> basic_vector2<T>::operator*(float value) const { return basic_vector2(x() * value, y() * value); }
	template<typename T> inline constexpr basic_vector2<T> basic_vector2<T>::operator/(float value) const { return *this * 1.0f / value; }

	template<typename T> inline constexpr void basic_vector2<T>::normalize()
	{
		*this /= length_square();
	}

	template<typename T> inline constexpr T basic_vector2<T>::length_square() const
	{
		return x() * x() + y() * y();
	}

	template<typename T> inline constexpr T basic_vector2<T>::length() const
	{
		return math::sqrt(this->length_square());
	}

	template<typename T> inline constexpr basic_vector2<T> basic_vector2<T>::normalized() const
	{
		basic_vector2 copy = *this;
		copy.normalize();
		return copy;
	}

	template<typename T> inline constexpr float basic_vector2<T>::dot(basic_vector2 other) const
	{
		return x() * other.x() + y() * other.y();
	}

	template<typename T> inline constexpr float basic_vector2<T>::cross(basic_vector2 other) const
	{
		return x() * other.y() - other.x() * y();
	}
}