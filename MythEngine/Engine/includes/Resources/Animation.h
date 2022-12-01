#pragma once

#include "Resources/Model.h"

namespace Resources
{
	class Animation : public Resource
	{
	public :
		Animation(const std::string& p_name);

		REGISTER_RESOURCE_HPP()

		std::unordered_map<std::string, aiNodeAnim*> m_nodeAnimMap;
		float m_duration;
		int m_ticksPerSecond;
		int m_index = 0;
		unsigned int m_numberAnim;
		
		const aiScene* GetAssimpScene() const { return m_scene; }

		void LoadResource() override;
		void UnloadResource() override;
#ifdef MODE_EDITOR
			// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
		void ShowEditorControl() override;
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
#endif // MODE_EDITOR

	private :

		const aiScene* m_scene = nullptr;
		Assimp::Importer m_importer;
	};
}