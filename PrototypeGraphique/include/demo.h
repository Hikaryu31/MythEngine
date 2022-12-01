#pragma once
#include "maths.h"
#include "shader.h"
#include "model.h"


struct Camera
{
    v3 pos = { 0.f, 0.f, 0.f };
    float yaw = 0.f;
    float pitch = 0.f;
    float roll = 0.f;
};

class Demo
{
public:
	Demo();
	~Demo();

    Camera camera;

	void DrawInstanced(const GLuint& p_program, const mat4& p_viewProj, const GLsizei& instanceCount) const;
    void Update();

	void SetInstanceAttributes();
	void UpdateInstanceAttributes();
	void AddInstanceAttributes();
	void DestroyInstanceAttributes();

    void Display();
	void DisplayDebugUI();

private:
    Shader* shader;

    GLuint VAO = 0;
    GLuint VBO = 0;

    GLuint InstanceTransformVBO = 0;
    GLuint InstanceColorVBO = 0;
    unsigned int InstanceCount = 1;
    unsigned int InstanceIndex = 0;

    std::vector<mat4>       InstanceModel;
    std::vector<Transform>  InstanceTransform;
    std::vector<v3>         InstanceColor;
    std::vector<Model>      models;

    Transform               InstanceAdditionalTransform;
    v3                      InstanceAdditionalColor;
};