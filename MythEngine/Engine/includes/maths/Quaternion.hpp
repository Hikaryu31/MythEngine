#ifndef _QUATERNION_HPP_
#define _QUATERNION_HPP_

#include "maths/Mat4x4.hpp"

class Quaternion
{
	public:
		float   m_i = 0,
				m_j = 0,
				m_k = 0,
				m_r = 1;

	// Static methods:
		static Quaternion const Identity;

		static Quaternion GetQuatFromEuler(Vec3 const& p_euler);
		static Quaternion GetQuatFromEuler(float const& p_pitch, float const& p_yaw, float const& p_roll);
		static Quaternion GetQuatFromRotMat(Mat4x4 p_mat);
		static Quaternion Lerp(Quaternion const& p_q1, Quaternion const& p_q2, float p_alpha);
		static Quaternion Nlerp(Quaternion const& p_q1, Quaternion const& p_q2, float p_alpha);
		static Quaternion Slerp(Quaternion const& p_q1, Quaternion p_q2, float p_alpha);
		static Quaternion Vec3FromTo(Vec3 const& p_from, Vec3 const& p_to);
		static Vec3 Vec3RotateByQuat(Vec3 const& p_vec, Quaternion const& p_quat);
		static Mat4x4 TRSQuat(const Vec3& p_pos, Quaternion& p_quat, const Vec3& p_scale);
		static Quaternion LookAt(const Vec3& p_source, const Vec3& p_dest);
		static Quaternion CreateFromAxisAngle(const Vec3& axis, const float& angle);


	// Constructors:
		Quaternion();
		Quaternion(float const p_value);
		Quaternion(float const p_i, float const p_j, float const p_k, float const p_r);
		Quaternion(Vec4 const& p_vec);
		Quaternion(Vec3 const& p_axis, float p_angle);
		Quaternion(Quaternion const& p_copy);
		~Quaternion() = default;

	// Member Methods:
		Vec3 ToEulerAngles() const;
		Vec3 Foreward() const;
		Vec3 Up() const;
		Vec3 Right() const;
		Quaternion ApplyTRS(Mat4x4 p_mat);
		void GetAxisAndAngle(Vec3* p_outAxis, float* p_utAngle);
		float GetMagnitude();
		float GetSquaredMagnitude();
		Mat4x4 GetRotationMatrix() const;
		Quaternion GetSafeNormalized();
		Quaternion GetUnsafeNormalized();
		void SetAxis(Vec3 const&);

	// Operators:
		inline bool        operator==  (Quaternion const& p_vector)   const
		{
			return m_i == p_vector.m_i && m_j == p_vector.m_j && m_k == p_vector.m_k && m_r == p_vector.m_r;
		}

		inline Quaternion  operator+(Quaternion const& p_quat) const
		{
			return
			{
				m_i + p_quat.m_i,
				m_j + p_quat.m_j,
				m_k + p_quat.m_k,
				m_r + p_quat.m_r
			};
		}
		inline Quaternion  operator+(float const& p_value) const
		{
			return
			{
				m_i + p_value,
				m_j + p_value,
				m_k + p_value,
				m_r + p_value
			};
		}
		inline Quaternion& operator+=(Quaternion const& p_quat)
		{
			m_i += p_quat.m_i;
			m_j += p_quat.m_j;
			m_k += p_quat.m_k;
			m_r += p_quat.m_r;

			return *this;
		}
		inline Quaternion& operator+=(float const& p_value)
		{

			m_i += p_value;
			m_j += p_value;
			m_k += p_value;
			m_r += p_value;

			return *this;
		}

		inline Quaternion  operator-() const
		{
			return
			{
				-m_i,
				-m_j,
				-m_k,
				m_r
			};
		}
		inline Quaternion  operator-(Quaternion const& p_quat) const
		{
			return
			{
				m_i - p_quat.m_i,
				m_j - p_quat.m_j,
				m_k - p_quat.m_k,
				m_r - p_quat.m_r
			};
		}
		inline Quaternion  operator-(float const& p_value) const
		{
			return
			{
				m_i - p_value,
				m_j - p_value,
				m_k - p_value,
				m_r - p_value
			};
		}
		inline Quaternion& operator-=(Quaternion const& p_quat)
		{
			m_i -= p_quat.m_i;
			m_j -= p_quat.m_j;
			m_k -= p_quat.m_k;
			m_r -= p_quat.m_r;

			return *this;
		}
		inline Quaternion& operator-=(float const& p_value)
		{

			m_i -= p_value;
			m_j -= p_value;
			m_k -= p_value;
			m_r -= p_value;

			return *this;
		}

		inline Quaternion  operator*(Quaternion const& p_quat) const
		{
			return
			{
				m_i * p_quat.m_r + m_r * p_quat.m_i + m_j * p_quat.m_k - m_k * p_quat.m_j,
				m_j * p_quat.m_r + m_r * p_quat.m_j + m_k * p_quat.m_i - m_i * p_quat.m_k,
				m_k * p_quat.m_r + m_r * p_quat.m_k + m_i * p_quat.m_j - m_j * p_quat.m_i,
				m_r * p_quat.m_r - m_i * p_quat.m_i - m_j * p_quat.m_j - m_k * p_quat.m_k
			};
		}
		inline Quaternion  operator*(float const& p_value) const
		{
			return
			{
				m_i * p_value + m_j * p_value + m_r * p_value - m_k * p_value,
				m_j * p_value + m_k * p_value + m_r * p_value - m_i * p_value,
				m_k * p_value + m_i * p_value + m_r * p_value - m_j * p_value,
				m_r * p_value - m_i * p_value - m_j * p_value - m_k * p_value
			};
		}
		inline Quaternion& operator*=(Quaternion const& p_quat)
		{
			float i = m_i, j = m_j, k = m_k, r = m_r;

			m_i = i * p_quat.m_r + r * p_quat.m_i + j * p_quat.m_k - k * p_quat.m_j;
			m_j = j * p_quat.m_r + r * p_quat.m_j + k * p_quat.m_i - i * p_quat.m_k;
			m_k = k * p_quat.m_r + r * p_quat.m_k + i * p_quat.m_j - j * p_quat.m_i;
			m_r = r * p_quat.m_r - i * p_quat.m_i - j * p_quat.m_j - k * p_quat.m_k;

			return *this;
		}
		inline Quaternion& operator*=(float const& p_value)
		{
			float i = m_i, j = m_j, k = m_k, r = m_r;
			m_i = i * p_value + j * p_value + r * p_value - k * p_value;
			m_j = j * p_value + k * p_value + r * p_value - i * p_value;
			m_k = k * p_value + i * p_value + r * p_value - j * p_value;
			m_r = r * p_value - i * p_value - j * p_value - k * p_value;

			return *this;
		}

		inline Quaternion  operator/(Quaternion const& p_quat) const
		{
			return
			{
				m_i / p_quat.m_i,
				m_j / p_quat.m_j,
				m_k / p_quat.m_k,
				m_r / p_quat.m_r
			};
		}
		inline Quaternion& operator/=(Quaternion const& p_quat)
		{
			float i = m_i, j = m_j, k = m_k, r = m_r;

			m_i = i / p_quat.m_i;
			m_j = j / p_quat.m_j;
			m_k = k / p_quat.m_k;
			m_r = r / p_quat.m_r;

			return *this;
		}

		inline Quaternion& operator=(Quaternion const& p_quat)
		{
			m_i = p_quat.m_i;
			m_j = p_quat.m_j;
			m_k = p_quat.m_k;
			m_r = p_quat.m_r;
			return *this;
		}
};

#endif