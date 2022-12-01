#include "maths/Vec4.hpp"

Vec4 const Vec4::One(1, 1, 1, 1);
Vec4 const Vec4:: Zero(0, 0, 0, 0);

float Vec4::Distance(Vec4 const& p_a, Vec4 const& p_b)
{
    return Maths::Sqrt(DistanceSquared(p_a, p_b));
}

float Vec4::DistanceSquared(Vec4 const& p_a, Vec4 const& p_b)
{
    return Maths::Square(p_b.m_x - p_a.m_x) + Maths::Square(p_b.m_y - p_a.m_y) + Maths::Square(p_b.m_z - p_a.m_z), Maths::Square(p_b.m_z - p_a.m_z);
}

float Vec4::Dot (Vec4 const& p_a, Vec4 const& p_b)
{
    return p_a.m_x * p_b.m_x + p_a.m_y * p_b.m_y + p_a.m_z * p_b.m_z + p_a.m_w * p_b.m_w;
}

bool Vec4::IsNearlyEqual (Vec4 const& p_a, Vec4 const& p_b,float const p_tolerance)
{
    return Maths::IsNearlyEqual(p_a.m_x, p_b.m_x, p_tolerance) && Maths::IsNearlyEqual(p_a.m_y, p_b.m_y, p_tolerance) && Maths::IsNearlyEqual(p_a.m_z, p_b.m_z, p_tolerance) && Maths::IsNearlyEqual(p_a.m_w, p_b.m_w, p_tolerance);
}

Vec4 Vec4::Lerp (Vec4 const& p_a,Vec4 const& p_b, float const    p_alpha)
{
    return p_a + (p_b - p_a) * p_alpha;
}

Vec4 Vec4::Max (Vec4 const& p_a, Vec4 const& p_b)
{
    return { Maths::Max(p_a.m_x, p_b.m_x), Maths::Max(p_a.m_y, p_b.m_y), Maths::Max(p_a.m_z, p_b.m_z), Maths::Max(p_a.m_w, p_b.m_w) };
}

Vec4 Vec4::Min (Vec4 const& p_a, Vec4 const& p_b)
{
    return { Maths::Min(p_a.m_x, p_b.m_x), Maths::Min(p_a.m_y, p_b.m_y), Maths::Min(p_a.m_z, p_b.m_z), Maths::Min(p_a.m_w, p_b.m_w) };
}

Vec4 Vec4::MoveTowards (Vec4 const& p_current, Vec4 const& p_target,float const    p_maxDistanceDelta)
{
    Vec4 const CrtToTarget{ p_current, p_target };
    float const distance = CrtToTarget.GetMagnit();

    if (distance <= p_maxDistanceDelta || Maths::IsNearlyZero(distance))
    {
        return p_target;
    }
    return p_current + CrtToTarget / distance * p_maxDistanceDelta;
}

Vec4 Vec4::Project (Vec4 const& p_vector, Vec4 const& p_onTarget)
{
    return p_onTarget * (Dot(p_onTarget, p_vector) / p_onTarget.GetMagnitudeSquared());
}

Vec4 Vec4::ProjectOnNormal (Vec4 const& p_vector, Vec4 const& p_onNormal)
{
    return p_onNormal * Dot(p_onNormal, p_vector);
}

// Constructors
Vec4::Vec4 ()
: m_x{0},  m_y{0},  m_z{0},  m_w{0}
{}
Vec4::Vec4 (float const p_value)
: m_x{p_value},  m_y{p_value},  m_z{p_value},  m_w{p_value}
{}
Vec4::Vec4 (float const p_x, float const p_y, float const p_z, float const p_w)
: m_x{p_x},  m_y{p_y},  m_z{p_z},  m_w{p_w}
{}
Vec4::Vec4 (Vec4 const& p_from, Vec4 const& p_to)
: m_x{p_to.m_x-p_from.m_x},  m_y{p_to.m_y-p_from.m_y},  m_z{p_to.m_z-p_from.m_z},  m_w{p_to.m_w-p_from.m_w}
{}

Vec4::Vec4 (Vec2 const& p_vector2, float const p_z, float const p_w)
:m_x{p_vector2.m_x},  m_y{p_vector2.m_y}, m_z{p_z}, m_w{p_w}
{}
Vec4::Vec4 (Vec3 const& p_vector3, float const p_w)
:m_x{p_vector3.m_x},  m_y{p_vector3.m_y}, m_z{p_vector3.m_z}, m_w{p_w}
{}
Vec4::Vec4 (Vec2 const& p_xy, Vec2 const& p_zw)
:m_x{p_xy.m_x}, m_y{p_xy.m_y}, m_z{p_zw.m_x}, m_w{p_zw.m_y}
{}
Vec4::Vec4 (Vec4 const& p_copy)
:m_x{p_copy.m_x}, m_y{p_copy.m_y}, m_z{p_copy.m_z}, m_w{p_copy.m_w}
{}

