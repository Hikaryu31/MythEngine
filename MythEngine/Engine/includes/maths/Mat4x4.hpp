#ifndef _MATRIX_4X4_HPP_
#define _MATRIX_4X4_HPP_
#include "maths/Vec4.hpp"
#include <array>


class Mat4x4
{
public:

    std::array<float, 16> m_mat;

    static  const   Mat4x4  Identity,
                            Zero;

    Mat4x4   ();

    Mat4x4   (float const   p_m00, float const  p_m01, float const  p_m02, float const  p_m03,
              float const   p_m10, float const  p_m11, float const  p_m12, float const  p_m13,
              float const   p_m20, float const  p_m21, float const  p_m22, float const  p_m23,
              float const   p_m30, float const  p_m31, float const  p_m32, float const  p_m33);

    Mat4x4   (Vec3 const&     p_columnX,
              Vec3 const&     p_columnY,
              Vec3 const&     p_columnZ,
              Vec3 const&     p_columnW);

    Mat4x4   (Vec4 const&     p_columnX,
              Vec4 const&     p_columnY,
              Vec4 const&     p_columnZ,
              Vec4 const&     p_columnW);

    Mat4x4   (Mat4x4 const&   p_copy);


    ~Mat4x4  () = default;

    float    Acces(unsigned int line, unsigned int col);

    float&    Accesor(unsigned int line, unsigned int col);

    static bool     IsNearlyEqual(Mat4x4 const& p_a,
                                  Mat4x4 const& p_b,
                                  float const        p_tolerance = MATH_EPSILON_F);

    static Mat4x4   LookAt(Vec3 const& p_from,
                           Vec3 const& p_to,
                           Vec3 const& p_up = Vec3::Up);

    static Mat4x4   Perspective(float const        p_fov,
                                float const        p_aspect,
                                float const        p_near,
                                float const        p_far);

    static Mat4x4   Perspective(float const        p_fov,
                                float const        p_width,
                                float const        p_height,
                                float const        p_near,
                                float const        p_far);

    static Mat4x4   Orthographique(float const        p_fov,
                                   float const        p_aspect,
                                   float const        p_near,
                                   float const        p_far);

    static Mat4x4   Orthographique(float const        p_fov,
                                   float const        p_width,
                                   float const        p_height,
                                   float const        p_near,
                                   float const        p_far);

    static Mat4x4   ViewDeg(float pitch, float yaw, Vec3 pos);

    static Mat4x4   ViewRad(float pitch, float yaw, Vec3 pos);

    static Mat4x4   Frustum(float l, float r, float t, float b, float n, float f);
    
    static Mat4x4   Ortho(float const        p_left,
                          float const        p_right,
                          float const        p_bottom,
                          float const        p_top,
                          float const        p_near,
                          float const        p_far);
    
    static Mat4x4   Ortho(float const        p_width,
                          float const        p_height,
                          float const        p_near,
                          float const        p_far);

    static Mat4x4   OrthoScreenToGL(float const        p_width,
                                    float const        p_height);

    void            PrintMat();

    static Mat4x4   RotateDeg(Vec3 const& p_rotate);
    static Mat4x4   RotateDeg(float const& p_x, float const& p_y, float const& p_z);

    static Mat4x4   RotateRad(Vec3 const& p_rotate);
    static Mat4x4   RotateRad(float const& p_x, float const& p_y, float const& p_z);

    static Mat4x4   RotationXDeg(float const        p_angleDeg);

    static Mat4x4   RotationXRad(float const        p_angleRad);

    static Mat4x4   RotationYDeg(float const        p_angleDeg);

    static Mat4x4   RotationYRad(float const        p_angleRad);

    static Mat4x4   RotationZDeg(float const        p_angleDeg);

    static Mat4x4   RotationZRad(float const        p_angleRad);

    static Mat4x4   Scale(Vec3 const& p_scale);


