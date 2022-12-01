
#include "maths/Vec2.hpp"

Vec2 const Vec2::Right(1, 0);
Vec2 const Vec2::Left(-1, 0);
Vec2 const Vec2::Up(0, 1);
Vec2 const Vec2::Down(0, -1);
Vec2 const Vec2::One(1, 1);
Vec2 const Vec2::Zero(0, 0);

Vec2::Vec2()
	:m_x{0}, m_y{0}
{
}

Vec2::Vec2( Vec2 p_from, Vec2 p_to )
: 	m_x{p_to.m_x - p_from.m_x},
  	m_y{p_to.m_y - p_from.m_y}
{
}

Vec2::Vec2(float p_x, float p_y)
	: m_x{ p_x },
	m_y{ p_y }
{
}
Vec2::Vec2(float p_xy)
	: m_x{ p_xy },
	m_y{ p_xy }
{
}

Vec2::Vec2( Vec2 const& p_copy )
:   m_x{ p_copy.m_x },
	m_y{ p_copy.m_y }
{

}
float Vec2::AngleDeg (Vec2 const& p_from, Vec2 const& p_to)     //(Angle SIGNE (entre -180 et 180) en degree !)
{
    return Maths::RadToDeg( AngleRad( p_from, p_to ));
}
float Vec2::AngleRad (Vec2 const& p_from, Vec2 const& p_to)     //(Angle SIGNE (entre -180 et 180) en radian !)
{
    return Maths::Acos( Dot( p_from, p_to ) / Maths::Sqrt(p_from.SquaredLength() * p_to.SquaredLength()));
}
Vec2 Vec2::Clamp	(Vec2 const& p_value, Vec2 const& p_min, Vec2 const& p_max)
{
	return Vec2( Maths::Clamp( p_value.m_x, p_min.m_x, p_max.m_x ), Maths::Clamp( p_value.m_y, p_min.m_y, p_max.m_y ));
}
Vec2 Vec2::ClampMagnitude (Vec2 const& p_vector, float const p_minMagnitude, float const    p_maxMagnitude)
{
	float const VecMagnitude = p_vector.Length();

	if(Maths::IsNearlyZero(VecMagnitude))
	{
		return Vec2::Zero;
	}

    return Maths::Clamp( VecMagnitude, p_minMagnitude, p_maxMagnitude ) * p_vector / VecMagnitude;
}
Vec2 Vec2::ClampMaxMagnitude (Vec2 const& p_vector, float const p_maxMagnitude)
{
	if(p_maxMagnitude < MATH_EPSILON)
	{
		return Vec2::Zero;
	}

	float const SquaredLen{ p_vector.SquaredLength()};

	if( SquaredLen > Maths::Square(p_maxMagnitude))
	{
		return p_vector * Maths::InvSqr(SquaredLen) * p_maxMagnitude;
	}
	else
	{
		return p_vector;
	}
}
Vec2 Vec2::ClampMinMagnitude (Vec2 const& p_vector, float const p_minMagnitude)
{
	if(p_minMagnitude < MATH_EPSILON)
	{
		return Vec2::Zero;
	}
	float const SquaredLen{ p_vector.SquaredLength()};

	if( SquaredLen < Maths::Square(p_minMagnitude))
	{
		return p_vector * Maths::InvSqr(SquaredLen) * p_minMagnitude;
	}
	else
	{
		return p_vector;
	}
}
float Vec2::Distance (Vec2 const& p_a, Vec2 const& p_b)
{
    return Maths::Sqrt(DistanceSquared(p_a, p_b));
}
float Vec2::DistanceSquared (Vec2 const& p_a, Vec2 const& p_b)
{
    return Maths::Square(p_b.m_x - p_a.m_x)+ Maths::Square(p_b.m_y - p_a.m_y);
}
bool Vec2::IsCollinear (Vec2 const& p_a,Vec2 const& p_b) // p_a finir
{
    return Maths::IsNearlyZero( p_a.m_x * p_b.m_y - p_b.m_x *p_a.m_y );
}
bool Vec2::IsNearlyEqual (Vec2 const& p_a, Vec2 const& p_b,float const p_tolerance)
{
    return Maths::IsNearlyEqual(p_a.m_x, p_b.m_x, p_tolerance) && Maths::IsNearlyEqual(p_a.m_y, p_b.m_y, p_tolerance); 
}
bool Vec2::IsOrthogonal (Vec2 const& p_a,Vec2 const& p_b)
{
    return Maths::IsNearlyZero(Dot(p_a, p_b));
}
Vec2 Vec2::Lerp (Vec2 const& p_a,Vec2 const& p_b,float const p_alpha)
{
	return p_a + (p_b - p_a) * p_alpha;
}
Vec2 Vec2::Max (Vec2 const& p_a, Vec2 const& p_b)
{
    return { Maths::Max(p_a.m_x, p_b.m_x), Maths::Max(p_a.m_y, p_b.m_y) };
}
Vec2 Vec2:: Min(Vec2 const& p_a, Vec2 const& p_b)
{
    return { Maths::Min(p_a.m_x, p_b.m_x), Maths::Min(p_a.m_y, p_b.m_y) };
}
Vec2 Vec2::MoveTowards (Vec2 const& p_current, Vec2 const& p_target,float const    p_maxDistanceDelta)
{
	Vec2 const CrtToTarget{p_current, p_target};
	float const distance = CrtToTarget.SquaredLength();

	if(distance <= p_maxDistanceDelta || Maths::IsNearlyZero(distance))
	{
		return p_target;
	}
    return p_current + CrtToTarget/distance * p_maxDistanceDelta;
}
Vec2 Vec2::Project (Vec2 const& p_vector, Vec2 const& p_onTarget)
{
    return p_onTarget * ( Dot( p_onTarget, p_vector ) / p_onTarget.SquaredLength());
}
Vec2 Vec2::ProjectOnNormal (Vec2 const& p_vector, Vec2 const& p_onNormal)
{
    return p_onNormal * Dot(p_onNormal, p_vector);
}
Vec2 Vec2::Reflect (Vec2 const& p_direction, Vec2 const& p_normal)
{
	return Dot( p_direction, p_normal ) * -2 * p_normal + p_direction;
}
Vec2 Vec2::ReflectInvert (Vec2 const& p_direction, Vec2 const& p_normal)
{
	return Dot(p_direction, p_normal) * 2 * p_normal - p_direction;
}
Vec2 Vec2::RotateDeg (Vec2 const& p_vector, float const    p_angleDeg)
{
	return RotateRad(p_vector, Maths::DegToRad(p_angleDeg));
}
Vec2 Vec2::RotateRad (Vec2 const& p_vector, float const    p_angleRad)
{
	float cosTheta = Maths::Cos(p_angleRad), 
		  sinTheta = Maths::Sin(p_angleRad);

	return { p_vector.m_x * cosTheta - p_vector.m_y * sinTheta, 
		     p_vector.m_x * sinTheta + p_vector.m_y * cosTheta };
}
float Vec2::SignedAngleDeg (Vec2 const& p_from, Vec2 const& p_to)
{
	float angle = Maths::Atan2(p_to.m_y, p_to.m_x) - Maths::Atan2(p_from.m_y, p_from.m_x);
	if (angle > PI)
	{
		angle -= TAU;
	}
	else if (angle <= -PI)
	{
		angle += TAU;
	}
	return angle;
}
float Vec2::SignedAngleRad (Vec2 const& p_from, Vec2 const& p_to)
{
    return Maths::DegToRad( SignedAngleDeg(p_from, p_to) ); 
}
float Vec2::SquaredLength() const
{
    return m_x*m_x + m_y*m_y;
}
float Vec2::Length() const
{
    return Maths::Sqrt( SquaredLength() );
}
float   Vec2::Dot(Vec2 const& p_a, Vec2 const& p_b)
{
    return p_a.m_x * p_b.m_x + p_a.m_y * p_b.m_y;
}

