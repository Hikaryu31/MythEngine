#include <Maths/Vec3.hpp>

Vec3 const Vec3::Right(1, 0, 0);
Vec3 const Vec3::Left(-1, 0, 0);
Vec3 const Vec3::Up(0, 1, 0);
Vec3 const Vec3::Down(0, -1, 0);
Vec3 const Vec3::Forward(0, 0, 1);
Vec3 const Vec3::Backward(0, 0, -1);
Vec3 const Vec3::One(1, 1, 1);
Vec3 const Vec3::Zero(0, 0, 0);
Vec3 const Vec3::Plane(1, 0, 1);

Vec3::Vec3 ()
    :m_x{0}, m_y{0}, m_z{0}
{}
Vec3::Vec3 (float const p_value)
: m_x{p_value}, m_y{p_value}, m_z{p_value}
{}
Vec3::Vec3 (float const p_m_x, float const p_m_y, float const p_m_z)
: m_x{p_m_x}, m_y{p_m_y}, m_z{p_m_z}
{}
Vec3::Vec3 (Vec3 const& p_from, Vec3 const& p_to)
: m_x{p_to.m_x-p_from.m_x}, m_y{p_to.m_y-p_from.m_y}, m_z{p_to.m_z-p_from.m_z}
{}
Vec3::Vec3 (Vec2 const& p_vector2, float const p_m_z)
: m_x{p_vector2.m_x}, m_y{p_vector2.m_y}, m_z{p_m_z}
{}
Vec3::Vec3 (Vec3 const& p_copm_y)
: m_x{p_copm_y.m_x}, m_y{p_copm_y.m_y}, m_z{p_copm_y.m_z}
{}

