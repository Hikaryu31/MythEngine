#include "maths/Quaternion.hpp"


Quaternion const Quaternion::Identity(0, 0, 0, 1);

Quaternion::Quaternion()
	:m_i{0}, m_j{0}, m_k{0}, m_r{1}
{
}

Quaternion::Quaternion(float const p_value)
	:m_i{p_value}, m_j{p_value}, m_k{p_value}, m_r{p_value}
{
}

Quaternion::Quaternion(float const p_i,  float const p_j,  float const p_k,  float const p_r)
	:m_i{p_i}, m_j{p_j}, m_k{p_k}, m_r{p_r}
{
}

Quaternion::Quaternion(Vec4 const& p_vec)
	:m_i{p_vec.m_x}, m_j{p_vec.m_y}, m_k{p_vec.m_z}, m_r{p_vec.m_w}
{
}

Quaternion::Quaternion(Vec3 const& p_axis,  float p_angle)
{
	float axisLength = p_axis.GetMagnitude();
	if (axisLength != 0)
	{
		p_angle *= 0.5f;
		p_axis.GetSafeNormalized();

		float cos = Maths::Cos(p_angle);
		float sin = Maths::Sin(p_angle);

		SetAxis(p_axis * sin);
		m_r = cos;

		GetSafeNormalized();
	}
}

Quaternion::Quaternion(Quaternion const& p_copy)
	:m_i{p_copy.m_i}, m_j{p_copy.m_j}, m_k{p_copy.m_k}, m_r{p_copy.m_r}
{
}

Vec3 Quaternion::ToEulerAngles() const
{
	Vec3 angles = Vec3(0.f);

	// roll (x-axis rotation)
	float x0 = 2.f * (m_r * m_i + m_j * m_k);
	float x1 = 1.f - 2.f * (m_i * m_i + m_j * m_j);
	angles.m_x = Maths::Atan2(x0, x1);

	// pitch (y-axis rotation)
	float sinp = 2.f * (m_r * m_j - m_k * m_i);
	sinp = sinp > 1.f ? 1.f : sinp;
	sinp = sinp < -1.f ? -1.f : sinp;
	angles.m_y = Maths::Asin(sinp);

	// yaw (z-axis rotation)
	float z0 = 2.f * (m_r * m_k + m_i * m_j);
	float z1 = 1.f - 2.f * (m_j * m_j + m_k * m_k);
	angles.m_z = Maths::Atan2(z0, z1);

	return angles;
}

Vec3 Quaternion::Foreward() const
{
	return Vec3RotateByQuat((0.f, 0.f, -1.f), *this);
	//return Vec3((2 * (m_i * m_k + m_r * m_j)), (2 * (m_j * m_k - m_r * m_i)), -(1 - 2 * (m_i * m_i + m_j * m_j)));
}

Vec3 Quaternion::Up() const
{
	return Vec3((2 * (m_i * m_j + m_k * m_r)), (1 - 2 * (m_i * m_i + m_k * m_k)), (2 * (m_j * m_k - m_r * m_i)));
}

Vec3 Quaternion::Right() const
{
	return Vec3((1 - 2 * (m_j * m_j + m_k * m_k)), (2 * (m_i * m_j - m_k * m_r)), (2 * (m_i * m_k + m_j * m_r)));
}

Quaternion Quaternion::ApplyTRS(Mat4x4 p_mat)
{
	return
	{
		p_mat * Vec4{m_i, m_j, m_k, m_r}
	};
}

void Quaternion::GetAxisAndAngle(Vec3* p_outAxis, float* p_outAngle)
{
	if (Maths::Abs(m_r) > 1)
		GetSafeNormalized();
	
	*p_outAngle = 2 * Maths::Acos(m_r);
	float ratio = Maths::Sqrt(1 - m_r * m_r);

	if (ratio > MATH_EPSILON_F)
	{
		p_outAxis->m_x = m_i / ratio;
		p_outAxis->m_y = m_j / ratio;
		p_outAxis->m_z = m_k / ratio;
	}
	else
	{
		p_outAxis->m_x = 1;
	}
}

float Quaternion::GetMagnitude()
{
	return{m_i*m_i + m_j* m_j + m_k*m_k + m_r*m_r};
}

float Quaternion::GetSquaredMagnitude()
{
	return Maths::Sqrt(GetMagnitude());
}