    static Mat4x4   Translate(Vec3 const& p_translation);
    static Mat4x4   Translate(float const& p_x, float const& p_y, float const& p_z);

    static Mat4x4   TRSDeg(Vec3 const& p_translation,
                           Vec3 const& p_rotation,
                           Vec3 const& p_scale);

    static Mat4x4   TRSRad(Vec3 const& p_translation,
                           Vec3 const& p_rotation,
                           Vec3 const& p_scale);
    Vec3         GetAxis(unsigned int const    p_index) const;

    Vec3         GetBackward() const;

    Mat4x4       GetCofactor() const;

    Vec4         GetColumn(unsigned int const    p_index) const;

    float        GetDeterminant() const;

    Vec3         GetDown() const;

    Vec3         GetForward() const;

    Mat4x4       GetInverse()                                              const;

    Vec3         GetLeft()                                              const;

    Mat4x4       GetMatrixWithoutScale(float const    p_tolerance = MATH_EPSILON_F)   const;

    float        GetMaximumAxisScale()                                              const;

    Vec3         GetOrigin()                                              const;

    Vec3         GetRight()                                              const;

    Vec3         GetRotationDeg()                                              const;
    Vec3         GetRotationRad()                                              const;

    float        GetRotationDeterminant()                                              const;

    Vec4         GetRow(unsigned int const    p_index)                        const;

    Vec3         GetScale(float const    p_tolerance = MATH_EPSILON_F)   const;

    void         GetScaledAxes(Vec3 & p_outXAxis,
                 Vec3 & p_outYAxis,
                 Vec3 & p_outZAxis)                     const;

    Vec3         GetScaledBackward()                                              const;

    Vec3         GetScaledDown()                                              const;

    Vec3         GetScaledForward()                                              const;

    Vec3         GetScaledLeft()                                              const;

    Vec3         GetScaledRight()                                              const;

    Vec3         GetScaledUp()                                              const;


    Mat4x4       GetTransposed()                                              const;

    Vec3         GetUp()                                              const;

    std::array<float, 16>          GetValues() const;

    Vec3         MultiplyPoint(Vec3 const& p_point)                        const;

    Vec3         MultiplyPoint3x4(Vec3 const& p_point)                        const;

    Vec3         MultiplyVector(Vec3 const& p_point)                        const;

    void         RemoveScaling(float const    p_tolerance = MATH_EPSILON_F);

    void         SetAxis(unsigned int const    p_axisIndex,
                 Vec3 const& p_axis);

    void         SetAxis(unsigned int const      p_axisIndex,
                 float const    p_x,
                 float const    p_y,
                 float const    p_z);

    void         SetAxisX(Vec3 const& p_axis);

    void         SetAxisX(float const    p_x,
                 float const    p_y,
                 float const    p_z);

    void         SetAxisY(Vec3 const& p_axis);

    void         SetAxisY(float const    p_x,
                 float const    p_y,
                 float const    p_z);

    void         SetAxisZ(Vec3 const& p_axis);

    void         SetAxisZ(float const    p_x,
                 float const    p_y,
                 float const    p_z);

    void         SetColumn(unsigned int const    p_index,
                 Vec4 const& p_column);

    void         SetRow(unsigned int const    p_index,
                 Vec4 const& p_row);

    void         SetOrigin(Vec3 const& p_newOrigin);

    void         SetOrigin(float const    p_newX,
                 float const    p_newY,
                 float const    p_newZ);