// Methods static
float       Vec3::AngleDeg (Vec3 const& p_from, Vec3 const& p_to) //(Angle SIGNE (entre 0 et 180) en degree !)
{
    return Maths::RadToDeg( AngleRad( p_from, p_to ));
}
float       Vec3::AngleRad (Vec3 const& p_from, Vec3 const& p_to) //(Angle SIGNE (entre 0 et 180) en radian !)
{
    return Maths::Acos(Dot(p_from.GetSafeNormalized(), p_to.GetSafeNormalized()));
}
Vec3     Vec3::Clamp  (Vec3 const& p_value, Vec3 const& p_min, Vec3 const& p_Max)
{
	return {Maths::Clamp( p_value.m_x, p_min.m_x, p_Max.m_x ), Maths::Clamp( p_value.m_y, p_min.m_y, p_Max.m_y ), Maths::Clamp( p_value.m_z, p_min.m_z, p_Max.m_z )};
}
Vec3     Vec3::ClampMagnitude (Vec3 const& p_vector, float const p_minMagnitude, float const    p_MaxMagnitude)
{
    float const VecMagnitude = p_vector.GetMagnitude();

	if(Maths::IsNearlyZero(VecMagnitude))
	{
		return Vec3::Zero;
	}

    return p_vector / VecMagnitude * Maths::Clamp( VecMagnitude, p_minMagnitude, p_MaxMagnitude );
}
Vec3     Vec3::ClampMaxMagnitude (Vec3 const& p_vector, float const p_MaxMagnitude)
{
    if(p_MaxMagnitude < MATH_EPSILON)
	{
		return Vec3::Zero;
	}

	float const SquaredLen{ p_vector.GetMagnitudeSquared()};

	if( SquaredLen > Maths::Square(p_MaxMagnitude))
	{
		return p_vector * Maths::InvSqr(SquaredLen) * p_MaxMagnitude;
	}
	else
	{
		return p_vector;
	}
}
Vec3     Vec3::ClampMinMagnitude (Vec3 const& p_vector, float const p_minMagnitude)
{
    if(p_minMagnitude < MATH_EPSILON)
	{
		return Vec3::Zero;
	}
	float const SquaredLen{ p_vector.GetMagnitudeSquared()};

	if( SquaredLen < Maths::Square(p_minMagnitude))
	{
		return p_vector * Maths::InvSqr(SquaredLen) * p_minMagnitude;
	}
	else
	{
		return p_vector;
	}
}
Vec3     Vec3::Cross (Vec3 const& p_vec1, Vec3 const& p_vec2)
{
    return {p_vec1.m_y * p_vec2.m_z - p_vec2.m_y * p_vec1.m_z,
            p_vec1.m_z * p_vec2.m_x - p_vec2.m_z * p_vec1.m_x,
            p_vec1.m_x * p_vec2.m_y - p_vec2.m_x * p_vec1.m_y};
}
float       Vec3::Distance (Vec3 const& p_a, Vec3 const& p_b)
{
    return Maths::Sqrt(DistanceSquared(p_a, p_b));
}
float       Vec3::DistanceSquared (Vec3 const& p_a, Vec3 const& p_b)
{
    return Maths::Square(p_b.m_x - p_a.m_x) + Maths::Square(p_b.m_y - p_a.m_y) + Maths::Square(p_b.m_z - p_a.m_z);
}
float       Vec3::Dot (Vec3 const& p_a, Vec3 const& p_b)
{
    return p_a.m_x * p_b.m_x + p_a.m_y * p_b.m_y + p_a.m_z * p_b.m_z;
}
bool        Vec3::IsCollinear (Vec3 const& p_a,Vec3 const& p_b)
{
    return Cross(p_a, p_b).IsNearlyZero();
}
bool        Vec3::IsNearlyEqual (Vec3 const& p_a, Vec3 const& p_b,float const p_tolerance)
{
    return Maths::IsNearlyEqual(p_a.m_x, p_b.m_x, p_tolerance) && Maths::IsNearlyEqual(p_a.m_y, p_b.m_y, p_tolerance) && Maths::IsNearlyEqual(p_a.m_z, p_b.m_z, p_tolerance); 
}
bool        Vec3::IsOrthogonal (Vec3 const& p_a, Vec3 const& p_b)
{
    return Maths::IsNearlyZero(Dot(p_a, p_b));
}
Vec3     Vec3::Lerp (Vec3 const& p_a, Vec3 const& p_b, float const p_alpha)
{
    if (Vec3::IsNearlyEqual(p_a, p_b))
        return p_b;

	return p_a + (p_b - p_a) * p_alpha;
}
Vec3     Vec3::Max (Vec3 const& p_a, Vec3 const& p_b)
{
    return { Maths::Max(p_a.m_x, p_b.m_x), Maths::Max(p_a.m_y, p_b.m_y), Maths::Max(p_a.m_z, p_b.m_z)};
}
Vec3     Vec3::Min (Vec3 const& p_a, Vec3 const& p_b)
{
    return { Maths::Min(p_a.m_x, p_b.m_x), Maths::Min(p_a.m_y, p_b.m_y), Maths::Min(p_a.m_z, p_b.m_z)};
}
Vec3     Vec3::MoveTowards (Vec3 const& p_current, Vec3 const& p_target,float const    p_MaxDistanceDelta)
{
    Vec3 const CrtToTarget{p_current, p_target};
	float const distance = CrtToTarget.GetMagnitude();

	if(distance <= p_MaxDistanceDelta || Maths::IsNearlyZero(distance))
	{
		return p_target;
	}
    return p_current + CrtToTarget / distance * p_MaxDistanceDelta;
}
Vec3     Vec3::Project (Vec3 const& p_vector, Vec3 const& p_onTarget)
{
    return p_onTarget * ( Dot( p_onTarget, p_vector ) / p_onTarget.GetMagnitudeSquared());
}
Vec3     Vec3::ProjectOnNormal (Vec3 const& p_vector, Vec3 const& p_onNormal)
{
    return p_onNormal * Dot(p_onNormal, p_vector);
}
Vec3     Vec3::ProjectOnPlane (Vec3 const& p_vector, Vec3 const& p_onNormal) //(Plane qui a pour position le (0,0,0))
{
    return p_vector - ProjectOnNormal(p_vector, p_onNormal);
}
Vec3     Vec3::Reflect (Vec3 const& p_direction, Vec3 const& p_normal)
{
	return p_normal * Dot(p_direction, p_normal) * -2 + p_direction;
}
Vec3     Vec3::ReflectInvert (Vec3 const& p_direction, Vec3 const& p_normal)
{
	return  p_normal * Dot(p_direction, p_normal) * 2 - p_direction;
}
float       Vec3::SignedAngleDeg (Vec3 const& p_from, Vec3 const& p_to)
{
    return AngleDeg(p_from, p_to) -180;
}
float       Vec3::SignedAngleRad (Vec3 const& p_from, Vec3 const& p_to)
{
    return Maths::DegToRad( SignedAngleDeg(p_from, p_to) ); 
}

