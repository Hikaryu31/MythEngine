#include "maths/Mat4x4.hpp"

Mat4x4 const Mat4x4::Identity
(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
); 
Mat4x4 const Mat4x4::Zero
(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
);
Mat4x4::Mat4x4()
{
    m_mat[0] = 0; m_mat[4] = 0; m_mat[8]  = 0; m_mat[12] = 0;
    m_mat[1] = 0; m_mat[5] = 0; m_mat[9]  = 0; m_mat[13] = 0;
    m_mat[2] = 0; m_mat[6] = 0; m_mat[10] = 0; m_mat[14] = 0;
    m_mat[3] = 0; m_mat[7] = 0; m_mat[11] = 0; m_mat[15] = 0;
}
Mat4x4::Mat4x4(float const p_m00,float const p_m01,float const p_m02,float const p_m03,
               float const p_m10,float const p_m11,float const p_m12,float const p_m13,
               float const p_m20,float const p_m21,float const p_m22,float const p_m23,
               float const p_m30,float const p_m31,float const p_m32,float const p_m33)
{
        m_mat[0]  = p_m00; m_mat[1]  = p_m01; m_mat[2]  = p_m02; m_mat[3]  = p_m03;
        m_mat[4]  = p_m10; m_mat[5]  = p_m11; m_mat[6]  = p_m12; m_mat[7]  = p_m13;
        m_mat[8]  = p_m20; m_mat[9]  = p_m21; m_mat[10] = p_m22; m_mat[11] = p_m23;
        m_mat[12] = p_m30; m_mat[13] = p_m31; m_mat[14] = p_m32; m_mat[15] = p_m33;

}

Mat4x4::Mat4x4   (Vec3 const&     p_columnX,
                  Vec3 const&     p_columnY,
                  Vec3 const&     p_columnZ,
                  Vec3 const&     p_columnW)
{
    m_mat[0] = p_columnX.m_x; m_mat[4] = p_columnY.m_x; m_mat[8] = p_columnZ.m_x;  m_mat[12] = p_columnW.m_x;
    m_mat[1] = p_columnX.m_y; m_mat[5] = p_columnY.m_y; m_mat[9] = p_columnZ.m_y;  m_mat[13] = p_columnW.m_y;
    m_mat[2] = p_columnX.m_z; m_mat[6] = p_columnY.m_z; m_mat[10] = p_columnZ.m_z; m_mat[14] = p_columnW.m_z;
    m_mat[3] = 0;             m_mat[7] = 0;             m_mat[11] = 0;             m_mat[15] = 1;
}

Mat4x4::Mat4x4   (Vec4 const&     p_columnX,
                  Vec4 const&     p_columnY,
                  Vec4 const&     p_columnZ,
                  Vec4 const&     p_columnW)
{
    m_mat[0] = p_columnX.m_x; m_mat[4] = p_columnY.m_x; m_mat[8] = p_columnZ.m_x;  m_mat[12] = p_columnW.m_x;
    m_mat[1] = p_columnX.m_y; m_mat[5] = p_columnY.m_y; m_mat[9] = p_columnZ.m_y;  m_mat[13] = p_columnW.m_y;
    m_mat[2] = p_columnX.m_z; m_mat[6] = p_columnY.m_z; m_mat[10] = p_columnZ.m_z; m_mat[14] = p_columnW.m_z;
    m_mat[3] = p_columnX.m_w; m_mat[7] = p_columnX.m_w; m_mat[11] = p_columnX.m_w; m_mat[15] = p_columnX.m_w;
}

Mat4x4::Mat4x4   (Mat4x4 const&   p_copy)
{
    m_mat = p_copy.m_mat;
}

float       Mat4x4::Acces(unsigned int line, unsigned int col)
{
    return m_mat[col + line * 4];
}

float&      Mat4x4::Accesor(unsigned int line, unsigned int col)
{
    return m_mat[col + line * 4];
}

