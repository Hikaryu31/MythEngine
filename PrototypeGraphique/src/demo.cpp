#include "demo.h"
#include <imgui.h>
#include <tiny_obj_loader.h>
#include <stb_image.h>

// Shaders
// ==================================================
static const char* gVertexShaderStr = R"GLSL(
#version 450 core
// Attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in mat4 aInstanceModel;
layout(location = 6) in vec3 aColor;

// Uniforms
uniform mat4 uVP;

// Varyings (variables that are passed to fragment shader with perspective interpolation)
out vec2 vUV;
out vec3 vColor;

void main()
{
    vUV = aUV;
    vColor = aColor;

    gl_Position = uVP * aInstanceModel * vec4(aPosition, 1.0);
})GLSL";

static const char* gFragmentShaderStr = R"GLSL(
#version 450 core
// Varyings
in vec2 vUV;
in vec3 vColor;

// Uniforms
uniform sampler2D uColorTexture;

// Shader outputs
out vec4 oColor;

void main()
{
    oColor = vec4(vColor, 1.0) * texture(uColorTexture, vUV);
})GLSL";

Demo::Demo()
{
    // Create render pipeline
    shader = new Shader(gVertexShaderStr, gFragmentShaderStr);

    // Gen mesh
    {
        models.push_back(("ressources/Sponza/sponza.obj"));

        // Upload cube to gpu (VRAM)
        glGenBuffers(1, &this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, models[0].allVertices.size() * sizeof(Vertex), &models[0].allVertices[0], GL_STATIC_DRAW);
    }

    // Create a vertex array
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glGenBuffers(1, &InstanceTransformVBO);
    glGenBuffers(1, &InstanceColorVBO);
    SetInstanceAttributes();
}

Demo::~Demo()
{
    // Cleanup GL
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shader->Program);

    glDeleteBuffers(1, &InstanceTransformVBO);
    glDeleteBuffers(1, &InstanceColorVBO);
    InstanceModel.clear();
    InstanceTransform.clear();
    InstanceColor.clear();
}

void Demo::DrawInstanced(const GLuint& Program, const mat4& ViewProj, const GLsizei& instanceCount) const
{
    glUniformMatrix4fv(glGetUniformLocation(shader->Program, "uVP"), 1, GL_FALSE, ViewProj.e);
    glDrawArraysInstanced(GL_TRIANGLES, 0, models[0].allVertices.size(), instanceCount);
}

void Demo::Update()
{
    //Camera = CameraUpdateFreefly(Camera, IO.CameraInputs);

    // Compute model-view-proj and send it to shader
    mat4 ProjectionMatrix = Mat4::Perspective(Math::ToRadians(60.f), 1200.f / 800.f, 0.1f, 100.f);
    mat4 ViewMatrix = Mat4::RotateZ(camera.roll) * Mat4::RotateX(-camera.pitch) * Mat4::RotateY(-camera.yaw) * Mat4::Translate(-camera.pos);

    // Setup GL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Clear screen
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader and send data
    shader->use();

    glBindVertexArray(VAO);
    DrawInstanced(shader->Program, ProjectionMatrix * ViewMatrix, InstanceCount);
}

void Demo::SetInstanceAttributes()
{
    InstanceModel.clear();
    InstanceTransform.clear();
    InstanceColor.clear();

    // Create vertex instance offset
    for (int i = 0; i < InstanceCount; ++i)
    {
        Transform tf;
        tf.t = { Rng(-10.f, 10.f), Rng(-5.f, 5.f), Rng(-20.f, -10.f) };
        tf.r = { 0.f, 0.f, 0.f };
        float s = Rng(0.1f, 1.5f);
        tf.s = { s, s, s };

        InstanceTransform.push_back(tf);
        InstanceModel.push_back(tf.GetModelMatrix());
        InstanceColor.push_back({ Rng(0.f, 1.f), Rng(0.f, 1.f), Rng(0.f, 1.f) });
    }

    // transform buffer
    glBindBuffer(GL_ARRAY_BUFFER, InstanceTransformVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * InstanceCount, InstanceModel.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)sizeof(v4));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(5, 1);

    // color buffer
    glBindBuffer(GL_ARRAY_BUFFER, InstanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * InstanceCount, InstanceColor.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void*)0);
    glVertexAttribDivisor(6, 1);
}