// Member Methods :
float Vec4::GetMagnit () const
{
    return Maths::Sqrt(GetMagnitudeSquared());
}
Vec4 Vec4::GetHomogenized () const
{
    return {m_x/ m_w,  m_y/ m_w,  m_y/ m_w,  m_w};
}
Vec3 Vec4::GetHomogenized3D () const
{
    return {m_x/ m_w,  m_y/ m_w,  m_y/ m_w};
}
Vec3 Vec4::Get3D() const
{
    return { m_x,  m_y,  m_z };
}

float Vec4::GetMagnitudeSquared() const
{
    return m_x*m_x +  m_y* m_y +  m_z* m_z +  m_w* m_w;
}
float Vec4::GetMax () const
{
    return Maths::Max(m_x, Maths::Max3( m_y,  m_z,  m_w));
}
float Vec4::GetMin () const
{
    return Maths::Min(m_x, Maths::Min3( m_y,  m_z,  m_w));
}
Vec4 Vec4::GetSafeNormalized (float const p_tolerance) const
{
    float SquaredLength = GetMagnitudeSquared();
    if (SquaredLength < p_tolerance)
    {
        return  Zero;
    }
    else if (SquaredLength == 1)
    {
        return *this;
    }

    SquaredLength = Maths::Sqrt(SquaredLength);

    return {m_x/SquaredLength, m_y/SquaredLength, m_z/SquaredLength, m_w/SquaredLength};
    
}
Vec4 Vec4::GetUnsafeNormalized () const
{
    float length = GetMagnit();
    return {m_x/length, m_y/length, m_z/length, m_w/length};
}
void Vec4::Homogenize () 
{
    m_x /=  m_w;
     m_y /=  m_w;
     m_z /=  m_w;
}
bool Vec4::IsNearlyZero (float const    p_tolerance) const
{
    return Maths::IsNearlyZero(m_x, p_tolerance) && Maths::IsNearlyZero( m_y, p_tolerance) && Maths::IsNearlyZero( m_z, p_tolerance) && Maths::IsNearlyZero( m_w, p_tolerance);
}
bool Vec4::IsNormalized () const
{
    return Maths::IsNearlyEqual(GetMagnitudeSquared(), 1);
}
bool Vec4::IsUniform (float const p_tolerance) const
{
    return Maths::IsNearlyEqual(m_x,  m_y, p_tolerance) && Maths::IsNearlyEqual(m_x,  m_z, p_tolerance) && Maths::IsNearlyEqual(m_x,  m_w, p_tolerance);
}
bool Vec4::IsUnit (float const p_lengthSquaredTolerance)  const
{
    return Maths::IsNearlyEqual(GetMagnitudeSquared(), 1, p_lengthSquaredTolerance);
}
bool Vec4::IsZero () const
{
    return m_x == 0 &&  m_y == 0 &&  m_z == 0 &&  m_w == 0;   
}
void Vec4::Normalize (float const p_tolerance)
{
    float SquaredLength = GetMagnitudeSquared();
    if (SquaredLength > p_tolerance)
    {
        SquaredLength = Maths::Sqrt(SquaredLength);

        m_x /= SquaredLength; 
         m_y /= SquaredLength; 
         m_z /= SquaredLength;
         m_w /= SquaredLength;
    }
}
void Vec4::Set (float const p_value)
{
    m_x =  m_y =  m_z =  m_w = p_value;
}
void Vec4::Set (float const p_x, float const p_y, float const p_z, float const p_w)
{
    m_x = p_x;
     m_y = p_y;
     m_z = p_z;
     m_w = p_w;
}
void Vec4::Set (Vec2 const& p_vec2, float const p_z , float const p_w)
{
    m_x = p_vec2.m_x;
     m_y = p_vec2.m_y;
     m_z = p_z;
     m_w = p_w;
}
void Vec4::Set (Vec3 const& p_vec3, float const p_w)
{
    
    m_x = p_vec3.m_x;
     m_y = p_vec3.m_y;
     m_z = p_vec3.m_z;
     m_w = p_w;
}
void Vec4::ToDirectionAndLength (Vec4& p_outDirection, float& p_outLength)const
{
    
    p_outLength = GetMagnit();
    if (p_outLength == 0) return;
	p_outDirection.m_x = m_x / p_outLength;
	p_outDirection.m_y = m_y / p_outLength;
	p_outDirection.m_z = m_z / p_outLength;
	p_outDirection.m_w = m_w / p_outLength;
}