bool        Mat4x4::IsNearlyEqual(Mat4x4 const& p_a,
                               Mat4x4 const& p_b,
                               float const        p_tolerance)
{
    return Maths::IsNearlyEqual(p_a.m_mat[0]  , p_b.m_mat [0], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[1]  , p_b.m_mat [1], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[2]  , p_b.m_mat [2], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[3]  , p_b.m_mat [3], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[4]  , p_b.m_mat [4], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[5]  , p_b.m_mat [5], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[6]  , p_b.m_mat [6], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[7]  , p_b.m_mat [7], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[8]  , p_b.m_mat [8], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[9]  , p_b.m_mat [9], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[10] , p_b.m_mat[10], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[11] , p_b.m_mat[11], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[12] , p_b.m_mat[12], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[13] , p_b.m_mat[13], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[14] , p_b.m_mat[14], p_tolerance) &&
           Maths::IsNearlyEqual(p_a.m_mat[15] , p_b.m_mat[15], p_tolerance);
}

Mat4x4      Mat4x4::LookAt(Vec3 const& p_from,
                        Vec3 const& p_to,
                        Vec3 const& p_up)
{
    Vec3 const forward = (p_from - p_to).GetSafeNormalized();
    Vec3 const right = Vec3::Cross(p_up, forward).GetSafeNormalized();
    Vec3 const up = Vec3::Cross(forward, right);
    Mat4x4 lookAt
    (
        right.m_x  , right.m_y  , right.m_z  , 0,
        up.m_x     , up.m_y     , up.m_z     , 0,
        forward.m_x, forward.m_y, forward.m_z, 0,
        p_from.m_x , p_from.m_y , p_from.m_z , 1
    );
    return lookAt;
}

Mat4x4      Mat4x4::Perspective(float const        p_fov,
                                float const        p_aspect,
                                float const        p_near,
                                float const        p_far)
{
    float top = p_near * tan(p_fov / 2.f);
    float right = top * p_aspect;
    return Frustum(-right, right, -top, top, p_near, p_far);
}

Mat4x4      Mat4x4::Perspective(float const        p_fov,
                                float const        p_width,
                                float const        p_height,
                                float const        p_near,
                                float const        p_far)
{
    float p_aspect = p_width / p_height;
    float top = p_near * tan(p_fov / 2.f);
    float right = top * p_aspect;
    return Frustum(-right, right, -top, top, p_near, p_far);
}

Mat4x4 Mat4x4::Orthographique(float const p_fov,
                              float const p_aspect,
                              float const p_near,
                              float const p_far) //not functional
{
    float top = p_near * tan(p_fov / 2.f);
    float right = top * p_aspect;
    return Ortho(right, top, p_near, p_far);
}

Mat4x4 Mat4x4::Orthographique(float const p_fov,
                              float const p_width,
                              float const p_height,
                              float const p_near,
                              float const p_far)
{
    float p_aspect = p_width / p_height;
    float top = p_near * tan(p_fov / 2.f);
    float right = top * p_aspect;
    return Ortho(-right, right, -top, top, p_near, p_far);
}


Mat4x4   Mat4x4::ViewDeg(float pitch, float yaw, Vec3 pos)
{
    return Mat4x4::RotationXDeg(pitch) * Mat4x4::RotationYDeg(yaw) * Mat4x4::Translate(-pos);
}

Mat4x4   Mat4x4::ViewRad(float pitch, float yaw, Vec3 pos)
{
    return Mat4x4::RotationXRad(pitch) * Mat4x4::RotationYRad(yaw) * Mat4x4::Translate(-pos);
}

Mat4x4      Mat4x4::Frustum(float l, float r, float b, float t, float n, float f)
{
    Mat4x4 frustum
    (
        (n * 2.f) / (r - l), 0.f                , 0.f                     , 0.f,
        0.f                , (n * 2.f) / (t - b), 0.f                     , 0.f,
        (r + l) / (r - l)  , (t + b) / (t - b)  , -(f + n) / (f - n)      , -1.f,
        0.f                , 0.f                , (-2.f * f * n) / (f - n), 0.f
    );
    return frustum;
}

Mat4x4      Mat4x4::Ortho(float const        p_left,
                          float const        p_right,
                          float const        p_top,
                          float const        p_bottom,
                          float const        p_near,
                          float const        p_far)
{
    Mat4x4 ortho
    (
        2 / (p_right - p_left), 0                     , 0                    , -(p_right + p_left) / (p_right - p_left),
        0                     , 2 / (p_top - p_bottom), 0                    , -(p_top + p_bottom) / (p_top - p_bottom),
        0                     , 0                     , -2 / (p_far - p_near), -(p_far + p_near) / (p_far - p_near),
        0                     , 0                     , 0                    , 1
    );
    return ortho;
}



