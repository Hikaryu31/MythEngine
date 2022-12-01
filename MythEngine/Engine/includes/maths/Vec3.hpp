#ifndef _VECTOR_3_
#define _VECTOR_3_
#include "maths/Vec2.hpp"
#include <stdexcept>
#include <sstream>
#include <iomanip>

class Vec3
{
public:
    static Vec3 const Right; 
    static Vec3 const Left; 
    static Vec3 const Up; 
    static Vec3 const Down; 
    static Vec3 const Forward;
    static Vec3 const Backward;
    static Vec3 const One; 
    static Vec3 const Zero;
    static Vec3 const Plane;

    float   m_x, 
            m_y, 
            m_z;

    // Static Methods:
	static float AngleDeg (Vec3 const& p_from, Vec3 const& p_to);
	static float AngleRad (Vec3 const& p_from, Vec3 const& p_to);
	static Vec3 Clamp	(Vec3 const& p_value, Vec3 const& p_min, Vec3 const& p_max);
    static Vec3 ClampMagnitude (Vec3 const& p_vector, float const p_minMagnitude, float const    p_maxMagnitude);
    static Vec3 ClampMaxMagnitude (Vec3 const& p_vector, float const p_maxMagnitude);
    static Vec3 ClampMinMagnitude (Vec3 const& p_vector, float const p_minMagnitude);
    static Vec3 Cross (Vec3 const& p_lhs, Vec3 const& p_rhs);
    static float Distance (Vec3 const& p_a, Vec3 const& p_b);
    static float DistanceSquared (Vec3 const& p_a, Vec3 const& p_b);
    static float Dot (Vec3 const& p_a, Vec3 const& p_b);
    static bool IsCollinear (Vec3 const& p_a,Vec3 const& p_b);
    static bool IsNearlyEqual (Vec3 const& p_a, Vec3 const& p_b,float const p_tolerance = MATH_EPSILON_F);
    static bool IsOrthogonal (Vec3 const& p_a,Vec3 const& p_b);
    static Vec3 Lerp (Vec3 const& p_a,Vec3 const& p_b,float const    p_alpha);
    static Vec3 Max (Vec3 const& p_a, Vec3 const& p_b);
    static Vec3 Min (Vec3 const& p_a, Vec3 const& p_b);
    static Vec3 MoveTowards (Vec3 const& p_current, Vec3 const& p_target,float const    p_maxDistanceDelta);
    static Vec3 Project (Vec3 const& p_vector, Vec3 const& p_onTarget);
    static Vec3 ProjectOnNormal (Vec3 const& p_vector, Vec3 const& p_onNormal);
    static Vec3 ProjectOnPlane (Vec3 const& p_vector, Vec3 const& p_onNormal); //(Plane qui a pour position le (0,0,0))
    static Vec3 Reflect (Vec3 const& p_direction, Vec3 const& p_normal);
    static Vec3 ReflectInvert (Vec3 const& p_direction, Vec3 const& p_normal);
    static float SignedAngleDeg (Vec3 const& p_from, Vec3 const& p_to);
    static float SignedAngleRad (Vec3 const& p_from, Vec3 const& p_to);

    // Constructor:
    Vec3 ();
    Vec3 (float const p_value);
    Vec3 (float const p_x, float const p_y, float const p_z);
    Vec3 (Vec3 const& p_from, Vec3 const& p_to);
    Vec3 (Vec2 const& p_vector2, float const p_z = 0.0f);
    Vec3 (Vec3 const& p_copy);
	~Vec3 () = default;

	// Member Methods:
    Vec3 GetAbs () const;
    float GetAbsMax () const;
    float GetAbsMin () const;
    float GetMagnitude() const;
    float GetMagnitudeSquared () const;
    float GetMax () const;
    float GetMin () const;
    Vec3 GetSign () const;
    Vec3 GetSafeNormalized (float const p_tolerance = MATH_EPSILON_F) const;
    Vec3 GetUnsafeNormalized () const;
    bool IsNearlyZero (float const    p_tolerance = MATH_EPSILON_F) const;
    bool IsNormalized () const;
    bool IsUniform (float const p_tolerance = MATH_EPSILON_F) const;
    bool IsUnit (float const p_lengthSquaredTolerance = MATH_EPSILON_F)  const;
    bool IsZero () const;
    void Normalize (float const p_tolerance = MATH_EPSILON_F);
    void Set (float const p_value);
    void Set (Vec2 const& p_vec2, float const p_z = 0.0f);
    void Set (float const p_x, float const p_y, float const p_z);
    Vec3 Sqrt();
    void ToDirectionAndLength (Vec3& p_outDirection, float& p_outLength)const;

    // Operators:
	inline bool        operator==  (Vec3 const& p_vector)   const
    {
        return m_x == p_vector.m_x && m_y == p_vector.m_y && m_z == p_vector.m_z;
    }
    inline bool        operator!=  (Vec3 const& p_vector)   const
    {
        return m_x != p_vector.m_x || m_y != p_vector.m_y || m_z != p_vector.m_z;
    }
    inline Vec3&    operator=   (Vec3 const& p_other)
    {
        m_x = p_other.m_x;
        m_y = p_other.m_y;
        m_z = p_other.m_z;
        return *this;
    }
    inline Vec3& operator=   (Vec3& p_other)
    {
        m_x = p_other.m_x;
        m_y = p_other.m_y;
        m_z = p_other.m_z;
        return *this;
    }

