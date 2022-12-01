
#include <Maths/Maths.hpp>

float Maths::Abs (float const p_value)
{
    return fabsf(p_value);
}
float Maths::Acos (float const p_value)
{
    return acosf(p_value);
}

float Maths::AcosH (float const p_value)
{
    return acoshf(p_value);
}
float Maths::Asin (float const p_value)
{
    return asinf(p_value);
}
float Maths::AsinH (float const p_value)
{
    return asinhf(p_value);
}
float Maths::Atan2 (float const p_y, float p_x)
{
    return atan2f(p_y, p_x);
}
float Maths::Atan (float const p_value)
{
    return atanf(p_value);
}
float Maths::AtanH (float const p_value)
{
    return atanhf(p_value);
}
float Maths::Cbrt (float const p_value) // (C'est la racine cubique (Racine de 3))
{
    return cbrtf(p_value);
}
float Maths::Ceil (float const p_value)
{
    return ceilf(p_value);
}
float Maths::Clamp (float const p_value, float p_min, float p_max)
{
    return (p_value < p_min ) ? p_min : (p_value > p_max) ? p_max : p_value;
}
float Maths::Cos (float const p_value)
{
    return cosf(p_value);
}
float Maths::CosH (float const p_value)
{
    return coshf(p_value);
}
float Maths::Cube (float const p_value) //(Puissance 3)
{
    return p_value * p_value * p_value;
}
float Maths::DegToRad (float const p_value)
{
    return p_value * DEGTORAD;
}
float Maths::DeltaAngleDeg (float const current, float const p_target) // (Angle en deg  ]-180,180] et valeur retourne en Deg)
{
    float output = Repeat( p_target - current, 360 );
    if(output > 180)
    {
        output -= 360;
    }
    return output;
}
float Maths::DeltaAngleRad (float const current, float const p_target) // (Angle en deg  ]-PI,PI] et valeur retourne en Rad)
{
    float output = Repeat( p_target - current, TAU );
    if(output > PI)
    {
        output -= TAU;
    }
    return output;
}
float Maths::Exp (float const p_value)
{
    return expf(p_value);
}
float Maths::Ex2 (float const p_value)
{
    return exp2f(p_value);
}
float Maths::Floor (float const p_value)
{
    return floorf(p_value);
}
float Maths::Fmod (float const p_value, float const p_modulo)
{
    return fmodf(p_value, p_modulo);
}
int   Maths::GetSigni(int value)
{
    if(value >= 0)
        return 1;
    return -1;
}
int   Maths::GetSignf(float value)
{
    if(value >= 0)
        return 1;
    return -1;
}
int   Maths::GetSignd(double value)
{
    if (value >= 0)
        return 1;
    return -1;
}
float Maths::InverseLerp (float const p_a, float p_b, float p_value) // (ca donne le pourcentage : p_a == p_value : 0% soit 0.0f et p_b == p_value : 100% soit 1.0f)
{
    return (p_value - p_a)/(p_b - p_a);
}
float Maths::InvSqr (float const p_value) // (1 / racine(value))
{
    return 1/sqrtf(p_value);
}
bool Maths::IsNearlyEqual (float p_a, float p_b, float p_tolerance)
{
    return Abs(p_a - p_b) <= p_tolerance;
}
bool Maths::IsNearlyZero(float p_value, float p_tolerance)
{
    return Abs(p_value) <= p_tolerance;
}
float Maths::Lerp (float const p_a, float p_b, float p_value)
{
    return p_a + (p_b - p_a) * p_value;
}
float Maths::Log2 (float const p_value)
{
    return log2f(p_value);
}
float Maths::Log10 (float const p_value)
{
    return log10f(p_value);
}
float Maths::Ln (float const p_value) // logarithme neperien
{
    return logf(p_value);
}
float Maths::LogX (float const p_value, float const p_exponent)
{
    return Ln(p_value) / Ln(p_exponent);
}
float Maths::Max (float const p_a, float const p_b)
{
    return ( p_a > p_b ) ? p_a : p_b;
}
float Maths::Max3 (float const p_a, float const p_b, float const p_c)
{
    return Max( Max( p_a, p_b ), p_c );
}
float Maths::Min (float const p_a, float const p_b)
{
    return ( p_a < p_b ) ? p_a : p_b;
}
float Maths::Min3 (float const p_a, float const p_b, float const p_c)
{
    return Min( Min( p_a, p_b ), p_c );
}
float Maths::Modf (float const p_value, float* p_IntPart)
{
    return modff(p_value, p_IntPart);
}
float Maths::MoveTowards (float const p_current, float const p_target, float p_maxDelta)
{
    if( Abs( p_target - p_current ) <= p_maxDelta )
    {
        return p_target;
    }
    return p_current + (p_target - p_current) * p_maxDelta;
}
float Maths::PingPong (float p_value, float length)
{
    return length - Abs( Repeat( p_value, 2.f * length ) - length );
}
float Maths::Root (float p_base, float p_exponent)
{
    return powf(p_base, 1/p_exponent);
}
float Maths::Pow (float p_base, float p_exponent)
{
    return powf(p_base, p_exponent);
}
float Maths::RadToDeg (float const p_value)
{
    return p_value * RADTODEG;
}
float Maths::Repeat (float p_value, float length)
{ 
    return p_value - Floor(p_value / length) * length;
}
float Maths::Round (float const p_value)
{
    return roundf(p_value);
}
float Maths::Sin (float const p_value)
{
    return sinf(p_value);
}
float Maths::SinH (float const p_value)
{
    return sinhf(p_value);
}
float Maths::Square (float const p_value) // (value * value)
{
    return p_value * p_value;
}
float Maths::Sqrt (float const p_value)
{
    return sqrtf(p_value);
}
float Maths::Tan (float const p_value)
{
    return tanf(p_value);
}
float Maths::TanH (float const p_value)
{
    return tanhf(p_value);
}

float Maths::Mod(float Value, float Base)
{
    return fmodf(Value, Base);
}
