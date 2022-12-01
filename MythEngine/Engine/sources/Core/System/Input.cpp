#include "Core/System/Input.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_stdlib.h"

SINGLETON_SETUP(Core::System::Input)

Core::System::Input::Input() :
	m_xMousePosition(0.0),
	m_yMousePosition(0.0),
	m_xOldMousePosition(0.0),
	m_yOldMousePosition(0.0),
	m_lockMouse(false)
{
	MYTH_ASSERT(singleton == nullptr);
	singleton = this;
}

void Core::System::Input::Update(GLFWwindow* p_window)
{
	if (p_window == nullptr)
		return;

	m_xOldMousePosition = m_xMousePosition;
	m_yOldMousePosition = m_yMousePosition;

	glfwGetCursorPos(p_window, &m_xMousePosition, &m_yMousePosition);

	if (m_lockMouse)
		glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	int KeyStatus = 0;

	for (unsigned int i = 0; i < m_keyboardMouseInput.size(); i++)
	{
		if (m_keyboardMouseInput[i].KeyCodes.empty())
		{
			m_keyboardMouseInput.erase(m_keyboardMouseInput.begin() + i);
			i--;
			continue;
		}

		KeyboardMouseInputData& currentInputdata = m_keyboardMouseInput[i];

		currentInputdata.PreviousDown = currentInputdata.IsDown;
		currentInputdata.IsRepeated = false;
		currentInputdata.IsDown = false;

		for (const std::pair<bool, int>& currentKeyCode : currentInputdata.KeyCodes)
		{
			if (currentKeyCode.first)
				KeyStatus = glfwGetMouseButton(p_window, currentKeyCode.second);
			else
				KeyStatus = glfwGetKey(p_window, currentKeyCode.second);

			currentInputdata.IsRepeated = currentInputdata.IsRepeated || (KeyStatus == GLFW_REPEAT);
			currentInputdata.IsDown = currentInputdata.IsDown || ((KeyStatus == GLFW_PRESS) || (KeyStatus == GLFW_REPEAT));
		}
	}
}

bool Core::System::Input::HasKeyInput(const std::string& p_inputName)
{
	for (unsigned int i = 0; i < m_keyboardMouseInput.size(); i++)
		if (m_keyboardMouseInput[i].Name == p_inputName)
			return true;

	return false;
}

Core::System::KeyboardMouseInputData* Core::System::Input::GetKeyInput(const unsigned int p_inputIndex)
{
	if (p_inputIndex >= m_keyboardMouseInput.size())
		return nullptr;
	
	return &(m_keyboardMouseInput[p_inputIndex]);
}


Core::System::KeyboardMouseInputData* Core::System::Input::GetKeyInput(const std::string& p_inputName)
{
	for (unsigned int i = 0; i < m_keyboardMouseInput.size(); i++)
		if (m_keyboardMouseInput[i].Name == p_inputName)
			return &(m_keyboardMouseInput[i]);

	return nullptr;
}

void Core::System::Input::AddKeyInput(const std::string& p_inputName, int p_keycode, bool p_isMouse)
{
	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputName);

	if (inputData == nullptr)
	{
		m_keyboardMouseInput.emplace_back();
		inputData = &m_keyboardMouseInput.back();
		inputData->Name = p_inputName;
	}

	inputData->KeyCodes.push_back({ p_isMouse , p_keycode });
}

void Core::System::Input::RemKeyInput(const std::string& p_inputName, int p_keycode, bool p_isMouse)
{
	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputName);

	if (inputData == nullptr)
		return;

	for (unsigned int i = 0; i < inputData->KeyCodes.size(); i++)
	{
		if (inputData->KeyCodes[i].first == p_isMouse)
			if (inputData->KeyCodes[i].second == p_keycode)
			{
				inputData->KeyCodes.erase(inputData->KeyCodes.begin() + i);
				i--;
			}
	}
}

unsigned int Core::System::Input::GetKeyInputIndex(const std::string& p_inputName)
{
	MYTH_ASSERT(singleton != nullptr);

	for (unsigned int i = 0; i < singleton->m_keyboardMouseInput.size(); i++)
		if (singleton->m_keyboardMouseInput[i].Name == p_inputName)
			return i;

	return EOF;
}