Mat4x4      Mat4x4::Ortho(float const        p_width,
                          float const        p_height,
                          float const        p_near,
                          float const        p_far)
{
    float aspect = p_width / p_height;
    Mat4x4 ortho
    (
        2 / p_width, 0,            0, 0,
        0,           2 / p_height, 0, 0,
        0,           0,            1, 0,
       -1,          -1,            0, 1
    );
    return ortho;
}
Mat4x4      Mat4x4::OrthoScreenToGL(float const        p_width,
                                    float const        p_height)
{
    float aspect = p_width / p_height;
    Mat4x4 ortho
    (
        2 / p_width, 0,            0, 0,
        0,           2 / p_height, 0, 0,
        0,           0,            1, 0,
       -1,          -1,            0, 1
    );
    return ortho;
}

void        Mat4x4::PrintMat()
{
    std::cout << m_mat[0]  << " | " << m_mat[1]  << " | " << m_mat[2]  << " | " << m_mat[3] << std::endl;
    std::cout << m_mat[4]  << " | " << m_mat[5]  << " | " << m_mat[6]  << " | " << m_mat[7] << std::endl;
    std::cout << m_mat[8]  << " | " << m_mat[9]  << " | " << m_mat[10] << " | " << m_mat[11] << std::endl;
    std::cout << m_mat[12] << " | " << m_mat[13] << " | " << m_mat[14] << " | " << m_mat[15] << std::endl;
}

Mat4x4      Mat4x4::RotateDeg(Vec3 const& p_rotate)
{
    return RotateRad(p_rotate * (float)DEGTORAD);
}
Mat4x4      Mat4x4::RotateDeg(float const& p_x, float const& p_y, float const& p_z)
{
    return RotateRad(p_x * (float)DEGTORAD, p_y * (float)DEGTORAD, p_z * (float)DEGTORAD);
}

Mat4x4      Mat4x4::RotateRad(Vec3 const& p_rotate)
{
    return  RotationXRad(p_rotate.m_x) * RotationYRad(p_rotate.m_y) * RotationZRad(p_rotate.m_z);
}

Mat4x4      Mat4x4::RotateRad(float const& p_x, float const& p_y, float const& p_z)
{
    return  RotationXRad(p_x) * RotationYRad(p_y) * RotationZRad(p_z);
}

Mat4x4      Mat4x4::RotationXDeg(float const        p_angleDeg)
{
    return RotationXRad(p_angleDeg * (float)DEGTORAD);
}

Mat4x4      Mat4x4::RotationXRad(float const        p_angleRad)
{
    float Cos = Maths::Cos(p_angleRad);
    float Sin = Maths::Sin(p_angleRad);
    Mat4x4 rotX
    (
        1, 0   , 0   ,0,
        0, Cos , Sin,0,
        0, -Sin , Cos ,0,
        0, 0   , 0   ,1
    );
    return rotX;
}

Mat4x4      Mat4x4::RotationYDeg(float const        p_angleDeg)
{
    return RotationYRad(p_angleDeg * (float)DEGTORAD);
}

Mat4x4      Mat4x4::RotationYRad(float const        p_angleRad)
{
    float Cos = Maths::Cos(p_angleRad);
    float Sin = Maths::Sin(p_angleRad);
    Mat4x4 rotY
    (
        Cos , 0, -Sin , 0,
        0   , 1, 0   , 0,
        Sin, 0, Cos , 0,
        0   , 0, 0   , 1
    );
    return rotY;
}

Mat4x4      Mat4x4::RotationZDeg(float const        p_angleDeg)
{
    return RotationZRad(p_angleDeg * (float)DEGTORAD);
}

Mat4x4      Mat4x4::RotationZRad(float const        p_angleRad)
{
    float Cos = Maths::Cos(p_angleRad);
    float Sin = Maths::Sin(p_angleRad);
    Mat4x4 rotZ
    (
        Cos, Sin, 0, 0,
        -Sin, Cos , 0, 0,
        0  , 0   , 1, 0,
        0  , 0   , 0, 1
    );
    return rotZ;
}

