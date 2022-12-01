#ifndef _VECTOR_2_HPP_
#define _VECTOR_2_HPP_
#include <stdexcept>
#include "maths/Maths.hpp"
#include <iostream>

class Vec2
{
public:
    static Vec2 const Right;
    static Vec2 const Left;
    static Vec2 const Up;
    static Vec2 const Down;
    static Vec2 const One;
    static Vec2 const Zero;

    float   m_x,
            m_y;

    // Constructors:
    Vec2();
    Vec2(Vec2 p_from, Vec2 p_to);
    Vec2(float p_x, float p_y);
    Vec2(float p_xy);
    Vec2(Vec2 const& p_copy);
    ~Vec2() = default;

    // Static methods:
    static float AngleDeg(Vec2 const& p_from, Vec2 const& p_to); //(Angle SIGNE (entre -180 et 180) en degree !)
    static float AngleRad(Vec2 const& p_from, Vec2 const& p_to); //(Angle SIGNE (entre -180 et 180) en radian !)

    static Vec2 Clamp(Vec2 const& p_value, Vec2 const& p_min, Vec2 const& p_max);
    static Vec2 ClampMagnitude(Vec2 const& p_vector, float const p_minMagnitude, float const p_maxMagnitude);
    static Vec2 ClampMaxMagnitude(Vec2 const& p_vector, float const p_maxMagnitude);
    static Vec2 ClampMinMagnitude(Vec2 const& p_vector, float const p_minMagnitude);

    static float Distance(Vec2 const& p_a, Vec2 const& p_b);
    static float DistanceSquared(Vec2 const& p_a, Vec2 const& p_b);
    static float Dot(Vec2 const& p_a, Vec2 const& p_b);

    static bool IsCollinear(Vec2 const& p_a, Vec2 const& p_b);
    static bool IsNearlyEqual(Vec2 const& p_a, Vec2 const& p_b, float const p_tolerance = MATH_EPSILON_F);
    static bool IsOrthogonal(Vec2 const& p_a, Vec2 const& p_b);

    static Vec2 Lerp(Vec2 const& p_a, Vec2 const& p_b, float const p_alpha);
    static Vec2 Max(Vec2 const& p_a, Vec2 const& p_b);
    static Vec2 Min(Vec2 const& p_a, Vec2 const& p_b);
    static Vec2 MoveTowards(Vec2 const& p_current, Vec2 const& p_target, float const p_maxDistanceDelta);
    static Vec2 Project(Vec2 const& p_vector, Vec2 const& p_onTarget);
    static Vec2 ProjectOnNormal(Vec2 const& p_vector, Vec2 const& p_onNormal);
    static Vec2 Reflect(Vec2 const& p_direction, Vec2 const& p_normal);
    static Vec2 ReflectInvert(Vec2 const& p_direction, Vec2 const& p_normal);
    static Vec2 RotateDeg(Vec2 const& p_vector, float const p_angleDeg);
    static Vec2 RotateRad(Vec2 const& p_vector, float const p_angleRad);

    static float SignedAngleDeg(Vec2 const& p_from, Vec2 const& p_to);
    static float SignedAngleRad(Vec2 const& p_from, Vec2 const& p_to);

    // Member Methods:
    Vec2 GetAbs() const;

    float GetAbsMax() const;
    float GetAbsMin() const;
    float Length() const;
    float SquaredLength() const;
    float GetMax() const;
    float GetMin() const;

    Vec2 GetPerpendicular() const;
    Vec2 GetSign() const;
    Vec2 GetSafeNormalized(float const p_tolerance = MATH_EPSILON_F) const;
    Vec2 GetUnsafeNormalized() const;

    bool IsNearlyZero(float const    p_tolerance = MATH_EPSILON_F) const;
    bool IsNormalized() const;
    bool IsUniform(float const p_tolerance = MATH_EPSILON_F) const;
    bool IsUnit(float const p_lengthSquaredTolerance = MATH_EPSILON_F)  const;
    bool IsZero() const;