// Member Methods
Vec3     Vec3::GetAbs () const
{
    return {Maths::Abs(m_x),Maths::Abs(m_y),Maths::Abs(m_z)};
}
float       Vec3::GetAbsMax () const
{
    return Maths::Abs(GetMax());
}
float       Vec3::GetAbsMin () const
{
    return Maths::Abs(GetMin());
}
float       Vec3::GetMagnitude() const
{
    return Maths::Sqrt(GetMagnitudeSquared());
}
float       Vec3::GetMagnitudeSquared () const
{
    return m_x*m_x + m_y*m_y + m_z*m_z;
}
float       Vec3::GetMax () const
{
    return Maths::Max3(m_x, m_y, m_z);
}
float       Vec3::GetMin () const
{
    return Maths::Min3(m_x, m_y, m_z);
}
Vec3     Vec3::GetSign () const
{
    return { (m_x >= 0) ? 1.f : -1.f,
             (m_y >= 0) ? 1.f : -1.f,
             (m_z >= 0) ? 1.f : -1.f };
}
Vec3     Vec3::GetSafeNormalized (float const p_tolerance) const
{
    float SquaredLength = GetMagnitudeSquared();

    if( SquaredLength < p_tolerance )
    {
        return Vec3::Zero;
    }
    else if (SquaredLength == 1)
    {
        return *this;
    }
    SquaredLength = Maths::Sqrt(SquaredLength);

    return {m_x/SquaredLength, m_y/SquaredLength, m_z/SquaredLength};
}
Vec3     Vec3::GetUnsafeNormalized () const
{
    float VecLength = GetMagnitude();
    return {m_x/VecLength, m_y/VecLength, m_z/VecLength};
}
bool        Vec3::IsNearlyZero (float const    p_tolerance) const
{
    return Maths::IsNearlyZero(m_x, p_tolerance) && Maths::IsNearlyZero(m_y, p_tolerance) && Maths::IsNearlyZero(m_z, p_tolerance);
}
bool        Vec3::IsNormalized () const
{
    return Maths::IsNearlyEqual(GetMagnitudeSquared(), 1);
}
bool        Vec3::IsUniform (float const p_tolerance) const
{
    return Maths::IsNearlyEqual(m_x, m_y, p_tolerance) && Maths::IsNearlyEqual(m_x, m_z, p_tolerance);
}
bool        Vec3::IsUnit (float const p_lengthSquaredTolerance)  const
{
    return Maths::IsNearlyEqual(GetMagnitudeSquared(), 1, p_lengthSquaredTolerance);
}
bool        Vec3::IsZero () const
{
    return m_x == 0 && m_y == 0 && m_z == 0;   
}
void        Vec3::Normalize (float const p_tolerance)
{
    float SquaredLength = GetMagnitudeSquared();
    if (SquaredLength > p_tolerance)
    {
        SquaredLength = Maths::Sqrt(SquaredLength);

        m_x /= SquaredLength; 
        m_y /= SquaredLength; 
        m_z /= SquaredLength;
    }
}
void        Vec3::Set (float const p_value)
{
    m_x = m_y = m_z = p_value;
}
void        Vec3::Set (Vec2 const& p_vec2, float const p_m_z)
{
    m_x = p_vec2.m_x;
    m_y = p_vec2.m_y;
    m_z = p_m_z;
}
void        Vec3::Set (float const p_m_x, float const p_m_y, float const p_m_z)
{
    m_x = p_m_x;
    m_y = p_m_y;
    m_z = p_m_z;
}
Vec3        Vec3::Sqrt()
{
    return { Maths::Sqrt(m_x) , Maths::Sqrt(m_y) , Maths::Sqrt(m_z) };
}
void        Vec3::ToDirectionAndLength (Vec3& p_outDirection, float& p_outLength)const
{
    p_outLength = GetMagnitude();
	p_outDirection.m_x = 	m_x / p_outLength;
	p_outDirection.m_y = 	m_y / p_outLength;
	p_outDirection.m_z = 	m_z / p_outLength;
}