    // Operators:
    inline bool         operator==  (Mat4x4 const&   p_mat4x4)    const
    {
        return m_mat[0] == p_mat4x4.m_mat[0] &&
               m_mat[1] == p_mat4x4.m_mat[1] &&
               m_mat[2] == p_mat4x4.m_mat[2] &&
               m_mat[3] == p_mat4x4.m_mat[3] &&
               m_mat[4] == p_mat4x4.m_mat[4] &&
               m_mat[5] == p_mat4x4.m_mat[5] &&
               m_mat[6] == p_mat4x4.m_mat[6] &&
               m_mat[7] == p_mat4x4.m_mat[7] &&
               m_mat[8] == p_mat4x4.m_mat[8] &&
               m_mat[9] == p_mat4x4.m_mat[9] &&
               m_mat[10] == p_mat4x4.m_mat[10] &&
               m_mat[11] == p_mat4x4.m_mat[11] &&
               m_mat[12] == p_mat4x4.m_mat[12] &&
               m_mat[13] == p_mat4x4.m_mat[13] &&
               m_mat[14] == p_mat4x4.m_mat[14] &&
               m_mat[15] == p_mat4x4.m_mat[15];
    }
    inline bool         operator!=  (Mat4x4 const&   p_mat4x4)    const
    {
        return m_mat[0] != p_mat4x4.m_mat[0] ||
               m_mat[1] != p_mat4x4.m_mat[1] ||
               m_mat[2] != p_mat4x4.m_mat[2] ||
               m_mat[3] != p_mat4x4.m_mat[3] ||
               m_mat[4] != p_mat4x4.m_mat[4] ||
               m_mat[5] != p_mat4x4.m_mat[5] ||
               m_mat[6] != p_mat4x4.m_mat[6] ||
               m_mat[7] != p_mat4x4.m_mat[7] ||
               m_mat[8] != p_mat4x4.m_mat[8] ||
               m_mat[9] != p_mat4x4.m_mat[9] ||
               m_mat[10] != p_mat4x4.m_mat[10] ||
               m_mat[11] != p_mat4x4.m_mat[11] ||
               m_mat[12] != p_mat4x4.m_mat[12] ||
               m_mat[13] != p_mat4x4.m_mat[13] ||
               m_mat[14] != p_mat4x4.m_mat[14] ||
               m_mat[15] != p_mat4x4.m_mat[15];
    }

    inline Mat4x4&      operator=   (Mat4x4 const&   p_copy)
    {
        m_mat = p_copy.m_mat;
        return *this;
    }

    inline float        operator()  (unsigned int const        p_rowIndex,
                                     unsigned int const        p_columnIndex)  const
    {
        if (p_rowIndex < 0 || p_rowIndex > 3 || p_columnIndex < 0 || p_columnIndex > 3)
        {
            throw std::out_of_range("One or both of your indexes is out of range, should be between 0 and 3");
        }
        return m_mat[p_rowIndex + 4 * p_columnIndex];
    }
    inline float&       operator()  (unsigned int const        p_rowIndex,
                                     unsigned int const        p_columnIndex)
    {
        if (p_rowIndex < 0 || p_rowIndex > 3 || p_columnIndex < 0 || p_columnIndex > 3)
        {
            throw std::out_of_range("One or both of your indexes is out of range, should be between 0 and 3");
        }
        return m_mat[p_rowIndex + 4 * p_columnIndex];
    }

    inline float        operator()  (unsigned int const        p_index)        const
    {
        if (p_index < 0 || p_index > 15)
        {
            throw std::out_of_range("Index is out of range, should be between 0 and 15");
        }
        return m_mat[p_index];
    }
    inline float&       operator()  (unsigned int const        p_index)
    {
        if (p_index < 0 || p_index > 15)
        {
            throw std::out_of_range("Index is out of range, should be between 0 and 15");
        }
        return m_mat[p_index];
    }