Vec2 Vec2::GetAbs() const
{
	return { Maths::Abs(m_x), Maths::Abs(m_y) };
}
float Vec2::GetAbsMax() const
{
	return Maths::Max(Maths::Abs(m_x), Maths::Abs(m_y));
}
float Vec2::GetAbsMin() const
{
	return Maths::Min(Maths::Abs(m_x), Maths::Abs(m_y));
}
float Vec2::GetMax() const
{
	return Maths::Max(m_x, m_y);
}
float Vec2::GetMin() const
{
	return Maths::Min(m_x, m_y);
}
Vec2 Vec2::GetPerpendicular() const
{
	return { -m_y, m_x };
}
Vec2 Vec2::GetSign() const
{
	return { (m_x >= 0) ? 1.f : -1.f,
			 (m_y >= 0) ? 1.f : -1.f };
}
Vec2 Vec2::GetSafeNormalized(float const p_tolerance) const
{
	float _SquaredLength = SquaredLength();

	if (_SquaredLength < p_tolerance)
	{
		return Vec2::Zero;
	}
	else if (_SquaredLength == 1)
	{
		return *this;
	}
	_SquaredLength = Maths::Sqrt(_SquaredLength);

	return { m_x / _SquaredLength, m_y / _SquaredLength };
}
Vec2 Vec2::GetUnsafeNormalized() const
{
	float VecLength = Length();
	return { m_x / VecLength, m_y / VecLength};
}
bool Vec2::IsNearlyZero(float const    p_tolerance) const
{
	return Maths::IsNearlyZero(m_x, p_tolerance) && Maths::IsNearlyZero(m_y, p_tolerance);

}
bool Vec2::IsNormalized() const
{
	return Maths::IsNearlyEqual(SquaredLength(), 1);

}
bool Vec2::IsUniform(float const p_tolerance) const
{
	return Maths::IsNearlyEqual(m_x, m_y, p_tolerance);
}
bool Vec2::IsUnit(float const p_lengthSquaredTolerance)  const
{
	return Maths::IsNearlyEqual(SquaredLength(), 1, p_lengthSquaredTolerance);
}
bool Vec2::IsZero() const
{
	return m_x == 0 && m_y == 0;
}

void Vec2::Normalize() 
{
    float length = Length();
    m_x = m_x/length;
    m_y = m_y/ length;
}
void Vec2::Set (float const p_value)
{
	m_x = m_y = p_value;
}
void Vec2::Set (float const p_x, float const    p_y)
{
	m_x = p_x;
	m_y = p_y;
}
void Vec2::ToDirectionAndLength (Vec2& p_outDirection, float& p_outLength)const
{
	p_outLength = Length();
	p_outDirection.m_x = 	m_x/p_outLength;
	p_outDirection.m_y = 	m_y/p_outLength;
}