void Demo::UpdateInstanceAttributes()
{
    InstanceModel[InstanceIndex] = InstanceTransform[InstanceIndex].GetModelMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, InstanceTransformVBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(mat4) * InstanceIndex, sizeof(mat4), InstanceModel[InstanceIndex].e);

    glBindBuffer(GL_ARRAY_BUFFER, InstanceColorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(v3) * InstanceIndex, sizeof(v3), InstanceColor[InstanceIndex].e);
}

void Demo::AddInstanceAttributes()
{
    InstanceTransform.push_back(InstanceAdditionalTransform);
    InstanceModel.push_back(InstanceAdditionalTransform.GetModelMatrix());
    InstanceColor.push_back(InstanceAdditionalColor);

    ++InstanceCount;

    // transform buffer
    glBindBuffer(GL_ARRAY_BUFFER, InstanceTransformVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * InstanceCount, InstanceModel.data(), GL_STATIC_DRAW);

    // color buffer
    glBindBuffer(GL_ARRAY_BUFFER, InstanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * InstanceCount, InstanceColor.data(), GL_STATIC_DRAW);
}

void Demo::DestroyInstanceAttributes()
{
    InstanceTransform.erase(InstanceTransform.begin() + InstanceIndex);
    InstanceModel.erase(InstanceModel.begin() + InstanceIndex);
    InstanceColor.erase(InstanceColor.begin() + InstanceIndex);

    --InstanceCount;
    InstanceIndex = 0;

    // transform buffer
    glBindBuffer(GL_ARRAY_BUFFER, InstanceTransformVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * InstanceCount, InstanceModel.data(), GL_STATIC_DRAW);

    // color buffer
    glBindBuffer(GL_ARRAY_BUFFER, InstanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * InstanceCount, InstanceColor.data(), GL_STATIC_DRAW);
}

void Demo::Display()
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.4f, 0.4f, 0.4f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Update();
    DisplayDebugUI();
}

void Demo::DisplayDebugUI()
{
    if (ImGui::TreeNodeEx("demo", ImGuiTreeNodeFlags_Framed))
    {
        // Debug display
        if (ImGui::TreeNodeEx("Camera"))
        {
            ImGui::DragFloat3("Position", camera.pos.e);
            ImGui::DragFloat("Pitch", &camera.pitch, 0.1f);
            ImGui::DragFloat("Yaw", &camera.yaw, 0.1f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Instancing"))
        {
            if (ImGui::DragInt("Instances", (int*)&InstanceCount, 1.f, 0, 9999))
            {
                InstanceIndex = 0;
                SetInstanceAttributes();
            }

            if (InstanceCount)
            {
                ImGui::SliderInt("Instance index", (int*)&InstanceIndex, 0, InstanceCount - 1);

                if (ImGui::DragFloat3("Instance position", InstanceTransform[InstanceIndex].t.e))
                    UpdateInstanceAttributes();
                if (ImGui::DragFloat3("Instance rotation", InstanceTransform[InstanceIndex].r.e))
                    UpdateInstanceAttributes();
                if (ImGui::DragFloat3("Instance scale", InstanceTransform[InstanceIndex].s.e))
                    UpdateInstanceAttributes();

                if (ImGui::DragFloat3("Instance color", InstanceColor[InstanceIndex].e, 1.f, 0.f, 1.f))
                    UpdateInstanceAttributes();

                if (ImGui::Button("Destroy"))
                    DestroyInstanceAttributes();
            }

            if (ImGui::TreeNodeEx("Add instance"))
            {
                ImGui::DragFloat3("Instance position", InstanceAdditionalTransform.t.e);
                ImGui::DragFloat3("Instance rotation", InstanceAdditionalTransform.r.e);
                ImGui::DragFloat3("Instance scale", InstanceAdditionalTransform.s.e);
                ImGui::DragFloat3("Instance color", InstanceAdditionalColor.e, 1.f, 0.f, 1.f);

                if (ImGui::Button("Add"))
                    AddInstanceAttributes();

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}