    void Normalize();
    void Set(float const p_value);
    void Set(float const p_x, float const    p_y);
    void ToDirectionAndLength(Vec2& p_outDirection, float& p_outLength)const;

    // Operators:
    inline bool    operator==  (Vec2 const& p_vector)   const
    {
        return m_x == p_vector.m_x && m_y == p_vector.m_y;
    }
    inline bool    operator!=  (Vec2 const& p_vector)   const
    {
        return m_x != p_vector.m_x || m_y != p_vector.m_y;
    }

    inline Vec2& operator=   (Vec2 const& p_other)
    {
        m_x = p_other.m_x;
        m_y = p_other.m_y;
        return *this;
    }

    inline float   operator[]  (unsigned int const    p_index)    const
    {
        if (p_index < 0 || p_index > 1)
        {
            throw std::out_of_range("Index is out of range, should be between 0 and 1");
        }
        return (p_index == 1) ? m_y : m_x;
    }
    inline float&  operator[]  (unsigned int const    p_index)
    {
        if (p_index < 0 || p_index > 1)
        {
            throw std::out_of_range("Index is out of range, should be between 0 and 1");
        }
        return (p_index == 1) ? m_y : m_x;
    }

    inline Vec2    operator+(Vec2 const& p_vector)const
    {
        return { m_x + p_vector.m_x , m_y + p_vector.m_y };
    }
    inline Vec2    operator+(float const   p_value)const
    {
        return { m_x + p_value, m_y + p_value };
    }
    inline Vec2&   operator+=(Vec2 const& p_vector)
    {
        m_x += p_vector.m_x;
        m_y += p_vector.m_y;
        return *this;
    }
    inline Vec2&   operator+=(float const   p_value)
    {
        m_x += p_value;
        m_y += p_value;
        return *this;
    }

    inline Vec2    operator-()const
    {
        return { -m_x, -m_y };
    }
    inline Vec2    operator-(Vec2 const& p_vector)const
    {
        return { m_x - p_vector.m_x , m_y - p_vector.m_y };

    }
    inline Vec2    operator-(float const   p_value)const
    {
        return { m_x - p_value , m_y - p_value };

    }
    inline Vec2&   operator-=(Vec2 const& p_vector)
    {
        m_x -= p_vector.m_x;
        m_y -= p_vector.m_y;
        return *this;
    }
    inline Vec2&   operator-=(float const   p_value)
    {
        m_x -= p_value;
        m_y -= p_value;
        return *this;
    }

    inline Vec2    operator*(Vec2 const& p_vector)const
    {
        return { m_x * p_vector.m_x, m_y * p_vector.m_y };
    }
    inline Vec2    operator*(float const   p_scale)const
    {
        return { m_x * p_scale, m_y * p_scale };
    }
    inline Vec2&   operator*=(Vec2 const& p_vector)
    {
        m_x *= p_vector.m_x;
        m_y *= p_vector.m_y;
        return *this;
    }
    inline Vec2&   operator*=(float const   p_scale)
    {
        m_x *= p_scale;
        m_y *= p_scale;
        return *this;
    }

    inline Vec2    operator/(Vec2 const& p_vector)const
    {
        return { m_x / p_vector.m_x, m_y / p_vector.m_y };
    }
    inline Vec2    operator/(float const   p_scale)const
    {
        return { m_x / p_scale, m_y / p_scale };
    }
    inline Vec2&   operator/=(Vec2 const& p_vector)
    {
        m_x /= p_vector.m_x;
        m_y /= p_vector.m_y;
        return *this;
    }
    inline Vec2&   operator/=(float const   p_scale)
    {
        m_x /= p_scale;
        m_y /= p_scale;
        return *this;
    }

};

inline Vec2 operator+(float const p_value, Vec2 const& p_vector)
{
    return  p_vector + p_value;
}
inline Vec2 operator*(float const p_scale, Vec2 const& p_vector)
{
    return p_vector * p_scale;
}


#endif