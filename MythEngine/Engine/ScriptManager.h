#pragma once

#include "Core/GameObject.h"
#include "Resources/Scene.h"

#include "resources/Scripts/Includes/PlayerController.h"
#include "resources/Scripts/Includes/Enemies.h"
#include "resources/Scripts/Includes/PlayerAttack.h"
#include "resources/Scripts/Includes/CameraBoom.h"

namespace ScriptManager
{
	inline void LoadScript()
	{
#ifdef MODE_EDITOR
		Core::GameObject::AddComponentType("PlayerController", PlayerController::Create);
		Core::GameObject::AddComponentType("Enemies", Enemies::Create);
		Core::GameObject::AddComponentType("PlayerAttack", PlayerAttack::Create);
		Core::GameObject::AddComponentType("CameraBoom", CameraBoom::Create);
#endif // MODE_EDITOR

		Resources::BasicScene::AddComponentType(PlayerController::TypeName, PlayerController::Create);
		Resources::BasicScene::AddComponentType(Enemies::TypeName, Enemies::Create);
		Resources::BasicScene::AddComponentType(PlayerAttack::TypeName, PlayerAttack::Create);
		Resources::BasicScene::AddComponentType(CameraBoom::TypeName, CameraBoom::Create);
	}
}