Mat4x4      Mat4x4::Scale(Vec3 const& p_scale)
{
    Mat4x4 scale
    (
        p_scale.m_x, 0, 0, 0,
        0, p_scale.m_y, 0, 0,
        0, 0, p_scale.m_z, 0,
        0, 0, 0        , 1
    );
    return scale;
}

Mat4x4      Mat4x4::Translate(Vec3 const& p_translation)
{
    Mat4x4 translate
    (
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        p_translation.m_x, p_translation.m_y, p_translation.m_z, 1
    );
    return translate;
}

Mat4x4      Mat4x4::Translate(float const& p_x, float const& p_y, float const& p_z)
{
    Mat4x4 translate
    (
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        p_x, p_y, p_z, 1
    );
    return translate;
}

Mat4x4      Mat4x4::TRSDeg(Vec3 const& p_translation,
                        Vec3 const& p_rotation,
                        Vec3 const& p_scale)
{
    return   Translate(p_translation)* RotateDeg(p_rotation) * Scale(p_scale);
}

Mat4x4      Mat4x4::TRSRad(Vec3 const& p_translation,
                        Vec3 const& p_rotation,
                        Vec3 const& p_scale)

{
    return  Translate(p_translation) * RotateRad(p_rotation) * Scale(p_scale);
}

//local methods

Vec3         Mat4x4::GetAxis(unsigned int const    p_index) const
{
    if (p_index < 0 || p_index > 2)
    {
        throw std::out_of_range("Index is out of range, should be between 0 and 2");
    }
    int line = p_index * 4;
    return
    {
        m_mat[0 + line], m_mat[1 + line], m_mat[2 + line]
    };
}

Vec3         Mat4x4::GetBackward() const
{
    return GetScaledBackward().GetSafeNormalized();
}

