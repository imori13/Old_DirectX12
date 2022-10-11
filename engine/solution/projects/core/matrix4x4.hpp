#pragma once

namespace
{
	constexpr uint8_t MATRIX4X4_ROW = 4u;
	constexpr uint8_t MATRIX4X4_COUNT = 16u;
}

namespace math
{
	template<typename T = float> class basic_matrix4x4
	{
	public:
		inline constexpr explicit basic_matrix4x4() noexcept = default;
		inline constexpr explicit basic_matrix4x4(float m_00, float m_01, float m_02, float m_03, float m_10, float m_11, float m_12, float m_13, float m_20, float m_21, float m_22, float m_23, float m_30, float m_31, float m_32, float m_33);

	public:
		inline constexpr T* data();
		inline constexpr T& at(uint32_t i);
		inline constexpr T& at(uint64_t x, uint64_t y);

	public:
		inline constexpr const T* data() const;
		inline constexpr const T& at(uint32_t i) const;
		inline constexpr const T& at(uint64_t x, uint64_t y) const;

	public:
		inline constexpr basic_matrix4x4& operator+=(const basic_matrix4x4& other);
		inline constexpr basic_matrix4x4& operator-=(const basic_matrix4x4& other);
		inline constexpr basic_matrix4x4& operator*=(const basic_matrix4x4& other);

	public:
		inline constexpr basic_matrix4x4 operator+(const basic_matrix4x4& other) const;
		inline constexpr basic_matrix4x4 operator-(const basic_matrix4x4& other) const;
		inline constexpr basic_matrix4x4 operator*(const basic_matrix4x4& other) const;

	public:
		static inline constexpr basic_matrix4x4 identity();

	private:
		std::array<T, MATRIX4X4_COUNT> _;
	};

	using matrix4x4 = basic_matrix4x4<float>;
	using matrix4x4f = basic_matrix4x4<float>;
	using matrix4x4d = basic_matrix4x4<double>;
	using matrix4x4i = basic_matrix4x4<int32_t>;
	using matrix4x4u = basic_matrix4x4<uint32_t>;

	// ----------------------------------------------------------------------------------------------------
	// inline定義
	// ----------------------------------------------------------------------------------------------------

	template<typename T> inline constexpr basic_matrix4x4<T>::basic_matrix4x4(float m_00, float m_01, float m_02, float m_03, float m_10, float m_11, float m_12, float m_13, float m_20, float m_21, float m_22, float m_23, float m_30, float m_31, float m_32, float m_33) : _{ m_00, m_01, m_02, m_03, m_10, m_11, m_12, m_13, m_20, m_21, m_22, m_23, m_30, m_31, m_32, m_33 } {}

	template<typename T> inline constexpr T* basic_matrix4x4<T>::data() { return _.data(); }
	template<typename T> inline constexpr T& basic_matrix4x4<T>::at(uint32_t i) { return _.at(i); }
	template<typename T> inline constexpr T& basic_matrix4x4<T>::at(uint64_t x, uint64_t y) { return _.at(x + y * MATRIX4X4_ROW); }
	template<typename T> inline constexpr const T* basic_matrix4x4<T>::data() const { return _.data(); }
	template<typename T> inline constexpr const T& basic_matrix4x4<T>::at(uint32_t i) const { return _.at(i); }
	template<typename T> inline constexpr const T& basic_matrix4x4<T>::at(uint64_t x, uint64_t y) const { return _.at(x + y * MATRIX4X4_ROW); }

	template<typename T> inline constexpr basic_matrix4x4<T>& basic_matrix4x4<T>::operator+=(const basic_matrix4x4& other)
	{
		at(0, 0) += other.at(0, 0);
		at(0, 1) += other.at(0, 1);
		at(0, 2) += other.at(0, 2);
		at(0, 3) += other.at(0, 3);

		at(1, 0) += other.at(1, 0);
		at(1, 1) += other.at(1, 1);
		at(1, 2) += other.at(1, 2);
		at(1, 3) += other.at(1, 3);

		at(2, 0) += other.at(2, 0);
		at(2, 1) += other.at(2, 1);
		at(2, 2) += other.at(2, 2);
		at(2, 3) += other.at(2, 3);

		at(3, 0) += other.at(3, 0);
		at(3, 1) += other.at(3, 1);
		at(3, 2) += other.at(3, 2);
		at(3, 3) += other.at(3, 3);

		return *this;
	}

	template<typename T> inline constexpr basic_matrix4x4<T>& basic_matrix4x4<T>::operator-=(const basic_matrix4x4& other)
	{
		at(0, 0) -= other.at(0, 0);
		at(0, 1) -= other.at(0, 1);
		at(0, 2) -= other.at(0, 2);
		at(0, 3) -= other.at(0, 3);

		at(1, 0) -= other.at(1, 0);
		at(1, 1) -= other.at(1, 1);
		at(1, 2) -= other.at(1, 2);
		at(1, 3) -= other.at(1, 3);

		at(2, 0) -= other.at(2, 0);
		at(2, 1) -= other.at(2, 1);
		at(2, 2) -= other.at(2, 2);
		at(2, 3) -= other.at(2, 3);

		at(3, 0) -= other.at(3, 0);
		at(3, 1) -= other.at(3, 1);
		at(3, 2) -= other.at(3, 2);
		at(3, 3) -= other.at(3, 3);

		return *this;
	}