    inline Mat4x4       operator+   (Mat4x4 const&   p_mat4x4)    const
    {
        return { m_mat[0] + p_mat4x4.m_mat[0], m_mat[1] + p_mat4x4.m_mat[1],     //1st line 
                 m_mat[2] + p_mat4x4.m_mat[2], m_mat[3] + p_mat4x4.m_mat[3],

                 m_mat[4] + p_mat4x4.m_mat[4], m_mat[5] + p_mat4x4.m_mat[5],     //2nd line
                 m_mat[6] + p_mat4x4.m_mat[6], m_mat[7] + p_mat4x4.m_mat[7],

                 m_mat[8] + p_mat4x4.m_mat[8], m_mat[9] + p_mat4x4.m_mat[9],     //3rd line
                 m_mat[10] + p_mat4x4.m_mat[10], m_mat[11] + p_mat4x4.m_mat[11],

                 m_mat[12] + p_mat4x4.m_mat[12], m_mat[13] + p_mat4x4.m_mat[13], //4th line
                 m_mat[14] + p_mat4x4.m_mat[14], m_mat[15] + p_mat4x4.m_mat[15] };
    }
    inline Mat4x4&      operator+=  (Mat4x4 const&   p_mat4x4)
    {
        m_mat[0] += p_mat4x4.m_mat[0];
        m_mat[1] += p_mat4x4.m_mat[1];
        m_mat[2] += p_mat4x4.m_mat[2];
        m_mat[3] += p_mat4x4.m_mat[3];
        m_mat[4] += p_mat4x4.m_mat[4];
        m_mat[5] += p_mat4x4.m_mat[5];
        m_mat[6] += p_mat4x4.m_mat[6];
        m_mat[7] += p_mat4x4.m_mat[7];
        m_mat[8] += p_mat4x4.m_mat[8];
        m_mat[9] += p_mat4x4.m_mat[9];
        m_mat[10] += p_mat4x4.m_mat[10];
        m_mat[11] += p_mat4x4.m_mat[11];
        m_mat[12] += p_mat4x4.m_mat[12];
        m_mat[13] += p_mat4x4.m_mat[13];
        m_mat[14] += p_mat4x4.m_mat[14];
        m_mat[15] += p_mat4x4.m_mat[15];

        return *this;
    }

    inline Mat4x4       operator-   (Mat4x4 const& p_mat4x4)    const
    {
        return { m_mat[0] - p_mat4x4.m_mat[0], m_mat[1] - p_mat4x4.m_mat[1],     //1st line
                 m_mat[2] - p_mat4x4.m_mat[2], m_mat[3] - p_mat4x4.m_mat[3],

                 m_mat[4] - p_mat4x4.m_mat[4], m_mat[5] - p_mat4x4.m_mat[5],     //2nd line
                 m_mat[6] - p_mat4x4.m_mat[6], m_mat[7] - p_mat4x4.m_mat[7],

                 m_mat[8] - p_mat4x4.m_mat[8], m_mat[9] - p_mat4x4.m_mat[9],     //3rd line
                 m_mat[10] - p_mat4x4.m_mat[10], m_mat[11] - p_mat4x4.m_mat[11],

                 m_mat[12] - p_mat4x4.m_mat[12] ,m_mat[13] - p_mat4x4.m_mat[13], //4th line
                 m_mat[14] - p_mat4x4.m_mat[14], m_mat[15] - p_mat4x4.m_mat[15] };
    }
    inline Mat4x4&      operator-=  (Mat4x4 const& p_mat4x4)
    {
        m_mat[0] -= p_mat4x4.m_mat[0];
        m_mat[1] -= p_mat4x4.m_mat[1];
        m_mat[2] -= p_mat4x4.m_mat[2];
        m_mat[3] -= p_mat4x4.m_mat[3];
        m_mat[4] -= p_mat4x4.m_mat[4];
        m_mat[5] -= p_mat4x4.m_mat[5];
        m_mat[6] -= p_mat4x4.m_mat[6];
        m_mat[7] -= p_mat4x4.m_mat[7];
        m_mat[8] -= p_mat4x4.m_mat[8];
        m_mat[9] -= p_mat4x4.m_mat[9];
        m_mat[10] -= p_mat4x4.m_mat[10];
        m_mat[11] -= p_mat4x4.m_mat[11];
        m_mat[12] -= p_mat4x4.m_mat[12];
        m_mat[13] -= p_mat4x4.m_mat[13];
        m_mat[14] -= p_mat4x4.m_mat[14];
        m_mat[15] -= p_mat4x4.m_mat[15];

        return *this;
    }
    