    inline float       operator[]  (unsigned int const    p_index)    const
    {
        if (p_index < 0 || p_index > 2)
        {
            throw std::out_of_range("Indem_x is out of range, should be between 0 and 2");
        }
        return (p_index == 2) ? m_z : (p_index == 1) ? m_y : m_x;
    }

    inline float&      operator[]  (unsigned int const    p_index)
    {
        if (p_index < 0 || p_index > 2)
        {
            throw std::out_of_range("Indem_x is out of range, should be between 0 and 2");
        }
        return (p_index == 2) ? m_z : (p_index == 1) ? m_y : m_x;
    }

    inline Vec3     operator+   (Vec3 const& p_vector)   const
    {
        return { m_x + p_vector.m_x, m_y + p_vector.m_y, m_z + p_vector.m_z };
    }
    inline Vec3     operator+   (float const    p_value)    const
    {
        return { m_x + p_value, m_y + p_value, m_z + p_value };
    }
    inline Vec3&    operator+=  (Vec3 const& p_vector)
    {
        m_x += p_vector.m_x;
        m_y += p_vector.m_y;
        m_z += p_vector.m_z;
        return *this;
    }
    inline const Vec3&    operator+=  (Vec3& p_vector)
    {
        m_x += p_vector.m_x;
        m_y += p_vector.m_y;
        m_z += p_vector.m_z;
        return *this;
    }
    inline Vec3&    operator+=  (float const    p_value)
    {
        m_x += p_value;
        m_y += p_value;
        m_z += p_value;
        return *this;
    }

    inline Vec3     operator-   () const
    {
        return { -m_x, -m_y, -m_z };
    }
    inline Vec3     operator-   (Vec3 const& p_vector)   const
    {
        return { m_x - p_vector.m_x, m_y - p_vector.m_y, m_z - p_vector.m_z };
    }
    inline Vec3     operator-   (float const    p_value)    const
    {
        return { m_x - p_value, m_y - p_value, m_z - p_value };
    }
    inline Vec3&    operator-=  (Vec3 const& p_vector)
    {
        m_x -= p_vector.m_x;
        m_y -= p_vector.m_y;
        m_z -= p_vector.m_z;
        return *this;
    }
    inline Vec3&    operator-=  (float const    p_value)
    {
        m_x -= p_value;
        m_y -= p_value;
        m_z -= p_value;
        return *this;
    }

    inline Vec3     operator*   (Vec3 const& p_vector)   const
    {
        return { m_x * p_vector.m_x, m_y * p_vector.m_y, m_z * p_vector.m_z };
    }
    inline Vec3     operator*   (float const    p_scale)    const
    {
        return { m_x * p_scale, m_y * p_scale, m_z * p_scale };
    }
    inline Vec3&    operator*=  (Vec3 const& p_vector)
    {
        m_x *= p_vector.m_x;
        m_y *= p_vector.m_y;
        m_z *= p_vector.m_z;
        return *this;
    }
    inline Vec3&    operator*=  (Vec3& p_vector) 
    {
        m_x *= p_vector.m_x;
        m_y *= p_vector.m_y;
        m_z *= p_vector.m_z;
        return *this;
    }
    inline Vec3&    operator*=  (float const    p_scale)
    {
        m_x *= p_scale;
        m_y *= p_scale;
        m_z *= p_scale;
        return *this;
    }

    inline Vec3     operator/   (Vec3 const& p_vector)   const
    {
        return { m_x / p_vector.m_x, m_y / p_vector.m_y, m_z / p_vector.m_z };
    }
    inline Vec3     operator/   (float const    p_scale)    const
    {
        return { m_x / p_scale, m_y / p_scale, m_z / p_scale };
    }
    inline Vec3&    operator/=  (Vec3 const& p_vector)
    {
        m_x /= p_vector.m_x;
        m_y /= p_vector.m_y;
        m_z /= p_vector.m_z;
        return *this;
    }
    inline Vec3&    operator/=  (float const    p_scale)
    {
        m_x /= p_scale;
        m_y /= p_scale;
        m_z /= p_scale;
        return *this;
    }


    static inline std::string displayFloat3(Vec3 value)
    {
        std::stringstream  result;

        result << std::fixed << std::setprecision(2);
        result << value.m_x << " " << value.m_y << " " << value.m_z;

        return result.str();
    }


    inline Vec2 GetXY() { return { m_x, m_y }; }
    inline Vec2 GetXZ() { return { m_x, m_z }; }
    inline Vec2 GetYZ() { return { m_y, m_z }; }

};
    inline Vec3     operator+ (float const p_value, Vec3 const& p_vector)
    {
        return p_vector + p_value;
    }
    inline Vec3     operator* (float const p_scale, Vec3 const& p_vector)
    {
        return p_vector * p_scale;
    }


#endif