	template<typename T> inline constexpr basic_matrix4x4<T>& basic_matrix4x4<T>::operator*=(const basic_matrix4x4& other)
	{
		*this = *this * other;
		return *this;
	}

	template<typename T> inline constexpr basic_matrix4x4<T> basic_matrix4x4<T>::operator+(const basic_matrix4x4& other) const
	{
		return basic_matrix4x4(
			at(0, 0) + other.at(0, 0), at(0, 1) + other.at(0, 1), at(0, 2) + other.at(0, 2), at(0, 3) + other.at(0, 3),
			at(1, 0) + other.at(1, 0), at(1, 1) + other.at(1, 1), at(1, 2) + other.at(1, 2), at(1, 3) + other.at(1, 3),
			at(2, 0) + other.at(2, 0), at(2, 1) + other.at(2, 1), at(2, 2) + other.at(2, 2), at(2, 3) + other.at(2, 3),
			at(3, 0) + other.at(3, 0), at(3, 1) + other.at(3, 1), at(3, 2) + other.at(3, 2), at(3, 3) + other.at(3, 3)
		);
	}

	template<typename T> inline constexpr basic_matrix4x4<T> basic_matrix4x4<T>::operator-(const basic_matrix4x4& other) const
	{
		return basic_matrix4x4(
			at(0, 0) - other.at(0, 0), at(0, 1) - other.at(0, 1), at(0, 2) - other.at(0, 2), at(0, 3) - other.at(0, 3),
			at(1, 0) - other.at(1, 0), at(1, 1) - other.at(1, 1), at(1, 2) - other.at(1, 2), at(1, 3) - other.at(1, 3),
			at(2, 0) - other.at(2, 0), at(2, 1) - other.at(2, 1), at(2, 2) - other.at(2, 2), at(2, 3) - other.at(2, 3),
			at(3, 0) - other.at(3, 0), at(3, 1) - other.at(3, 1), at(3, 2) - other.at(3, 2), at(3, 3) - other.at(3, 3)
		);
	}

	template<typename T> inline constexpr basic_matrix4x4<T> basic_matrix4x4<T>::operator*(const basic_matrix4x4& other) const
	{
		return basic_matrix4x4(
			at(0, 0) * other.at(0, 0) + at(0, 1) * other.at(1, 0) + at(0, 2) * other.at(2, 0) + at(0, 3) * other.at(3, 0),
			at(0, 0) * other.at(0, 1) + at(0, 1) * other.at(1, 1) + at(0, 2) * other.at(2, 1) + at(0, 3) * other.at(3, 1),
			at(0, 0) * other.at(0, 2) + at(0, 1) * other.at(1, 2) + at(0, 2) * other.at(2, 2) + at(0, 3) * other.at(3, 2),
			at(0, 0) * other.at(0, 3) + at(0, 1) * other.at(1, 3) + at(0, 2) * other.at(2, 3) + at(0, 3) * other.at(3, 3),

			at(1, 0) * other.at(0, 0) + at(1, 1) * other.at(1, 0) + at(1, 2) * other.at(2, 0) + at(1, 3) * other.at(3, 0),
			at(1, 0) * other.at(0, 1) + at(1, 1) * other.at(1, 1) + at(1, 2) * other.at(2, 1) + at(1, 3) * other.at(3, 1),
			at(1, 0) * other.at(0, 2) + at(1, 1) * other.at(1, 2) + at(1, 2) * other.at(2, 2) + at(1, 3) * other.at(3, 2),
			at(1, 0) * other.at(0, 3) + at(1, 1) * other.at(1, 3) + at(1, 2) * other.at(2, 3) + at(1, 3) * other.at(3, 3),

			at(2, 0) * other.at(0, 0) + at(2, 1) * other.at(1, 0) + at(2, 2) * other.at(2, 0) + at(2, 3) * other.at(3, 0),
			at(2, 0) * other.at(0, 1) + at(2, 1) * other.at(1, 1) + at(2, 2) * other.at(2, 1) + at(2, 3) * other.at(3, 1),
			at(2, 0) * other.at(0, 2) + at(2, 1) * other.at(1, 2) + at(2, 2) * other.at(2, 2) + at(2, 3) * other.at(3, 2),
			at(2, 0) * other.at(0, 3) + at(2, 1) * other.at(1, 3) + at(2, 2) * other.at(2, 3) + at(2, 3) * other.at(3, 3),

			at(3, 0) * other.at(0, 0) + at(3, 1) * other.at(1, 0) + at(3, 2) * other.at(2, 0) + at(3, 3) * other.at(3, 0),
			at(3, 0) * other.at(0, 1) + at(3, 1) * other.at(1, 1) + at(3, 2) * other.at(2, 1) + at(3, 3) * other.at(3, 1),
			at(3, 0) * other.at(0, 2) + at(3, 1) * other.at(1, 2) + at(3, 2) * other.at(2, 2) + at(3, 3) * other.at(3, 2),
			at(3, 0) * other.at(0, 3) + at(3, 1) * other.at(1, 3) + at(3, 2) * other.at(2, 3) + at(3, 3) * other.at(3, 3)
		);
	}

	template<typename T> inline constexpr basic_matrix4x4<T> basic_matrix4x4<T>::identity()
	{
		return basic_matrix4x4
		(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}
}