    inline Mat4x4       operator/   (float const p_scale)    const
    {
        return { m_mat[0] / p_scale , m_mat[1] / p_scale , m_mat[2] / p_scale , m_mat[3] / p_scale,
                 m_mat[4] / p_scale , m_mat[5] / p_scale , m_mat[6] / p_scale , m_mat[7] / p_scale,
                 m_mat[8] / p_scale , m_mat[9] / p_scale , m_mat[10]/ p_scale , m_mat[11]/ p_scale,
                 m_mat[12]/ p_scale , m_mat[13]/ p_scale , m_mat[14]/ p_scale , m_mat[15]/ p_scale };
    }

    inline Mat4x4       operator*   (Mat4x4 const&   p_mat4x4)    const
    {
        Mat4x4 res = Mat4x4::Zero;

        for (int co = 0; co < 4; co++)
            for (int r = 0; r < 4; r++)
                for (int i = 0; i < 4; i++)
                    res.m_mat[co + r * 4] += p_mat4x4.m_mat[i + r * 4] * m_mat[co + i * 4];
        return res;
    }
    inline Mat4x4       operator*   (float const        p_scale)        const
    {
        return { m_mat[0] * p_scale , m_mat[1] * p_scale , m_mat[2] * p_scale , m_mat[3] * p_scale,
                 m_mat[4] * p_scale , m_mat[5] * p_scale , m_mat[6] * p_scale , m_mat[7] * p_scale,
                 m_mat[8] * p_scale , m_mat[9] * p_scale , m_mat[10]* p_scale , m_mat[11]* p_scale,
                 m_mat[12]* p_scale , m_mat[13]* p_scale , m_mat[14]* p_scale , m_mat[15]* p_scale };
    }
    inline Vec3         operator*   (Vec3 const&     p_position)     const
    {
        return MultiplyPoint(p_position);
    }
    inline Vec4         operator*   (Vec4 const&     p_vector4)      const
    {
        return
        {
            m_mat[0] * p_vector4.m_x + m_mat[4] * p_vector4.m_y +   //1st line
            m_mat[8] * p_vector4.m_z + m_mat[12] * p_vector4.m_w,

            m_mat[1] * p_vector4.m_x + m_mat[5] * p_vector4.m_y +   //2nd line
            m_mat[9] * p_vector4.m_z + m_mat[13] * p_vector4.m_w,

            m_mat[2] * p_vector4.m_x + m_mat[6] * p_vector4.m_y +   //3rd line
            m_mat[10] * p_vector4.m_z + m_mat[14] * p_vector4.m_w,

            m_mat[3] * p_vector4.m_x + m_mat[7] * p_vector4.m_y +   //4th line
            m_mat[11] * p_vector4.m_z + m_mat[15] * p_vector4.m_w
        };
    }

    inline Mat4x4&      operator*=  (Mat4x4 const&   p_mat4x4)
    {
        *this = *this * p_mat4x4;
        return *this;
    }
    inline Mat4x4&      operator*=  (float const        p_scale)
    {
        m_mat[0] *= p_scale;
        m_mat[1] *= p_scale;
        m_mat[2] *= p_scale;
        m_mat[3] *= p_scale;
        m_mat[4] *= p_scale;
        m_mat[5] *= p_scale;
        m_mat[6] *= p_scale;
        m_mat[7] *= p_scale;
        m_mat[8] *= p_scale;
        m_mat[9] *= p_scale;
        m_mat[10] *= p_scale;
        m_mat[11] *= p_scale;
        m_mat[12] *= p_scale;
        m_mat[13] *= p_scale;
        m_mat[14] *= p_scale;
        m_mat[15] *= p_scale;

        return *this;
    }

    inline Vec4       operator[](int col) const
    {
        return GetColumn(col);
    }
    inline Vec4       operator[](int col)
    {
        return GetColumn(col);
    }
};
#endif