Mat4x4 Quaternion::GetRotationMatrix() const
{
	float i2 = m_i * m_i;
	float j2 = m_j * m_j;
	float k2 = m_k * m_k;

	float ik = m_i * m_k;
	float ij = m_i * m_j;
	float jk = m_j * m_k;
	float ir = m_i * m_r;
	float jr = m_j * m_r;
	float kr = m_k * m_r;

	return
	{
		1-2*(j2+k2), 2 * (ij+kr), 2 * (ik-jr), 0,
		2 * (ij-kr), 1-2*(i2+k2), 2 * (jk+ir), 0,
		2 * (ik+jr), 2 * (jk-ir), 1-2*(i2+j2), 0,
		0		   , 0			, 0			 , 1
	};
}

Quaternion Quaternion::GetSafeNormalized()
{
	float length = GetMagnitude();
	if (length == 0)
		length = 1;
	float iLength = 1 / length;

	m_i /= iLength;
	m_j /= iLength;
	m_k /= iLength;
	m_r /= iLength;

	return *this;
}

Quaternion Quaternion::GetUnsafeNormalized()
{
	float length = GetMagnitude();

	m_i /= length;
	m_j /= length;
	m_k /= length;
	m_r /= length;

	return *this;
}

void Quaternion::SetAxis(Vec3 const& p_vec)
{
	m_i = p_vec.m_x;
	m_j = p_vec.m_y;
	m_k = p_vec.m_z;
}


Quaternion Quaternion::GetQuatFromEuler(Vec3 const& p_euler)
{
	return GetQuatFromEuler(p_euler.m_x, p_euler.m_y, p_euler.m_z);
}
Quaternion Quaternion::GetQuatFromEuler(float const& p_pitch, float const& p_yaw, float const& p_roll)
{
	float cosi = Maths::Cos(p_pitch*0.5f);
	float sini = Maths::Sin(p_pitch*0.5f);
	float cosj = Maths::Cos(p_yaw*0.5f);
	float sinj = Maths::Sin(p_yaw*0.5f);
	float cosk = Maths::Cos(p_roll*0.5f);
	float sink = Maths::Sin(p_roll*0.5f);

	return
	{
		sini*cosj*cosk - cosi*sinj*sink,
		cosi*sinj*cosk + sini*cosj*sink,
		cosi*cosj*sink - sini*sinj*cosk,
		cosi*cosj*cosk + sini*sinj*sink
	};
}

Quaternion Quaternion::GetQuatFromRotMat(Mat4x4 p_mat)
{
	Quaternion r;
	r.m_r = (Maths::Sqrt(p_mat.m_mat[0] + p_mat.m_mat[5] + p_mat.m_mat[10] + 1)) / 2;
	r.m_i = (p_mat.m_mat[6] - p_mat.m_mat[9]) / (4 * r.m_r);
	r.m_j = (p_mat.m_mat[8] - p_mat.m_mat[2]) / (4 * r.m_r);
	r.m_k = (p_mat.m_mat[1] - p_mat.m_mat[4]) / (4 * r.m_r);
	return r;
}

Quaternion Quaternion::Lerp(Quaternion const& p_q1, Quaternion const& p_q2, float p_alpha)
{
	return
	{
		p_q1.m_i + p_alpha * (p_q2.m_i - p_q1.m_i),
		p_q1.m_j + p_alpha * (p_q2.m_j - p_q1.m_j),
		p_q1.m_k + p_alpha * (p_q2.m_k - p_q1.m_k),
		p_q1.m_r + p_alpha * (p_q2.m_r - p_q1.m_r)
	};
}

Quaternion Quaternion::Nlerp(Quaternion const& p_q1, Quaternion const& p_q2, float p_alpha)
{
	return Lerp(p_q1, p_q2, p_alpha).GetSafeNormalized();
}

Quaternion Quaternion::Slerp(Quaternion const& p_q1, Quaternion p_q2, float p_alpha)
{
	float cosHalfTheta = p_q1.m_i*p_q2.m_i + p_q1.m_j*p_q2.m_j + p_q1.m_k*p_q2.m_k + p_q1.m_r*p_q2.m_r;

	if (cosHalfTheta < 0)
	{
		p_q2 = -p_q2;
		cosHalfTheta = -cosHalfTheta;
	}

	if (cosHalfTheta >= 1)
		return p_q1;

	if (cosHalfTheta > 0.95f)
		return Nlerp(p_q1, p_q2, p_alpha);
	
	float halfTheta = Maths::Acos(cosHalfTheta);
	float sinHalfTheta = Maths::Sqrt(1-cosHalfTheta*cosHalfTheta);

	if (Maths::Abs(sinHalfTheta) < 0.001f)
		return
		{
			p_q1.m_i * 0.5f + p_q2.m_i * 0.5f,
			p_q1.m_j * 0.5f + p_q2.m_j * 0.5f,
			p_q1.m_k * 0.5f + p_q2.m_k * 0.5f,
			p_q1.m_r * 0.5f + p_q2.m_r * 0.5f
		};

	float ratioA = Maths::Sin((1 - p_alpha) * halfTheta) / sinHalfTheta;
	float ratioB = Maths::Sin(p_alpha * halfTheta) / sinHalfTheta;

	return
	{
		p_q1.m_i * ratioA + p_q2.m_i * ratioB,
		p_q1.m_j * ratioA + p_q2.m_j * ratioB,
		p_q1.m_k * ratioA + p_q2.m_k * ratioB,
		p_q1.m_r * ratioA + p_q2.m_r * ratioB
	};
}