Mat4x4          Mat4x4::GetCofactor() const
{
    Mat4x4 Cofactors;

    Cofactors.m_mat[0]  =   m_mat[5] * (m_mat[10] * m_mat[15] - m_mat[14] * m_mat[11]) - m_mat[9] * (m_mat[6] * m_mat[15] - m_mat[14] * m_mat[7]) + m_mat[13] * (m_mat[6] * m_mat[11] - m_mat[10] * m_mat[7]);
    Cofactors.m_mat[1]  = -(m_mat[4] * (m_mat[10] * m_mat[15] - m_mat[14] * m_mat[11]) - m_mat[8] * (m_mat[6] * m_mat[15] - m_mat[14] * m_mat[7]) + m_mat[12] * (m_mat[6] * m_mat[11] - m_mat[10] * m_mat[7]));
    Cofactors.m_mat[2]  =   m_mat[7] * (m_mat[8]  * m_mat[13] - m_mat[12] * m_mat[9]) - m_mat[11] * (m_mat[4] * m_mat[13] - m_mat[12] * m_mat[5]) + m_mat[15] * (m_mat[4] * m_mat[9]  - m_mat[8] * m_mat[5]);
    Cofactors.m_mat[3]  = -(m_mat[6] * (m_mat[8]  * m_mat[13] - m_mat[12] * m_mat[9]) - m_mat[10] * (m_mat[4] * m_mat[13] - m_mat[12] * m_mat[5]) + m_mat[14] * (m_mat[4] * m_mat[9]  - m_mat[8] * m_mat[5]));

    Cofactors.m_mat[4]  = -(m_mat[1] * (m_mat[10] * m_mat[15] - m_mat[14] * m_mat[11]) - m_mat[9] * (m_mat[2] * m_mat[15] - m_mat[14] * m_mat[3])  + m_mat[13] * (m_mat[2] * m_mat[11] - m_mat[10] * m_mat[3]));
    Cofactors.m_mat[5]  =   m_mat[0] * (m_mat[10] * m_mat[15] - m_mat[14] * m_mat[11]) - m_mat[8] * (m_mat[2] * m_mat[15] - m_mat[14] * m_mat[3])  + m_mat[12] * (m_mat[2] * m_mat[11] - m_mat[10] * m_mat[3]);
    Cofactors.m_mat[6]  = -(m_mat[3] * (m_mat[8]  * m_mat[13] - m_mat[12] * m_mat[9])  - m_mat[11] * (m_mat[0] * m_mat[13] - m_mat[12] * m_mat[1]) + m_mat[15] * (m_mat[0] * m_mat[9]  - m_mat[8] * m_mat[1]));
    Cofactors.m_mat[7]  =   m_mat[2] * (m_mat[8]  * m_mat[13] - m_mat[12] * m_mat[9])  - m_mat[10] * (m_mat[0] * m_mat[13] - m_mat[12] * m_mat[1]) + m_mat[14] * (m_mat[0] * m_mat[9]  - m_mat[8] * m_mat[1]);
    
    Cofactors.m_mat[8]  =   m_mat[1] * (m_mat[6] * m_mat[15] - m_mat[14] * m_mat[7]) - m_mat[5] * (m_mat[2] * m_mat[15] - m_mat[14] * m_mat[3]) + m_mat[13] * (m_mat[2] * m_mat[7] - m_mat[6] * m_mat[3]);
    Cofactors.m_mat[9]  = -(m_mat[0] * (m_mat[6] * m_mat[15] - m_mat[14] * m_mat[7]) - m_mat[4] * (m_mat[2] * m_mat[15] - m_mat[14] * m_mat[3]) + m_mat[12] * (m_mat[2] * m_mat[7] - m_mat[6] * m_mat[3]));
    Cofactors.m_mat[10] =   m_mat[3] * (m_mat[4] * m_mat[13] - m_mat[12] * m_mat[5]) - m_mat[7] * (m_mat[0] * m_mat[13] - m_mat[12] * m_mat[1]) + m_mat[15] * (m_mat[0] * m_mat[5] - m_mat[4] * m_mat[1]);
    Cofactors.m_mat[11] = -(m_mat[2] * (m_mat[4] * m_mat[13] - m_mat[12] * m_mat[5]) - m_mat[6] * (m_mat[0] * m_mat[13] - m_mat[12] * m_mat[1]) + m_mat[14] * (m_mat[0] * m_mat[5] - m_mat[4] * m_mat[1]));
    
    Cofactors.m_mat[12] = -(m_mat[1] * (m_mat[6] * m_mat[11] - m_mat[10] * m_mat[7]) - m_mat[5] * (m_mat[2] * m_mat[11] - m_mat[10] * m_mat[3]) + m_mat[9]  * (m_mat[2] * m_mat[7] - m_mat[6] * m_mat[3]));
    Cofactors.m_mat[13] =   m_mat[0] * (m_mat[6] * m_mat[11] - m_mat[10] * m_mat[7]) - m_mat[4] * (m_mat[2] * m_mat[11] - m_mat[10] * m_mat[3]) + m_mat[8]  * (m_mat[2] * m_mat[7] - m_mat[6] * m_mat[3]) ;
    Cofactors.m_mat[14] = -(m_mat[3] * (m_mat[4] * m_mat[9]  - m_mat[8] * m_mat[5])  - m_mat[7] * (m_mat[0] * m_mat[9]  - m_mat[8] * m_mat[1])  + m_mat[11] * (m_mat[0] * m_mat[5] - m_mat[4] * m_mat[1]));
    Cofactors.m_mat[15] =   m_mat[2] * (m_mat[4] * m_mat[9]  - m_mat[8] * m_mat[5])  - m_mat[6] * (m_mat[0] * m_mat[9]  - m_mat[8] * m_mat[1])  + m_mat[10] * (m_mat[0] * m_mat[5] - m_mat[4] * m_mat[1]);

    return Cofactors;
}

Vec4         Mat4x4::GetColumn(unsigned int const    p_index) const
{
    return
    {
        m_mat[0 + p_index * 4],
        m_mat[1 + p_index * 4],
        m_mat[2 + p_index * 4],
        m_mat[3 + p_index * 4]
    };
}

