#pragma once

namespace
{
	constexpr uint8_t VEC4_X = 0u;
	constexpr uint8_t VEC4_Y = 1u;
	constexpr uint8_t VEC4_Z = 2u;
	constexpr uint8_t VEC4_W = 3u;
	constexpr uint8_t VEC4_COUNT = 4u;
}

namespace math
{
	template<typename T> class basic_vector4
	{
	public:
		explicit inline constexpr basic_vector4() noexcept;
		explicit inline constexpr basic_vector4(T value);
		explicit inline constexpr basic_vector4(T x, T y, T z, T w);

	public:
		inline constexpr T* data();
		inline constexpr T& x();
		inline constexpr T& y();
		inline constexpr T& z();
		inline constexpr T& w();

	public:
		inline constexpr const T* data() const;
		inline constexpr const T& x() const;
		inline constexpr const T& y() const;
		inline constexpr const T& z() const;
		inline constexpr const T& w() const;

	public:
		inline constexpr basic_vector4& operator+=(const basic_vector4& other);
		inline constexpr basic_vector4& operator-=(const basic_vector4& other);
		inline constexpr basic_vector4& operator*=(float value);
		inline constexpr basic_vector4& operator/=(float value);

	public:
		inline constexpr basic_vector4 operator+(const basic_vector4& other) const;
		inline constexpr basic_vector4 operator-(const basic_vector4& other) const;
		inline constexpr basic_vector4 operator*(float value) const;
		inline constexpr basic_vector4 operator/(float value) const;

	public:
		inline constexpr void normalize();

	public:
		inline constexpr T length_square() const;
		inline constexpr T length() const;
		inline constexpr basic_vector4 normalized() const;
		inline constexpr float dot(const basic_vector4& other) const;

	private:
		std::array<T, VEC4_COUNT> _;
	};

	/*  -----  usingêÈåæ  -----------------------------------  */
	
	using vector4 = basic_vector4<float>;
	using vector4f = basic_vector4<float>;
	using vector4d = basic_vector4<double>;
	using vector4i = basic_vector4<int32_t>;
	using vector4u = basic_vector4<uint32_t>;

	/*  -----  inlineíËã`  -----------------------------------  */

	template<typename T> inline constexpr basic_vector4<T>::basic_vector4() noexcept : basic_vector4(0) {}
	template<typename T> inline constexpr basic_vector4<T>::basic_vector4(T value) : basic_vector4(value, value, value, value) {}
	template<typename T> inline constexpr basic_vector4<T>::basic_vector4(T x, T y, T z, T w) : _{ x,y,z,w } {}

	template<typename T> inline constexpr T* basic_vector4<T>::data() { return _.data(); }
	template<typename T> inline constexpr T& basic_vector4<T>::x() { return _.at(VEC4_X); }
	template<typename T> inline constexpr T& basic_vector4<T>::y() { return _.at(VEC4_Y); }
	template<typename T> inline constexpr T& basic_vector4<T>::z() { return _.at(VEC4_Z); }
	template<typename T> inline constexpr T& basic_vector4<T>::w() { return _.at(VEC4_W); }

	template<typename T> inline constexpr const T* basic_vector4<T>::data() const { return _.data(); }
	template<typename T> inline constexpr const T& basic_vector4<T>::x() const { return _.at(VEC4_X); }
	template<typename T> inline constexpr const T& basic_vector4<T>::y() const { return _.at(VEC4_Y); }
	template<typename T> inline constexpr const T& basic_vector4<T>::z() const { return _.at(VEC4_Z); }
	template<typename T> inline constexpr const T& basic_vector4<T>::w() const { return _.at(VEC4_W); }

	template<typename T> inline constexpr basic_vector4<T>& basic_vector4<T>::operator+=(const basic_vector4& other)
	{
		x() += other.x();
		y() += other.y();
		z() += other.z();
		w() += other.w();
		return *this;
	}

	template<typename T> inline constexpr basic_vector4<T>& basic_vector4<T>::operator-=(const basic_vector4& other)
	{
		x() -= other.x();
		y() -= other.y();
		z() -= other.z();
		w() -= other.w();
		return *this;
	}

	template<typename T> inline constexpr basic_vector4<T>& basic_vector4<T>::operator*=(float value)
	{
		x() *= value;
		y() *= value;
		z() *= value;
		w() *= value;
		return *this;
	}

	template<typename T> inline constexpr basic_vector4<T>& basic_vector4<T>::operator/=(float value)
	{
		return *this *= 1.0f / value;
	}

	template<typename T> inline constexpr basic_vector4<T> basic_vector4<T>::operator+(const basic_vector4& other) const { return basic_vector4(x() + other.x(), y() + other.y(), z() + other.z(), w() + other.w()); }
	template<typename T> inline constexpr basic_vector4<T> basic_vector4<T>::operator-(const basic_vector4& other) const { return basic_vector4(x() - other.x(), y() - other.y(), z() - other.z(), w() - other.w()); }
	template<typename T> inline constexpr basic_vector4<T> basic_vector4<T>::operator*(float value) const { return basic_vector4(x() * value, y() * value, z() * value, w() * value); }
	template<typename T> inline constexpr basic_vector4<T> basic_vector4<T>::operator/(float value) const { return *this * 1.0f / value; }

	template<typename T> inline constexpr void basic_vector4<T>::normalize()
	{
		*this /= length_square();
	}

	template<typename T> inline constexpr T basic_vector4<T>::length_square() const
	{
		return x() * x() + y() * y() + z() * z() + w() * w();
	}

	template<typename T> inline constexpr T basic_vector4<T>::length() const
	{
		return math::sqrt(this->length_square());
	}

	template<typename T> inline constexpr basic_vector4<T> basic_vector4<T>::normalized() const
	{
		basic_vector4 copy = *this;
		copy.normalize();
		return copy;
	}

	template<typename T> inline constexpr float basic_vector4<T>::dot(const basic_vector4& other) const
	{
		return x() * other.x() + y() * other.y() + z() * other.z() + w() * other.w();
	}
}