Quaternion Quaternion::Vec3FromTo(Vec3 const& p_from, Vec3 const& p_to)
{
	float cos2Theta = Vec3::Dot(p_from, p_to);
	Vec3 cross = Vec3::Cross(p_from, p_to);

	Quaternion result(cross, 1 + cos2Theta);

	return result.GetSafeNormalized();
}

Vec3 Quaternion::Vec3RotateByQuat(Vec3 const& p_vec, Quaternion const& p_quat)
{
	if (p_quat.m_i == 0 && p_quat.m_j == 0 && p_quat.m_k == 0 && p_quat.m_r == 0)
		return p_vec;

	return
	{
		p_vec.m_x * (p_quat.m_i * p_quat.m_i + p_quat.m_r * p_quat.m_r - p_quat.m_j * p_quat.m_j - p_quat.m_k * p_quat.m_k) + p_vec.m_y * (2 * p_quat.m_i * p_quat.m_j - 2 * p_quat.m_r * p_quat.m_k) + p_vec.m_z * (2 * p_quat.m_i * p_quat.m_k + 2 * p_quat.m_r * p_quat.m_j),
		p_vec.m_x * (2 * p_quat.m_r * p_quat.m_k + 2 * p_quat.m_i * p_quat.m_j) + p_vec.m_y * (p_quat.m_r * p_quat.m_r - p_quat.m_i * p_quat.m_i + p_quat.m_j * p_quat.m_j - p_quat.m_k * p_quat.m_k) + p_vec.m_z * (-2 * p_quat.m_r * p_quat.m_i + 2 * p_quat.m_j * p_quat.m_k),
		p_vec.m_x * (-2 * p_quat.m_r * p_quat.m_j + 2 * p_quat.m_i * p_quat.m_k) + p_vec.m_y * (2 * p_quat.m_r * p_quat.m_i + 2 * p_quat.m_j * p_quat.m_k) + p_vec.m_z * (p_quat.m_r * p_quat.m_r - p_quat.m_i * p_quat.m_i - p_quat.m_j * p_quat.m_j + p_quat.m_k * p_quat.m_k)
	};
}

Mat4x4 Quaternion::TRSQuat(const Vec3& p_pos, Quaternion& p_quat, const Vec3& p_scale)
{
	return  Mat4x4::Translate(p_pos) * p_quat.GetRotationMatrix() * Mat4x4::Scale(p_scale);
}

Quaternion Quaternion::LookAt(const Vec3& sourcePoint, const Vec3& destPoint)
{
	Vec3 forwardVec = (destPoint - sourcePoint).GetSafeNormalized();

	float dot = Vec3::Dot(Vec3::Forward, forwardVec);

	if (Maths::Abs(dot - (-1.0f)) < 0.000001f)
	{
		return Quaternion(Vec3::Up, 3.1415926535897932f);
	}
	if (Maths::Abs(dot - (1.0f)) < 0.000001f)
	{
		return Quaternion::Identity;
	}

	float rotAngle = (float)Maths::Acos(dot);
	Vec3 rotAxis = Vec3::Cross(Vec3::Forward, forwardVec);
	rotAxis.Normalize();
	return CreateFromAxisAngle(rotAxis, rotAngle);
}


Quaternion Quaternion::CreateFromAxisAngle(const Vec3& axis, const float& angle)
{
	float halfAngle = angle * .5f;
	float s = (float)Maths::Sin(halfAngle);
	Quaternion q;
	q.m_i = axis.m_x * s;
	q.m_j = axis.m_y * s;
	q.m_k = axis.m_z * s;
	q.m_r = Maths::Cos(halfAngle);
	return q;
}