float           Mat4x4::GetDeterminant() const
{
    return
        m_mat[0] * (m_mat[5] * (m_mat[10] * m_mat[15] - m_mat[14] * m_mat[11]) - m_mat[9] * (m_mat[6] * m_mat[15] - m_mat[14] * m_mat[7]) + m_mat[13] * (m_mat[6] * m_mat[11] - m_mat[10] * m_mat[7])) -
        m_mat[1] * (m_mat[4] * (m_mat[10] * m_mat[15] - m_mat[14] * m_mat[11]) - m_mat[8] * (m_mat[6] * m_mat[15] - m_mat[14] * m_mat[7]) + m_mat[12] * (m_mat[6] * m_mat[11] - m_mat[10] * m_mat[7])) +
        m_mat[2] * (m_mat[7] * (m_mat[8] * m_mat[13] - m_mat[12] * m_mat[9]) - m_mat[11] * (m_mat[4] * m_mat[13] - m_mat[12] * m_mat[5]) + m_mat[15] * (m_mat[4] * m_mat[9] - m_mat[8] * m_mat[5])) -
        m_mat[3] * (m_mat[6] * (m_mat[8] * m_mat[13] - m_mat[12] * m_mat[9]) - m_mat[10] * (m_mat[4] * m_mat[13] - m_mat[12] * m_mat[5]) + m_mat[14] * (m_mat[4] * m_mat[9] - m_mat[8] * m_mat[5]));
}

Vec3         Mat4x4::GetRotationDeg() const
{
    Vec3 rot = GetRotationRad() * RADTODEG;
    std::cout << "Vec3: " << rot.m_x << " , " << rot.m_y << " , " <<  rot.m_z << std::endl;
    return rot;
}
Vec3         Mat4x4::GetRotationRad() const
{
    //float Denom = Maths::Sqrt(Maths::Square(m_mat[9] - m_mat[6]) + 
    //                          Maths::Square(m_mat[2] - m_mat[8]) + 
    //                          Maths::Square(m_mat[4] - m_mat[1]));
    //return
    //{
    //    (m_mat[9] - m_mat[6]) / Denom,
    //    (m_mat[2] - m_mat[8]) / Denom,
    //    (m_mat[4] - m_mat[1]) / Denom
    //};

    //return
    //{
    //    Maths::Atan2(-m_mat[8], m_mat[0]),
    //    Maths::Asin(4),
    //    Maths::Atan2(m_mat[6], m_mat[5])
    //};

    if (Maths::Abs(m_mat[0]) == 1.0f)
    {
        return { Maths::Atan2(m_mat[2], m_mat[11]), 0, 0 };
    }
    else
    {
        return{
            Maths::Atan2(-m_mat[8], m_mat[0]),
            Maths::Asin(m_mat[4]), 
            Maths::Atan2(-m_mat[6], m_mat[5])
        };
    }
}

Vec3         Mat4x4::GetDown() const
{
    return GetScaledDown().GetSafeNormalized();
}

Vec3         Mat4x4::GetForward() const
{
    return GetScaledForward().GetSafeNormalized();
}

Mat4x4          Mat4x4::GetInverse()                                              const
{
    float det = GetDeterminant();
    if (det == 0)
    {       
        return Mat4x4::Zero;
    }
    return GetCofactor().GetTransposed() / det;
}

Vec3         Mat4x4::GetLeft()                                              const
{
    return GetScaledLeft().GetSafeNormalized();
}

Mat4x4          Mat4x4::GetMatrixWithoutScale(float const    p_tolerance)   const
{
    Mat4x4 NewMat(*this);
    NewMat.RemoveScaling();
    return NewMat;
}

float           Mat4x4::GetMaximumAxisScale()                                              const
{
    return Maths::Sqrt(Maths::Max3( GetScaledRight().GetMagnitudeSquared(), 
                        GetScaledUp().GetMagnitudeSquared(), 
                        GetScaledForward().GetMagnitudeSquared()));
}

Vec3         Mat4x4::GetOrigin()                                              const
{
    return { m_mat[12], m_mat[13], m_mat[14] };
}

Vec3         Mat4x4::GetRight()                                              const
{
    return GetScaledRight().GetSafeNormalized();
}

//Vec3         GetRotationDeg()                                              const;
//Vec3         GetRotationRad()                                              const;

float           Mat4x4::GetRotationDeterminant()                                              const
{
    return m_mat[0] * (m_mat[5] * m_mat[10] - m_mat[9] * m_mat[6])-
           m_mat[4] * (m_mat[1] * m_mat[10] - m_mat[9] * m_mat[2])+
           m_mat[8] * (m_mat[1] * m_mat[6]  - m_mat[5] * m_mat[2]);
}

Vec4         Mat4x4::GetRow(unsigned int const    p_index)                        const
{
    if (p_index < 0 || p_index > 3)
    {
        throw std::out_of_range("Index is out of range, should be between 0 and 3");
    }
    return
    {
        m_mat[p_index],m_mat[p_index + 4],m_mat[p_index + 8],m_mat[p_index + 12]
    };
}

