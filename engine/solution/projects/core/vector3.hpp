#pragma once

namespace
{
	inline constexpr uint8_t VEC3_X = 0u;
	inline constexpr uint8_t VEC3_Y = 1u;
	inline constexpr uint8_t VEC3_Z = 2u;
	inline constexpr uint8_t VEC3_COUNT = 3u;
}

namespace math
{
	template<typename T> class basic_vector3
	{
	public:
		explicit inline constexpr basic_vector3() noexcept;
		explicit inline constexpr basic_vector3(T value) noexcept;
		explicit inline constexpr basic_vector3(T x, T y, T z) noexcept;

	public:
		inline constexpr T* data() noexcept;
		inline constexpr T& x() noexcept;
		inline constexpr T& y() noexcept;
		inline constexpr T& z() noexcept;
		inline constexpr T& at(const size_t index);

	public:
		inline constexpr const T* data() const noexcept;
		inline constexpr const T& x() const noexcept;
		inline constexpr const T& y() const noexcept;
		inline constexpr const T& z() const noexcept;
		inline constexpr T& at(const size_t index) const;

	public:
		inline constexpr basic_vector3& operator+=(const basic_vector3& other) noexcept;
		inline constexpr basic_vector3& operator-=(const basic_vector3& other) noexcept;
		inline constexpr basic_vector3& operator*=(float value) noexcept;
		inline constexpr basic_vector3& operator/=(float value) noexcept;

	public:
		inline constexpr basic_vector3 operator+(const basic_vector3& other) const noexcept;
		inline constexpr basic_vector3 operator-(const basic_vector3& other) const noexcept;
		inline constexpr basic_vector3 operator*(float value) const noexcept;
		inline constexpr basic_vector3 operator/(float value) const noexcept;

	public:
		inline constexpr basic_vector3 operator-() const noexcept;

	public:
		inline constexpr void normalize();

	public:
		inline constexpr T length_square() const;
		inline constexpr T length() const;
		inline constexpr basic_vector3 normalized() const;
		inline constexpr float dot(const basic_vector3& other) const;
		inline constexpr basic_vector3 cross(const basic_vector3& other) const;

	public:
		static inline constexpr basic_vector3 right() noexcept;
		static inline constexpr basic_vector3 left() noexcept;
		static inline constexpr basic_vector3 up() noexcept;
		static inline constexpr basic_vector3 down() noexcept;
		static inline constexpr basic_vector3 forward() noexcept;
		static inline constexpr basic_vector3 backward() noexcept;
		static inline constexpr basic_vector3 one() noexcept;
		static inline constexpr basic_vector3 zero() noexcept;

	private:
		std::array<T, VEC3_COUNT> _;
	};

	/*  -----  using宣言  -----------------------------------  */

	using vector3 = basic_vector3<float>;
	using vector3f = basic_vector3<float>;
	using vector3d = basic_vector3<double>;
	using vector3i = basic_vector3<int32_t>;
	using vector3u = basic_vector3<uint32_t>;

	/*  -----  inline定義  -----------------------------------  */

	template<typename T> inline constexpr basic_vector3<T>::basic_vector3() noexcept : basic_vector3(0) {}
	template<typename T> inline constexpr basic_vector3<T>::basic_vector3(T value) noexcept : basic_vector3(value, value, value) {}
	template<typename T> inline constexpr basic_vector3<T>::basic_vector3(T x, T y, T z) noexcept : _{ x,y,z } {}

	template<typename T> inline constexpr T* basic_vector3<T>::data() noexcept { return _.data(); }
	template<typename T> inline constexpr T& basic_vector3<T>::x() noexcept { return _.at(VEC3_X); }
	template<typename T> inline constexpr T& basic_vector3<T>::y() noexcept { return _.at(VEC3_Y); }
	template<typename T> inline constexpr T& basic_vector3<T>::z() noexcept { return _.at(VEC3_Z); }

	template<typename T> inline constexpr T& basic_vector3<T>::at(const size_t index) { _.at(index); }

	template<typename T> inline constexpr const T* basic_vector3<T>::data() const noexcept { return _.data(); }
	template<typename T> inline constexpr const T& basic_vector3<T>::x() const noexcept { return _.at(VEC3_X); }
	template<typename T> inline constexpr const T& basic_vector3<T>::y() const noexcept { return _.at(VEC3_Y); }
	template<typename T> inline constexpr const T& basic_vector3<T>::z() const noexcept { return _.at(VEC3_Z); }

	template<typename T> inline constexpr T& basic_vector3<T>::at(const size_t index) const { _.at(index); }

	template<typename T> inline constexpr basic_vector3<T>& basic_vector3<T>::operator+=(const basic_vector3& other) noexcept
	{
		x() += other.x();
		y() += other.y();
		z() += other.z();
		return *this;
	}

	template<typename T> inline constexpr basic_vector3<T>& basic_vector3<T>::operator-=(const basic_vector3& other) noexcept
	{
		x() -= other.x();
		y() -= other.y();
		z() -= other.z();
		return *this;
	}

	template<typename T> inline constexpr basic_vector3<T>& basic_vector3<T>::operator*=(float value) noexcept
	{
		x() *= value;
		y() *= value;
		z() *= value;
		return *this;
	}

	template<typename T> inline constexpr basic_vector3<T>& basic_vector3<T>::operator/=(float value) noexcept
	{
		return *this *= 1.0f / value;
	}

	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::operator+(const basic_vector3& other) const noexcept { return basic_vector3(x() + other.x(), y() + other.y(), z() + other.z()); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::operator-(const basic_vector3& other) const noexcept { return basic_vector3(x() - other.x(), y() - other.y(), z() - other.z()); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::operator*(float value) const noexcept { return basic_vector3(x() * value, y() * value, z() * value); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::operator/(float value) const noexcept { return *this * 1.0f / value; }

	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::operator-() const noexcept { return basic_vector3(-x(), -y(), -z()); }

	template<typename T> inline constexpr void basic_vector3<T>::normalize()
	{
		*this /= length_square();
	}

	template<typename T> inline constexpr T basic_vector3<T>::length_square() const
	{
		return x() * x() + y() * y() + z() * z();
	}

	template<typename T> inline constexpr T basic_vector3<T>::length() const
	{
		return math::sqrt(this->length_square());
	}

	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::normalized() const
	{
		basic_vector3 copy = *this;
		copy.normalize();
		return copy;
	}

	template<typename T> inline constexpr float basic_vector3<T>::dot(const basic_vector3& other) const
	{
		return x() * other.x() + y() * other.y() + z() * other.z();
	}

	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::cross(const basic_vector3& other) const
	{
		return basic_vector3
		(
			y() * other.z() - z() * other.y(),
			z() * other.x() - x() * other.z(),
			x() * other.y() - y() * other.x()
		);
	}

	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::right() noexcept { return basic_vector3(1, 0, 0); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::left() noexcept { return basic_vector3(-1, 0, 0); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::up() noexcept { return basic_vector3(0, 1, 0); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::down() noexcept { return basic_vector3(0, -1, 0); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::forward() noexcept { return basic_vector3(0, 0, 1); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::backward() noexcept { return basic_vector3(0, 0, -1); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::one() noexcept { return basic_vector3(1, 1, 1); }
	template<typename T> inline constexpr basic_vector3<T> basic_vector3<T>::zero() noexcept { return basic_vector3(0, 0, 0); }
}
