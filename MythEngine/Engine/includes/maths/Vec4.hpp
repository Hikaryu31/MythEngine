#ifndef _VECTOR_4_HPP_
#define _VECTOR_4_HPP_
#include "maths/Vec3.hpp"
class Vec4
{
    public:

        float   m_x,
                m_y,
                m_z,
                m_w;

		static Vec4 const   One,
                            Zero;

        static float Distance (Vec4 const& p_a, Vec4 const& p_b);
        static float DistanceSquared (Vec4 const& p_a, Vec4 const& p_b);
        static float Dot (Vec4 const& p_a, Vec4 const& p_b);
        static bool IsNearlyEqual (Vec4 const& p_a, Vec4 const& p_b,float const p_tolerance = MATH_EPSILON_F);
        static Vec4 Lerp (Vec4 const& p_a,Vec4 const& p_b,float const    p_alpha);
        static Vec4 Max (Vec4 const& p_a, Vec4 const& p_b);
        static Vec4 Min (Vec4 const& p_a, Vec4 const& p_b);
        static Vec4 MoveTowards (Vec4 const& p_current, Vec4 const& p_target,float const    p_maxDistanceDelta);
        static Vec4 Project (Vec4 const& p_vector, Vec4 const& p_onTarget);
        static Vec4 ProjectOnNormal (Vec4 const& p_vector, Vec4 const& p_onNormal);

    // Constructors
    	Vec4 ();
        Vec4 (float const p_value);
        Vec4 (float const p_x, float const p_y, float const p_z, float const p_w);
        Vec4 (Vec4 const& p_from, Vec4 const& p_to);
        Vec4 (Vec2 const& p_vector2, float const p_z = 0.0f, float const p_w = 0.0f);
        Vec4 (Vec3 const& p_vector3, float const p_w = 0.0f);
        Vec4 (Vec2 const& p_xy, Vec2 const& p_zw);
        Vec4 (Vec4 const& p_copy);
		~Vec4 () = default;

	// Member Methods :
        float GetMagnit () const;
        Vec4 GetHomogenized () const;
        Vec3 GetHomogenized3D () const;
        Vec3 Get3D() const;
        float GetMagnitudeSquared () const;
        float GetMax () const;
        float GetMin () const;
        Vec4 GetSafeNormalized (float const p_tolerance = MATH_EPSILON_F) const;
        Vec4 GetUnsafeNormalized () const;
        void Homogenize ();
        bool IsNearlyZero (float const    p_tolerance = MATH_EPSILON_F) const;
        bool IsNormalized () const;
        bool IsUniform (float const p_tolerance = MATH_EPSILON_F) const;
        bool IsUnit (float const p_lengthSquaredTolerance = MATH_EPSILON_F)  const;
        bool IsZero () const;
        void Normalize (float const p_tolerance = MATH_EPSILON_F);
        void Set (float const p_value);
        void Set (float const p_x, float const p_y, float const p_z, float const p_w);
        void Set (Vec2 const& p_vec2, float const p_z = 0.0f, float const p_w = 0.0f);
        void Set (Vec3 const& p_vec3, float const p_w = 0.0f);
        void ToDirectionAndLength (Vec4& p_outDirection, float& p_outLength)const;

    // Operators:
		inline bool        operator==(Vec4 const& p_vector)   const
        {
            return m_x == p_vector.m_x && m_y == p_vector.m_y &&
                   m_z == p_vector.m_z && m_w == p_vector.m_w;
        }
        inline bool        operator!=(Vec4 const& p_vector)   const
        {
            return m_x != p_vector.m_x || m_y != p_vector.m_y || 
                   m_z != p_vector.m_z || m_w != p_vector.m_w;
        }

        inline Vec4&    operator=(Vec4 const& p_other)
        {
            m_x = p_other.m_x;
            m_y = p_other.m_y;
            m_z = p_other.m_z;
            m_w = p_other.m_w;
            return *this;
        }