Vec3         Mat4x4::GetScale(float const    p_tolerance)   const
{
    float   x_SquaredMagnitude = GetScaledRight().GetMagnitudeSquared();
    float   y_SquaredMagnitude = GetScaledUp().GetMagnitudeSquared();
    float   z_SquaredMagnitude = GetScaledForward().GetMagnitudeSquared();

    Vec3 Scales;

    if (x_SquaredMagnitude > p_tolerance)
    {
        Scales.m_x = Maths::Sqrt(x_SquaredMagnitude);
    }

    if (y_SquaredMagnitude > p_tolerance)
    {
        Scales.m_y = Maths::Sqrt(y_SquaredMagnitude);
    }

    if (z_SquaredMagnitude > p_tolerance)
    {
        Scales.m_z = Maths::Sqrt(z_SquaredMagnitude);
    }

    return Scales;
}

void            Mat4x4::GetScaledAxes(Vec3 & p_outXAxis,
                Vec3 & p_outYAxis,
                Vec3 & p_outZAxis)                     const
{
    p_outXAxis = GetScaledRight();
    p_outYAxis = GetScaledUp();
    p_outZAxis = GetScaledForward();
}

Vec3         Mat4x4::GetScaledBackward()                                              const
{
    return -GetScaledForward();
}

Vec3         Mat4x4::GetScaledDown()                                              const
{
    return -GetScaledUp();
}

Vec3         Mat4x4::GetScaledForward()                                              const
{
    return
    {
        m_mat[2], m_mat[6], m_mat[10]
    };
}

Vec3         Mat4x4::GetScaledLeft()                                              const
{
    return -GetScaledRight();
}

Vec3         Mat4x4::GetScaledRight()                                              const
{
    return
    {
        m_mat[0], m_mat[4], m_mat[8]
    };
}

Vec3         Mat4x4::GetScaledUp()                                              const
{
    return
    {
        m_mat[1], m_mat[5], m_mat[9]
    };
}

Mat4x4          Mat4x4::GetTransposed()                                              const
{
    return 
    {
        m_mat[0],  m_mat[4],  m_mat[8],  m_mat[12],
        m_mat[1],  m_mat[5],  m_mat[9],  m_mat[13],
        m_mat[2],  m_mat[6],  m_mat[10], m_mat[14],
        m_mat[3],  m_mat[7],  m_mat[11], m_mat[15]
    };
}

Vec3         Mat4x4::GetUp()                                              const
{
    return GetScaledUp().GetSafeNormalized();
}

std::array<float, 16> Mat4x4::GetValues() const
{
    return m_mat;
}

Vec3         Mat4x4::MultiplyPoint(Vec3 const& p_point)                        const
{
    float const w = m_mat[3] * p_point.m_x + m_mat[7] * p_point.m_y + m_mat[11] * p_point.m_z + m_mat[15];
    return
    {
        (m_mat[0] * p_point.m_x + m_mat[4] * p_point.m_y + m_mat[8] * p_point.m_z + m_mat[12]) / w,
        (m_mat[1] * p_point.m_x + m_mat[5] * p_point.m_y + m_mat[9] * p_point.m_z + m_mat[13]) / w,
        (m_mat[2] * p_point.m_x + m_mat[6] * p_point.m_y + m_mat[10] * p_point.m_z + m_mat[14]) / w,
    };
}

Vec3         Mat4x4::MultiplyPoint3x4(Vec3 const& p_point)                        const
{
    return
    {
        m_mat[0] * p_point.m_x + m_mat[4] * p_point.m_y + m_mat[8] * p_point.m_z + m_mat[12],
        m_mat[1] * p_point.m_x + m_mat[5] * p_point.m_y + m_mat[9] * p_point.m_z + m_mat[13],
        m_mat[2] * p_point.m_x + m_mat[6] * p_point.m_y + m_mat[10] * p_point.m_z + m_mat[14]
    };
}