Core::System::InputStatus Core::System::Input::GetKeyInputStatus(const unsigned int p_inputIndex)
{
	MYTH_ASSERT(singleton != nullptr);

	InputStatus to_return = { false, false, false, false };
	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputIndex);

	if (inputData == nullptr)
		return to_return;

	to_return.IsDown = inputData->IsDown;
	to_return.IsPressed = (inputData->IsDown && !inputData->PreviousDown);
	to_return.IsReleased = (!inputData->IsDown && inputData->PreviousDown);
	to_return.IsRepeated = inputData->IsRepeated;

	return to_return;
}

bool Core::System::Input::IsKeyDown(const unsigned int p_inputIndex)
{
	MYTH_ASSERT(singleton != nullptr);

	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputIndex);

	if (inputData == nullptr)
		return false;

	return inputData->IsDown;
}

bool Core::System::Input::IsKeyPressed(const unsigned int p_inputIndex)
{
	MYTH_ASSERT(singleton != nullptr);

	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputIndex);

	if (inputData == nullptr)
		return false;

	return (inputData->IsDown && !inputData->PreviousDown);
}

bool Core::System::Input::IsKeyReleased(const unsigned int p_inputIndex)
{
	MYTH_ASSERT(singleton != nullptr);

	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputIndex);

	if (inputData == nullptr)
		return false;

	return (!inputData->IsDown && inputData->PreviousDown);
}

bool Core::System::Input::IsKeyRepeated(const unsigned int p_inputIndex)
{
	MYTH_ASSERT(singleton != nullptr);

	KeyboardMouseInputData* inputData = singleton->GetKeyInput(p_inputIndex);

	if (inputData == nullptr)
		return false;

	return inputData->IsRepeated;
}

Vec2 Core::System::Input::GetMouseDelta()
{
	MYTH_ASSERT(singleton != nullptr);

	return Vec2(
		(float)(singleton->m_xMousePosition - singleton->m_xOldMousePosition),
		(float)(singleton->m_yMousePosition - singleton->m_yOldMousePosition)
	);
}

#ifdef MODE_EDITOR
void Core::System::keyboard_button_callback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	// KEY CALLBACK TO BIND INPUTS (Replace the callback set by ImGui)

	lastKeyPressedIsMouse = false;

	if (p_action == GLFW_PRESS)
		lastKeyPressed = p_key;
	else
		lastKeyPressed = GLFW_KEY_UNKNOWN;

	ImGui_ImplGlfw_KeyCallback(p_window, p_key, p_scancode, p_action, p_mods);
}

void Core::System::mouse_button_callback(GLFWwindow* p_window, int p_key, int p_action, int p_mods)
{
	// MOUSE KEY CALLBACK TO BIND INPUTS (Replace the callback set by ImGui)

	lastKeyPressedIsMouse = true;

	if (p_action == GLFW_PRESS)
		lastKeyPressed = p_key;
	else
		lastKeyPressed = GLFW_KEY_UNKNOWN;

	ImGui_ImplGlfw_MouseButtonCallback(p_window, p_key, p_action, p_mods);
}

void Core::System::SetKeyCallback(GLFWwindow* p_window)
{
	glfwSetKeyCallback(p_window, keyboard_button_callback);
	glfwSetMouseButtonCallback(p_window, mouse_button_callback);
}

std::string getKeyName(int p_key, const bool p_isMouse)
{
	// Some inputs have no name with glfwGetKeyName()

	if (p_isMouse)
	{
		if (p_key == GLFW_MOUSE_BUTTON_RIGHT)
			return "MOUSE RIGHT";
		if (p_key == GLFW_MOUSE_BUTTON_LEFT)
			return "MOUSE LEFT";
		if (p_key == GLFW_MOUSE_BUTTON_MIDDLE)
			return "MOUSE MIDDLE";
		if (p_key == GLFW_MOUSE_BUTTON_LAST)
			return "MOUSE LAST";

		return std::to_string(p_key);
	}

	if (p_key == GLFW_KEY_UNKNOWN)
		return "UNSET";

	else if (p_key == 256)
		return "ESCAPE";

	else if (p_key == 32)
		return "SPACE";

	else if (p_key == 262)
		return "RIGHT";

	else if (p_key == 263)
		return "LEFT";

	else if (p_key == 264)
		return "DOWN";

	else if (p_key == 265)
		return "UP";

	else if (p_key >= 290 && p_key < 301)
		return "F" + std::to_string(p_key - 289);

	else if (p_key == 341)
		return "Left-CTRL";

	else if (p_key == 340)
		return "Left-SHIFT";

	else
		return glfwGetKeyName(p_key, glfwGetKeyScancode(p_key));
}