        inline float       operator[](unsigned int const    p_index)    const
        {
            if (p_index < 0 || p_index > 3)
            {
                throw std::out_of_range("Index is out of range, should be bet m_ween 0 and 3");
            }
            return (p_index == 3) ? m_w :
                   (p_index == 2) ? m_z :
                   (p_index == 1) ? m_y : m_x;
        }
        inline float&      operator[](unsigned int const p_index)
        {
            return (p_index == 3) ? m_w :
                   (p_index == 2) ? m_z :
                   (p_index == 1) ? m_y : m_x;
        }

        inline Vec4     operator+(Vec4 const& p_vector)const
        {
            return { m_x + p_vector.m_x,  m_y + p_vector.m_y,
                     m_z + p_vector.m_z,  m_w + p_vector.m_w };
        }
        inline Vec4     operator+(float const p_value)const
        {
            return { m_x + p_value,  m_y + p_value,  m_z + p_value,  m_w + p_value };
        }
        inline Vec4&    operator+=(Vec4 const& p_vector)
        {
            m_x += p_vector.m_x;
            m_y += p_vector.m_y;
            m_z += p_vector.m_z;
            m_w += p_vector.m_z;
            return *this;
        }
        inline Vec4&    operator+=(float const p_value)
        {
            m_x += p_value;
            m_y += p_value;
            m_z += p_value;
            m_w += p_value;
            return *this;
        }

        inline Vec4     operator-()const
        {
            return{ -m_x, -m_y, -m_z, -m_w };
        }
        inline Vec4     operator-(Vec4 const& p_vector)const
        {
            return { m_x - p_vector.m_x,  m_y - p_vector.m_y,
                     m_z - p_vector.m_z,  m_w - p_vector.m_w };
        }
        inline Vec4     operator-(float const p_value)const
        {
            return { m_x - p_value,  m_y - p_value,  m_z - p_value, 0 };
        }
        inline Vec4&    operator-=(Vec4 const& p_vector)
        {
            m_x -= p_vector.m_x;
            m_y -= p_vector.m_y;
            m_z -= p_vector.m_z;
            m_w -= p_vector.m_z;
            return *this;
        }
        inline Vec4&    operator-=(float const p_value)
        {
            m_x -= p_value;
            m_y -= p_value;
            m_z -= p_value;
            m_w -= p_value;
            return *this;
        }

        inline Vec4     operator*(Vec4 const& p_vector)const
        {
            return { m_x * p_vector.m_x,  m_y * p_vector.m_y,
                     m_z * p_vector.m_z,  m_w * p_vector.m_w };
        }
        inline Vec4     operator*(float const p_scale)const
        {
            return { m_x * p_scale,  m_y * p_scale,  m_z * p_scale,  m_w * p_scale };
        }
        inline Vec4&    operator*=(Vec4 const& p_vector)
        {
            m_x *= p_vector.m_x;
            m_y *= p_vector.m_y;
            m_z *= p_vector.m_z;
            m_w *= p_vector.m_z;
            return *this;
        }
        inline Vec4&    operator*=(float const p_scale)
        {
            m_x *= p_scale;
            m_y *= p_scale;
            m_z *= p_scale;
            m_w *= p_scale;
            return *this;
        }

        inline Vec4     operator/(Vec4 const& p_vector)const
        {
            return { m_x / p_vector.m_x,  m_y / p_vector.m_y,
                     m_z / p_vector.m_z,  m_w / p_vector.m_w };
        }
        inline Vec4     operator/(float const p_scale)const
        {
            return { m_x / p_scale,  m_y / p_scale,  m_z / p_scale,  m_w / p_scale };
        }
        inline Vec4&    operator/=(Vec4 const& p_vector)
        {
            m_x /= p_vector.m_x;
            m_y /= p_vector.m_y;
            m_z /= p_vector.m_z;
            m_w /= p_vector.m_z;
            return *this;
        }
        inline Vec4&    operator/=(float const p_scale)
        {
            if (p_scale == 0 )
            {

            }
            m_x /= p_scale;
            m_y /= p_scale;
            m_z /= p_scale;
            m_w /= p_scale;
            return *this;
        }

};
        inline Vec4 operator+ (float const p_value, Vec4 const& p_vector)
        {
            return p_vector + p_value;
        }
    	inline Vec4 operator* (float const p_scale, Vec4 const& p_vector)
        {
            return p_vector * p_scale;
        }
#endif