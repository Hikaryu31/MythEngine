#pragma once

#include "define.h"

#include "Component.h"

#include <miniaudio.h>

#include <vector>
#include <memory>
#include <string>

namespace ECS
{
	class Audio : public Component, public GameplayComponent
	{
	public:
		Audio() : Component() {}
		~Audio() { UninitializeSound(); }

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY)

		std::string AudioPath = "";

		bool IsSpacializationEnabled() const { return m_spacialization; }
		void SetSpacialization(const bool value = true);

		bool IsLooping() const { return m_looping; }
		void SetLooping(const bool value = true);

		void Play(const ma_uint64 p_at = 4294967295Ui64);
		void Pause();
		void Stop();

		void Start(Core::GameObject* p_gameObject) override;
		void Update(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml) override;

#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override { Update(p_gameObject); }
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR

	private:
		ma_sound* m_sound = nullptr;
		bool m_playAtStart = true;
		bool m_looping = true;
		bool m_spacialization = true;
		float m_volume = 1.f;

		void InitializeSound();
		void UninitializeSound();
	};
}