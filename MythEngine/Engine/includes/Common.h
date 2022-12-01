#pragma once
#include "maths/Vec3.hpp"
#define MAX_NUM_BONES_PER_VERTEX 5

struct Vertex
{
	Vec3 position;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	int boneIDs[MAX_NUM_BONES_PER_VERTEX] = {0};
	float weights[MAX_NUM_BONES_PER_VERTEX] = {0.0f};
};