Vec3         Mat4x4::MultiplyVector(Vec3 const& p_point)                        const
{
    return
    {
        m_mat[0] * p_point.m_x + m_mat[4] * p_point.m_y + m_mat[8] * p_point.m_z,
        m_mat[1] * p_point.m_x + m_mat[5] * p_point.m_y + m_mat[9] * p_point.m_z,
        m_mat[2] * p_point.m_x + m_mat[6] * p_point.m_y + m_mat[10] * p_point.m_z
    };
}

void            Mat4x4::RemoveScaling(float const    p_tolerance)
{
    Vec3 Xaxis = GetRight();
    Vec3 Yaxis = GetUp();
    Vec3 Zaxis = GetForward();

    m_mat[0] = Xaxis.m_x; m_mat[4] = Yaxis.m_x; m_mat[8]  = Zaxis.m_x;
    m_mat[1] = Xaxis.m_y; m_mat[5] = Yaxis.m_y; m_mat[9]  = Zaxis.m_y;
    m_mat[2] = Xaxis.m_z; m_mat[6] = Yaxis.m_z; m_mat[10] = Zaxis.m_z;
}

void            Mat4x4::SetAxis(unsigned int const    p_axisIndex,
                        Vec3 const& p_axis)
{
    SetAxis(p_axisIndex, p_axis.m_x, p_axis.m_y, p_axis.m_z);
}

void            Mat4x4::SetAxis(unsigned int const      p_axisIndex,
    float const    p_x,
    float const    p_y,
    float const    p_z)
{
    if (p_axisIndex < 0 || p_axisIndex > 2)
    {
        throw std::out_of_range("Index is out of range, should be between 0 and 2");
    }
    m_mat[0 + p_axisIndex * 4] = p_x;
    m_mat[1 + p_axisIndex * 4] = p_y;
    m_mat[2 + p_axisIndex * 4] = p_z;
}

void            Mat4x4::SetAxisX(Vec3 const& p_axis)
{
    SetAxisX(p_axis.m_x, p_axis.m_y, p_axis.m_z);
}

void            Mat4x4::SetAxisX(float const    p_x,
                float const    p_y,
                float const    p_z)
{
    m_mat[0] = p_x;
    m_mat[1] = p_y;
    m_mat[2] = p_z;
}

void            Mat4x4::SetAxisY(Vec3 const& p_axis)
{
    SetAxisY(p_axis.m_x, p_axis.m_y, p_axis.m_z);
}

void            Mat4x4::SetAxisY(float const    p_x,
                float const    p_y,
                float const    p_z)
{
    m_mat[4] = p_x;
    m_mat[5] = p_y;
    m_mat[6] = p_z;
}

void            Mat4x4::SetAxisZ(Vec3 const& p_axis)
{
    SetAxisZ(p_axis.m_x, p_axis.m_y, p_axis.m_z);
}
void            Mat4x4::SetAxisZ(float const    p_x,
                float const    p_y,
                float const    p_z)
{
    m_mat[8] = p_x;
    m_mat[9] = p_y;
    m_mat[10] = p_z;
}

void            Mat4x4::SetColumn(unsigned int const    p_index,
                Vec4 const& p_column)
{
    if (p_index < 0 || p_index > 3)
    {
        throw std::out_of_range("Index is out of range, should be between 0 and 3");
    }
    m_mat[0 + p_index * 4] = p_column.m_x;
    m_mat[1 + p_index * 4] = p_column.m_y;
    m_mat[2 + p_index * 4] = p_column.m_z;
    m_mat[3 + p_index * 4] = p_column.m_w;
}

void            Mat4x4::SetRow(unsigned int const    p_index,
                Vec4 const& p_row)
{
    if (p_index < 0 || p_index > 3)
    {
        throw std::out_of_range("Index is out of range, should be between 0 and 3");
    }
    m_mat[p_index] = p_row.m_x;
    m_mat[p_index + 4] = p_row.m_y;
    m_mat[p_index + 8] = p_row.m_z;
    m_mat[p_index + 12] = p_row.m_w;
}

void            Mat4x4::SetOrigin(Vec3 const& p_newOrigin)
{
    SetOrigin(p_newOrigin.m_x, p_newOrigin.m_y, p_newOrigin.m_z);
}

void            Mat4x4::SetOrigin(float const    p_newX,
                float const    p_newY,
                float const    p_newZ)
{
    m_mat[12] = p_newX;
    m_mat[13] = p_newY;
    m_mat[14] = p_newZ;
}