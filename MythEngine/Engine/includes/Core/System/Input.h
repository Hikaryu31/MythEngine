#pragma once

#include "define.h"

#include "Core/BaseSingleton.h"

#include <string>
#include <vector>

#include "maths/Vec2.hpp"

#include "tinyxml2.h"

struct GLFWwindow;

namespace Core
{
	namespace System
	{
		struct KeyboardMouseInputData
		{
			std::string Name = "";
			std::vector<std::pair<bool, int>> KeyCodes = std::vector<std::pair<bool, int>>(); // { isMouse, keycode }
			bool IsDown = false;
			bool PreviousDown = false;
			bool IsRepeated = false;
		};

		struct InputStatus
		{
			bool IsDown;
			bool IsPressed;
			bool IsReleased;
			bool IsRepeated;
		};

#ifdef MODE_EDITOR
		static int lastKeyPressed = -1;
		static bool lastKeyPressedIsMouse = false;

		void keyboard_button_callback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods);
		void mouse_button_callback(GLFWwindow* p_window, int p_key, int p_action, int p_mods);
		void SetKeyCallback(GLFWwindow* p_window);
#endif // MODE_EDITOR

		class Input : public BaseSingleton<Input>
		{
		public:
			Input();
			~Input() { singleton = nullptr; }

			void Update(GLFWwindow* p_window);
			bool HasKeyInput(const std::string& p_inputName);
			KeyboardMouseInputData* GetKeyInput(const unsigned int p_inputIndex);
			KeyboardMouseInputData* GetKeyInput(const std::string& p_inputName);
			void AddKeyInput(const std::string& p_inputName, int p_keycode, bool p_isMouse = false);
			void RemKeyInput(const std::string& p_inputName, int p_keycode, bool p_isMouse = false);
			void ClearAllInput() { singleton->m_keyboardMouseInput.clear(); }
			inline bool IsMouseLocked() const { return m_lockMouse; }
			inline void SetMouseLock(const bool p_newLock = true) { m_lockMouse = p_newLock; }

			static unsigned int GetKeyInputIndex(const std::string& p_inputName);
			static InputStatus GetKeyInputStatus(const unsigned int p_inputIndex);
			static bool IsKeyDown(const unsigned int p_inputIndex);
			static bool IsKeyPressed(const unsigned int p_inputIndex);
			static bool IsKeyReleased(const unsigned int p_inputIndex);
			static bool IsKeyRepeated(const unsigned int p_inputIndex);

			static inline Vec2 GetMousePosition() { MYTH_ASSERT(singleton != nullptr); return Vec2((float)singleton->m_xMousePosition, (float)singleton->m_yMousePosition); }
			static Vec2 GetMouseDelta();

#ifdef MODE_EDITOR
			void ShowEditorWindow(bool* p_opened = nullptr);
			void SaveInputs(tinyxml2::XMLElement* InputsXml);
#endif // MODE_EDITOR

		private:
#ifdef MODE_EDITOR
			void SetInputKeycode(std::pair<bool, int>& pairToUpdate);
#endif // MODE_EDITOR

			std::vector<KeyboardMouseInputData> m_keyboardMouseInput;

			double m_xMousePosition;
			double m_yMousePosition;
			double m_xOldMousePosition;
			double m_yOldMousePosition;

			bool m_lockMouse;
		};
	}
}