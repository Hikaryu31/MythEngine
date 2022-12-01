#include "ECS/Audio.h"

#include "Core/System/Application.h"
#include "ECS/Transform.h"

REGISTER_COMPONENT_CPP(ECS::Audio)

void ECS::Audio::InitializeSound()
{
	if (m_sound != nullptr || AudioPath.empty())
		return;

	m_sound = new ma_sound;
	ma_engine& engine = Core::System::Application::GetInstance().GetAudioEngine();

	ma_result result = ma_sound_init_from_file(&engine, AudioPath.c_str(), NULL, NULL, NULL, m_sound);
	if (result != MA_SUCCESS)
	{
		MYTH_WARNING("impossible d'initializer depuis '%s'", AudioPath.c_str());

		delete m_sound;
		m_sound = nullptr;
		return;
	}

	ma_sound_set_spatialization_enabled(m_sound, m_spacialization);
	ma_sound_set_looping(m_sound, m_looping);
	ma_sound_set_volume(m_sound, m_volume);

	if (m_playAtStart)
		ma_sound_start(m_sound);
}

void ECS::Audio::UninitializeSound()
{
	if (m_sound == nullptr)
		return;

	ma_sound_stop(m_sound);
	ma_sound_uninit(m_sound);

	delete m_sound;
	m_sound = nullptr;
}

void ECS::Audio::SetSpacialization(const bool value)
{
	m_spacialization = value;

	if (m_sound != nullptr)
		ma_sound_set_spatialization_enabled(m_sound, m_spacialization);
}

void ECS::Audio::SetLooping(const bool value)
{
	m_looping = value;

	if (m_sound != nullptr)
		ma_sound_set_looping(m_sound, m_looping);
}

void ECS::Audio::Play(const ma_uint64 p_at)
{
	if (m_sound != nullptr)
	{
		if (p_at != 4294967295Ui64)
			ma_sound_seek_to_pcm_frame(m_sound, p_at);

		ma_sound_start(m_sound);
	}
}

void ECS::Audio::Pause()
{
	if (m_sound != nullptr)
		ma_sound_stop(m_sound);
}

void ECS::Audio::Stop()
{
	if (m_sound != nullptr)
	{
		ma_sound_stop(m_sound);
		ma_sound_seek_to_pcm_frame(m_sound, 0u);
	}
}

void ECS::Audio::Start(Core::GameObject* p_gameObject)
{
	InitializeSound();

	if (m_spacialization && m_sound != nullptr)
	{
		ECS::Transform* currentTransform = nullptr;
		if (p_gameObject->TryGetComponent(&currentTransform))
		{
			Vec3 currentPosition = currentTransform->GetGlobalPosition();
			ma_sound_set_position(m_sound, currentPosition.m_x, currentPosition.m_y, currentPosition.m_z);
		}
	}
}

void ECS::Audio::Update(Core::GameObject* p_gameObject)
{
	if (m_spacialization && m_sound != nullptr)
	{
		ECS::Transform* currentTransform = nullptr;
		if (p_gameObject->TryGetComponent(&currentTransform))
		{
			Vec3 currentPosition = currentTransform->GetGlobalPosition();
			ma_sound_set_position(m_sound, currentPosition.m_x, currentPosition.m_y, currentPosition.m_z);
		}
	}
}

void ECS::Audio::Load(tinyxml2::XMLElement* ComponentXml)
{
	m_playAtStart = ComponentXml->BoolAttribute("play");
	m_looping = ComponentXml->BoolAttribute("looping");
	m_spacialization = ComponentXml->BoolAttribute("spacialization");
	m_volume = ComponentXml->FloatAttribute("volume", 1.f);
	AudioPath = ComponentXml->Attribute("path");
}

#ifdef MODE_EDITOR
void ECS::Audio::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::Text("Audio :");
	ImGui::SameLine();
	ImGui::InputText(("##Audio_" + std::to_string(p_indexComponent)).c_str(), &AudioPath);
	ImGui::SameLine();
	if (ImGui::Button(("Reload##Audio_" + std::to_string(p_indexComponent)).c_str()))
	{
		UninitializeSound();
		InitializeSound();
	}
	ImGui::SameLine();
	if (ImGui::Button(("X##Audio_" + std::to_string(p_indexComponent)).c_str()))
	{
		UninitializeSound();
		AudioPath = "";
	}

	if (m_sound != nullptr)
	{
		if (ImGui::SliderFloat(("##SliderVolumeAudio_" + std::to_string(p_indexComponent)).c_str(), &m_volume, 0.f, 3.f))
			ma_sound_set_volume(m_sound, m_volume);

		ImGui::SameLine();

		if (ma_sound_is_playing(m_sound))
		{
			if (ImGui::Button(("Pause##Audio_" + std::to_string(p_indexComponent)).c_str()))
				ma_sound_stop(m_sound);

			ImGui::SameLine();
			if (ImGui::Button(("Restart##Audio_" + std::to_string(p_indexComponent)).c_str()))
				ma_sound_seek_to_pcm_frame(m_sound, 0u);
		}
		else
		{
			if (ImGui::Button(("Play ##Audio_" + std::to_string(p_indexComponent)).c_str()))
				ma_sound_start(m_sound);

			ImGui::SameLine();
			if (ImGui::Button(("Restart##Audio_" + std::to_string(p_indexComponent)).c_str()))
			{
				ma_sound_start(m_sound);
				ma_sound_seek_to_pcm_frame(m_sound, 0u);
			}
		}

		if (ImGui::Checkbox(("Spacialization##Audio_" + std::to_string(p_indexComponent)).c_str(), &m_spacialization))
			ma_sound_set_spatialization_enabled(m_sound, m_spacialization);

		if (ImGui::Checkbox(("Looping##Audio_" + std::to_string(p_indexComponent)).c_str(), &m_looping))
			ma_sound_set_looping(m_sound, m_looping);

		ImGui::Checkbox(("Play at start##Audio_" + std::to_string(p_indexComponent)).c_str(), &m_playAtStart);
	}
}

void ECS::Audio::Save(tinyxml2::XMLElement* ComponentXml)
{
	ComponentXml->SetAttribute("play", m_playAtStart);
	ComponentXml->SetAttribute("looping", m_looping);
	ComponentXml->SetAttribute("spacialization", m_spacialization);
	ComponentXml->SetAttribute("volume", m_volume);
	ComponentXml->SetAttribute("path", AudioPath.c_str());
}
#endif // MODE_EDITOR
