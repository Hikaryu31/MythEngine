#ifndef _MATHS_HPP_
#define _MATHS_HPP_
#include "math.h"

#include <utility>

#define MATH_EPSILON	0.00000000001
#define MATH_EPSILON_F  0.0001f
#define HALFPI          1.57079632679f
#define PI              3.14159265359f
#define TAU             6.28318530718f

#define DEGTORAD		0.01745329251f
#define RADTODEG		57.2957795131f

struct Maths
{
    static float Abs (float const p_value);
    static float Acos (float const p_value);
    static float AcosH (float const p_value);
    static float Asin (float const p_value);
    static float AsinH (float const p_value);
    static float Atan2 (float const p_y, float p_x);
    static float Atan (float const p_value);
    static float AtanH (float const p_value);
    static float Cbrt (float const p_value); // (C'est la racine cubique (Racine de 3))
    static float Ceil (float const p_value);
    static float Clamp (float const p_value, float p_min = 0.0f, float p_max = 0.0f);
    static float Cos (float const p_value);
    static float CosH (float const p_value);
    static float Cube (float const p_value); //(Puissance 3)
    static float DegToRad (float const p_value);
    static float DeltaAngleDeg (float const current, float const p_target); // (Angle en deg  ]-180,180] et valeur retourne en Deg);
    static float DeltaAngleRad (float const current, float const p_target); // (Angle en deg  ]-PI,PI] et valeur retourne en Deg);
    static float Exp (float const p_value);
    static float Ex2 (float const p_value);
    static float Floor (float const p_value);
    static float Fmod (float const p_value, float const p_modulo);
    static int   GetSigni(int);
    static int   GetSignf(float);
    static int   GetSignd(double);
    static float InverseLerp (float const p_a, float p_b, float p_value); // (ca donne le pourcentage : p_a == p_value : 0% soit 0.0f et p_b == p_value : 100% soit 1.0f);
    static float InvSqr (float const p_value); // (1 / racine(value));
    static bool  IsNearlyEqual (float p_a, float p_b, float p_tolerance = MATH_EPSILON);
    static bool  IsNearlyZero (float p_value, float p_tolerance = MATH_EPSILON);
    static float Lerp (float const p_a, float p_b, float p_value);
    static float Log2 (float const p_value);
    static float Log10 (float const p_value);
    static float Ln (float const p_value);
    static float LogX (float const p_value, float const p_exponent);
    static float Max (float const p_a, float const p_b);
    static float Max3 (float const p_a, float const p_b, float const p_c);
    static float Min (float const p_a, float const p_b);
    static float Min3 (float const p_a, float const p_b, float const p_c);
    static float Modf (float const p_value, float* p_IntPart);
    static float MoveTowards (float const p_current, float const p_target, float p_maxDelta);
    static float PingPong (float p_value, float length);
    static float Root (float p_base, float p_exponent);
    static float Pow (float p_base, float p_exponent);
    static float RadToDeg (float const p_value);
    static float Repeat (float p_value, float length);
    static float Round (float const p_value);
    static float Sin (float const p_value);
    static float SinH (float const p_value);
    static float Square (float const p_value); // (value * value);
    static float Sqrt (float const p_value);
    static float Tan (float const p_value);
    static float TanH (float const p_value);
    static float Mod(float Value, float Base);
    template<typename tVal>
    static tVal map_value(std::pair<tVal, tVal> a, std::pair<tVal, tVal> b, tVal inVal);
};


template<typename tVal>
tVal  Maths::map_value(std::pair<tVal, tVal> a, std::pair<tVal, tVal> b, tVal inVal)
{
    tVal inValNorm = inVal - a.first;
    tVal aUpperNorm = a.second - a.first;
    tVal normPosition = inValNorm / aUpperNorm;

    tVal bUpperNorm = b.second - b.first;
    tVal bValNorm = normPosition * bUpperNorm;
    tVal outVal = b.first + bValNorm;

    return outVal;
}

#endif