void Core::System::Input::ShowEditorWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("Input Editor", p_opened);

	std::vector<std::pair<std::string, std::pair<bool, int>>> toRemoveKey;

	for (unsigned int indexInput = 0; indexInput < m_keyboardMouseInput.size(); indexInput++)
		if (ImGui::TreeNode((m_keyboardMouseInput[indexInput].Name + "##indexInput" + std::to_string(indexInput)).c_str()))
		{
			KeyboardMouseInputData& currentData = m_keyboardMouseInput[indexInput];

			ImGui::Text(("CURRENT STATE : " + std::string(currentData.IsDown ? "DOWN" : "UP")).c_str());
			ImGui::Separator();

			for (unsigned int indexkey = 0; indexkey < currentData.KeyCodes.size(); indexkey++)
			{
				std::pair<bool, int>& currentKey = currentData.KeyCodes[indexkey];

				ImGui::Text(getKeyName(currentKey.second, currentKey.first).c_str());
				ImGui::SameLine();
				if (ImGui::Button(("Set##indexkey" + std::to_string(indexkey)).c_str()))
					SetInputKeycode(currentKey);
				ImGui::SameLine();
				if (ImGui::Button(("Delete##indexkey" + std::to_string(indexkey)).c_str()))
					toRemoveKey.push_back({ currentData.Name , {currentKey.first , currentKey.second} });
			}

			if (ImGui::Button(("Add Key##indexInput" + std::to_string(indexInput)).c_str()))
				AddKeyInput(currentData.Name, GLFW_KEY_UNKNOWN);

			ImGui::TreePop();
			ImGui::Separator();
		}

	static std::string newInputName = "";
	ImGui::InputText("##InputName", &newInputName);
	ImGui::SameLine();
	if (ImGui::Button("Add Input##Input"))
		AddKeyInput(newInputName, GLFW_KEY_UNKNOWN);

	for (const std::pair<std::string, std::pair<bool, int>>& currentToRem : toRemoveKey)
		RemKeyInput(currentToRem.first, currentToRem.second.second, currentToRem.second.first);

	ImGui::End();
}

#include "Core/System/XMLMacro.h"
void Core::System::Input::SaveInputs(tinyxml2::XMLElement* InputsXml)
{
	tinyxml2::XMLElement* currentXMLInput = nullptr;
	tinyxml2::XMLElement* currentXMLKeyCode = nullptr;

	for (KeyboardMouseInputData& currentInputtoSave : m_keyboardMouseInput)
	{
		if (currentInputtoSave.Name == EDITOR_INPUT_NAME)
			continue;

		currentXMLInput = InputsXml->InsertNewChildElement("input");
		currentXMLInput->SetAttribute("name", currentInputtoSave.Name.c_str());

		for (std::pair<bool, int>& currentKeyCode : currentInputtoSave.KeyCodes)
		{
			currentXMLKeyCode = currentXMLInput->InsertNewChildElement("key");
			currentXMLKeyCode->SetAttribute("code", currentKeyCode.second);
			if (currentKeyCode.first)
				currentXMLKeyCode->SetAttribute("mouse", true);
		}
	}
}

void Core::System::Input::SetInputKeycode(std::pair<bool, int>& p_pairToUpdate)
{
	lastKeyPressed = GLFW_KEY_UNKNOWN;

	// Wait before listen input to be sure that user put the wanted key
	int wait = 10000;
	while (wait > 0 || lastKeyPressed == GLFW_KEY_UNKNOWN)
	{
		glfwPollEvents();
		wait -= wait < -1 ? 0 : 1;
	}

	// Forbid escape binding
	if (lastKeyPressed == GLFW_KEY_ESCAPE)
	{
		p_pairToUpdate.first = false;
		p_pairToUpdate.second = GLFW_KEY_UNKNOWN;
		return;
	}

	// Forbid timeout
	if (lastKeyPressed == -1) return;

	p_pairToUpdate.first = lastKeyPressedIsMouse;
	p_pairToUpdate.second = lastKeyPressed;
}
#endif // MODE_EDITOR
