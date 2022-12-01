#include "ECS/ModelInstance.h"
#include "ECS/Animator.h"

#include "Core/System/Application.h"
#include <fmt/format.h>

REGISTER_COMPONENT_CPP(ECS::ModelInstance)

ECS::ModelInstance::ModelInstance()
{
}

void ECS::ModelInstance::Start(Core::GameObject* p_gameObject)
{
	DuplicatePart();

	Transform* TRS = nullptr;
	if (p_gameObject->TryGetComponent(&TRS, IndexTransform))
		m_worldMatrix = TRS->GetGlobalMatrix();
}

void ECS::ModelInstance::Update(Core::GameObject* p_gameObject)
{
	Transform* TRS = nullptr;
	if (p_gameObject->TryGetComponent(&TRS, IndexTransform))
	{
		m_worldMatrix = TRS->GetGlobalMatrix();
	}
}

#include "Core/System/XMLMacro.h"
void ECS::ModelInstance::Load(tinyxml2::XMLElement* ComponentXml)
{
	UseLight = ComponentXml->BoolAttribute("light");

	IndexTransform = ComponentXml->UnsignedAttribute("index-transform");

	Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

	const char* objName = ComponentXml->Attribute("obj");
	if (objName != nullptr)
	{
		if (!rm.TryGetAndLoadResource(objName, &Model))
		{
			Model = std::make_shared<Resources::Model>(objName);
			Model->Path = ComponentXml->Attribute("path");
			Model->LoadModel();
			rm.RegisterAndLoadResource(Model);
		}
	}
	else
		Model = nullptr;

	const char* shaderName = ComponentXml->Attribute("shader");
	if (shaderName != nullptr)
	{
		if (!rm.TryGetAndLoadResource(shaderName, &Shader))
		{
			tinyxml2::XMLElement* shaderXML = nullptr;
			if (XMLMacro::GetElement(ComponentXml, &shaderXML, "shader"))
			{
				const char* vertexPath = shaderXML->Attribute("vertex");
				const char* fragmentPath = shaderXML->Attribute("fragment");
				
				if (vertexPath != nullptr && fragmentPath != nullptr)
					Shader = std::make_shared<Resources::Shader>(shaderName, vertexPath, fragmentPath);
				else
					MYTH_WARNING("scene loader : chemin des shaders invalide");
			}
			else
				Shader = std::make_shared<Resources::Shader>(shaderName);

			rm.RegisterAndLoadResource(Shader);
		}
	}
	else
		Shader = nullptr;

	DuplicatePart();

	tinyxml2::XMLElement* currentMaterialMod = nullptr;
	tinyxml2::XMLElement* currentMatPart = nullptr;
	for (std::pair<const std::string, Material>& constCurrentMaterial : m_materials)
	{
		if (!XMLMacro::GetElement(ComponentXml, &currentMaterialMod, ("material_" + constCurrentMaterial.first).c_str()))
			continue;

		constCurrentMaterial.second.m_shininess = currentMaterialMod->FloatAttribute("shininess", constCurrentMaterial.second.m_shininess);

		if (XMLMacro::GetElement(currentMaterialMod, &currentMatPart, "ambient"))
		{
			constCurrentMaterial.second.m_ambientColor.m_x = currentMatPart->FloatAttribute("color-R", constCurrentMaterial.second.m_ambientColor.m_x);
			constCurrentMaterial.second.m_ambientColor.m_y = currentMatPart->FloatAttribute("color-G", constCurrentMaterial.second.m_ambientColor.m_y);
			constCurrentMaterial.second.m_ambientColor.m_z = currentMatPart->FloatAttribute("color-B", constCurrentMaterial.second.m_ambientColor.m_z);
			constCurrentMaterial.second.m_ambientColor.m_w = currentMatPart->FloatAttribute("color-A", constCurrentMaterial.second.m_ambientColor.m_w);

			const char* textureName = currentMatPart->Attribute("texture");
			if (textureName != nullptr)
			{
				if (textureName == "")
					constCurrentMaterial.second.m_ambientTexture = nullptr;
				else
				{
					Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

					if (!rm.TryGetAndLoadResource(textureName, &constCurrentMaterial.second.m_ambientTexture))
					{
						constCurrentMaterial.second.m_ambientTexture = std::make_shared<Resources::Texture>(std::string(textureName), true);
						rm.RegisterAndLoadResource(constCurrentMaterial.second.m_ambientTexture);
					}
				}
			}

			if (constCurrentMaterial.second.m_ambientTexture != nullptr)
			{
				constCurrentMaterial.second.m_ambientTexture->UnloadResource();
				constCurrentMaterial.second.m_ambientTexture->m_flipOnLoad = currentMatPart->BoolAttribute("flip");
				constCurrentMaterial.second.m_ambientTexture->LoadResource();
			}
		}

		if (XMLMacro::GetElement(currentMaterialMod, &currentMatPart, "diffuse"))
		{
			constCurrentMaterial.second.m_diffuseColor.m_x = currentMatPart->FloatAttribute("color-R", constCurrentMaterial.second.m_diffuseColor.m_x);
			constCurrentMaterial.second.m_diffuseColor.m_y = currentMatPart->FloatAttribute("color-G", constCurrentMaterial.second.m_diffuseColor.m_y);
			constCurrentMaterial.second.m_diffuseColor.m_z = currentMatPart->FloatAttribute("color-B", constCurrentMaterial.second.m_diffuseColor.m_z);
			constCurrentMaterial.second.m_diffuseColor.m_w = currentMatPart->FloatAttribute("color-A", constCurrentMaterial.second.m_diffuseColor.m_w);

			const char* textureName = currentMatPart->Attribute("texture");
			if (textureName != nullptr)
			{
				if (textureName == "")
					constCurrentMaterial.second.m_diffuseTexture = nullptr;
				else
				{
					Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

					if (!rm.TryGetAndLoadResource(textureName, &constCurrentMaterial.second.m_diffuseTexture))
					{
						constCurrentMaterial.second.m_diffuseTexture = std::make_shared<Resources::Texture>(std::string(textureName), true);
						rm.RegisterAndLoadResource(constCurrentMaterial.second.m_diffuseTexture);
					}
				}
			}

			if (constCurrentMaterial.second.m_diffuseTexture != nullptr)
			{
				constCurrentMaterial.second.m_diffuseTexture->UnloadResource();
				constCurrentMaterial.second.m_diffuseTexture->m_flipOnLoad = currentMatPart->BoolAttribute("flip");
				constCurrentMaterial.second.m_diffuseTexture->LoadResource();
			}
		}

		if (XMLMacro::GetElement(currentMaterialMod, &currentMatPart, "specular"))
		{
			constCurrentMaterial.second.m_specularColor.m_x = currentMatPart->FloatAttribute("color-R", constCurrentMaterial.second.m_specularColor.m_x);
			constCurrentMaterial.second.m_specularColor.m_y = currentMatPart->FloatAttribute("color-G", constCurrentMaterial.second.m_specularColor.m_y);
			constCurrentMaterial.second.m_specularColor.m_z = currentMatPart->FloatAttribute("color-B", constCurrentMaterial.second.m_specularColor.m_z);
			constCurrentMaterial.second.m_specularColor.m_w = currentMatPart->FloatAttribute("color-A", constCurrentMaterial.second.m_specularColor.m_w);

			const char* textureName = currentMatPart->Attribute("texture");
			if (textureName != nullptr)
			{
				if (textureName == "")
					constCurrentMaterial.second.m_specularTexture = nullptr;
				else
				{
					Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

					if (!rm.TryGetAndLoadResource(textureName, &constCurrentMaterial.second.m_specularTexture))
					{
						constCurrentMaterial.second.m_specularTexture = std::make_shared<Resources::Texture>(std::string(textureName), true);
						rm.RegisterAndLoadResource(constCurrentMaterial.second.m_specularTexture);
					}
				}
			}

			if (constCurrentMaterial.second.m_specularTexture != nullptr)
			{
				constCurrentMaterial.second.m_specularTexture->UnloadResource();
				constCurrentMaterial.second.m_specularTexture->m_flipOnLoad = currentMatPart->BoolAttribute("flip");
				constCurrentMaterial.second.m_specularTexture->LoadResource();
			}
		}

		if (XMLMacro::GetElement(currentMaterialMod, &currentMatPart, "emissive"))
		{
			constCurrentMaterial.second.m_emissiveColor.m_x = currentMatPart->FloatAttribute("color-R", constCurrentMaterial.second.m_emissiveColor.m_x);
			constCurrentMaterial.second.m_emissiveColor.m_y = currentMatPart->FloatAttribute("color-G", constCurrentMaterial.second.m_emissiveColor.m_y);
			constCurrentMaterial.second.m_emissiveColor.m_z = currentMatPart->FloatAttribute("color-B", constCurrentMaterial.second.m_emissiveColor.m_z);
			constCurrentMaterial.second.m_emissiveColor.m_w = currentMatPart->FloatAttribute("color-A", constCurrentMaterial.second.m_emissiveColor.m_w);

			const char* textureName = currentMatPart->Attribute("texture");
			if (textureName != nullptr)
			{
				if (textureName == "")
					constCurrentMaterial.second.m_emissiveTexture = nullptr;
				else
				{
					Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

					if (!rm.TryGetAndLoadResource(textureName, &constCurrentMaterial.second.m_emissiveTexture))
					{
						constCurrentMaterial.second.m_emissiveTexture = std::make_shared<Resources::Texture>(std::string(textureName), true);
						rm.RegisterAndLoadResource(constCurrentMaterial.second.m_emissiveTexture);
					}
				}
			}

			if (constCurrentMaterial.second.m_emissiveTexture != nullptr)
			{
				constCurrentMaterial.second.m_emissiveTexture->UnloadResource();
				constCurrentMaterial.second.m_emissiveTexture->m_flipOnLoad = currentMatPart->BoolAttribute("flip");
				constCurrentMaterial.second.m_emissiveTexture->LoadResource();
			}
		}

		if (XMLMacro::GetElement(currentMaterialMod, &currentMatPart, "normal_map"))
		{
			const char* textureName = currentMatPart->Attribute("texture");
			if (textureName != nullptr)
			{
				if (textureName == "")
					constCurrentMaterial.second.m_normalMap = nullptr;
				else
				{
					Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

					if (!rm.TryGetAndLoadResource(textureName, &constCurrentMaterial.second.m_normalMap))
					{
						constCurrentMaterial.second.m_normalMap = std::make_shared<Resources::Texture>(std::string(textureName), true);
						rm.RegisterAndLoadResource(constCurrentMaterial.second.m_normalMap);
					}
				}
			}

			if (constCurrentMaterial.second.m_normalMap != nullptr)
			{
				constCurrentMaterial.second.m_normalMap->UnloadResource();
				constCurrentMaterial.second.m_normalMap->m_flipOnLoad = currentMatPart->BoolAttribute("flip");
				constCurrentMaterial.second.m_normalMap->LoadResource();
			}
		}
	}
}

#ifdef MODE_EDITOR
void ECS::ModelInstance::ModelCreator()
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Model Creator", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string newModelName = "";
		ImGui::Text("Model name :");
		ImGui::SameLine();
		ImGui::InputText("##ModelName_ModelCreator", &newModelName);

		static std::string newModelPath = "";
		ImGui::Text("Model path :");
		ImGui::SameLine();
		ImGui::InputText("##ModelPath_ModelCreator", &newModelPath);

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) && !newModelName.empty())
		{
			Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

			if (!rm.TryGetAndLoadResource<Resources::Model>(newModelName, &Model))
			{
				Model = std::make_shared<Resources::Model>(newModelName);
				Model->Path = newModelPath;
				Model->LoadModel();
				rm.RegisterAndLoadResource(Model);

				m_materials.clear();
				DuplicatePart();
			}
			else
			{
				m_materials.clear();
				DuplicatePart();

				MYTH_WARNING("Model '%s' already exist", newModelName.c_str());
			}

			newModelName = "";
			newModelPath = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { newModelName = ""; newModelPath = ""; ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void ECS::ModelInstance::ShaderCreator()
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Shader Creator", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string newShaderName = "";
		static bool nameIsShaderFileName = true;
		static std::string newVertexShaderPath = "";
		static std::string newFragmentShaderPath = "";

		ImGui::Text("Shader name :");
		ImGui::SameLine();
		if (ImGui::InputText("##ShaderName_ShaderCreator", &newShaderName))
			if (nameIsShaderFileName && !newShaderName.empty())
			{
				newVertexShaderPath = "resources/shaders/" + newShaderName + ".vert";
				newFragmentShaderPath = "resources/shaders/" + newShaderName + ".frag";
			}

		ImGui::Checkbox("Name is filename ?##ShaderCreator", &nameIsShaderFileName);

		ImGui::Text("Vertex shader path :");
		ImGui::SameLine();
		ImGui::InputText("##VertexShaderPath_ShaderCreator", &newVertexShaderPath, (nameIsShaderFileName ? ImGuiInputTextFlags_ReadOnly : 0));

		ImGui::Text("Fragment shader path :");
		ImGui::SameLine();
		ImGui::InputText("##FragmentShaderPath_ShaderCreator", &newFragmentShaderPath, (nameIsShaderFileName ? ImGuiInputTextFlags_ReadOnly : 0));

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) && !newShaderName.empty())
		{
			Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

			if (!rm.TryGetAndLoadResource<Resources::Shader>(newShaderName, &Shader))
			{
				if (nameIsShaderFileName)
					Shader = std::make_shared<Resources::Shader>(newShaderName);
				else
					Shader = std::make_shared<Resources::Shader>(newShaderName, newVertexShaderPath.c_str(), newFragmentShaderPath.c_str());
				
				rm.RegisterAndLoadResource(Shader);
			}
			else
				MYTH_WARNING("Shader '%s' already exist", newShaderName.c_str());

			newShaderName = "";
			nameIsShaderFileName = true;
			newVertexShaderPath = "";
			newFragmentShaderPath = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { newShaderName = ""; nameIsShaderFileName = true; newVertexShaderPath = ""; newFragmentShaderPath = ""; ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void TextureCreator()
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Texture Creator", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string newTexturePath = "";
		ImGui::Text("Texture path :");
		ImGui::SameLine();
		ImGui::InputText("##TexturePath_TextureCreator", &newTexturePath);

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) && !newTexturePath.empty())
		{
			Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

			if (!rm.TryGetAndLoadResource<Resources::Texture>(newTexturePath, nullptr))
				rm.RegisterAndLoadResource(std::make_shared<Resources::Texture>(newTexturePath, true));
			else
				MYTH_WARNING("Texture '%s' already exist", newTexturePath.c_str());

			newTexturePath = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { newTexturePath = ""; ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void ECS::ModelInstance::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

	static std::string ModelName = "NONE";
	ModelName = (Model == nullptr ? "NONE" : Model->GetName());
	ImGui::Text("Model :");
	ImGui::SameLine();
	ImGui::InputText(("##modelOfmodelInstance_" + std::to_string(p_indexComponent)).c_str(), &ModelName, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
		{
			if (Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &Model))
			{
				m_materials.clear();
				DuplicatePart();
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::SameLine();
	if (ImGui::Button(("X##ModelModelInstance_" + std::to_string(p_indexComponent)).c_str()))
	{
		Model = nullptr;
		m_materials.clear();
	}
	ImGui::SameLine();
	if (ImGui::Button(("Create model##modelInstance_" + std::to_string(p_indexComponent)).c_str()))
		ImGui::OpenPopup("Model Creator");
	ModelCreator();

	static std::string ShaderlName = "NONE";
	ShaderlName = (Shader == nullptr ? "NONE" : Shader->GetName());
	ImGui::Text("Shader :");
	ImGui::SameLine();
	ImGui::InputText(("##ShaderOfmodelInstance_" + std::to_string(p_indexComponent)).c_str(), &ShaderlName, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
			Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &Shader);
		ImGui::EndDragDropTarget();
	}
	ImGui::SameLine();
	if (ImGui::Button(("X##ShaderModelInstance_" + std::to_string(p_indexComponent)).c_str()))
		Shader = nullptr;
	ImGui::SameLine();
	if (ImGui::Button(("Create shader##modelInstance_" + std::to_string(p_indexComponent)).c_str()))
		ImGui::OpenPopup("Shader Creator");
	ShaderCreator();

	ImGui::Checkbox(("Use Light##modelInstance_" + std::to_string(p_indexComponent)).c_str(), &UseLight);

	ImGui::Spacing();

	ImGui::InputInt(("Transform##modelInstance_" + std::to_string(p_indexComponent)).c_str(), (int*)&IndexTransform);

	ImGui::Spacing();

	if (ImGui::TreeNode(("Material##ModelInstance_" + std::to_string(p_indexComponent)).c_str()))
	{
		if (ImGui::Button(("Add new texture##modelInstance_" + std::to_string(p_indexComponent)).c_str()))
			ImGui::OpenPopup("Texture Creator");
		TextureCreator();

		static std::string TextureName = "NONE";
		for (std::pair<const std::string, Material>& currentMaterial : m_materials)
		{
			if (ImGui::TreeNode((currentMaterial.first + "##ModelInstance_" + std::to_string(p_indexComponent)).c_str()))
			{
				ImGui::DragFloat(("shininess##" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), &currentMaterial.second.m_shininess, 0.1f);
				ImGui::Spacing();

				ImGui::ColorEdit4(("ambient##" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), (float*)&currentMaterial.second.m_ambientColor);
				TextureName = (currentMaterial.second.m_ambientTexture == nullptr ? "NONE" : currentMaterial.second.m_ambientTexture->GetName());
				ImGui::Text("texture :");
				ImGui::SameLine();
				ImGui::InputText(("##ambientTexture_" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), &TextureName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &currentMaterial.second.m_ambientTexture);
					
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Button(("X##TAmbientModelInstance_" + std::to_string(p_indexComponent)).c_str()))
					currentMaterial.second.m_ambientTexture = nullptr;
				ImGui::Spacing();

				ImGui::ColorEdit4(("diffuse##" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), (float*)&currentMaterial.second.m_diffuseColor);
				TextureName = (currentMaterial.second.m_diffuseTexture == nullptr ? "NONE" : currentMaterial.second.m_diffuseTexture->GetName());
				ImGui::Text("texture :");
				ImGui::SameLine();
				ImGui::InputText(("##diffuseTexture_" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), &TextureName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &currentMaterial.second.m_diffuseTexture);
					
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Button(("X##TDiffuseModelInstance_" + std::to_string(p_indexComponent)).c_str()))
					currentMaterial.second.m_diffuseTexture = nullptr;
				ImGui::Spacing();

				ImGui::ColorEdit4(("specular##" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), (float*)&currentMaterial.second.m_specularColor);
				TextureName = (currentMaterial.second.m_specularTexture == nullptr ? "NONE" : currentMaterial.second.m_specularTexture->GetName());
				ImGui::Text("texture :");
				ImGui::SameLine();
				ImGui::InputText(("##specularTexture_" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), &TextureName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &currentMaterial.second.m_specularTexture);

					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Button(("X##TSpecularModelInstance_" + std::to_string(p_indexComponent)).c_str()))
					currentMaterial.second.m_specularTexture = nullptr;
				ImGui::Spacing();

				ImGui::ColorEdit4(("emissive##" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), (float*)&currentMaterial.second.m_emissiveColor);
				TextureName = (currentMaterial.second.m_emissiveTexture == nullptr ? "NONE" : currentMaterial.second.m_emissiveTexture->GetName());
				ImGui::Text("texture :");
				ImGui::SameLine();
				ImGui::InputText(("##emissiveTexture_" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), &TextureName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &currentMaterial.second.m_emissiveTexture);
					
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Button(("X##TEmissiveModelInstance_" + std::to_string(p_indexComponent)).c_str()))
					currentMaterial.second.m_emissiveTexture = nullptr;
				ImGui::Spacing();

				TextureName = (currentMaterial.second.m_normalMap == nullptr ? "NONE" : currentMaterial.second.m_normalMap->GetName());
				ImGui::Text("normal map");
				ImGui::Text("texture :");
				ImGui::SameLine();
				ImGui::InputText(("##normalMap_" + currentMaterial.first + "_ModelInstance_" + std::to_string(p_indexComponent)).c_str(), &TextureName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &currentMaterial.second.m_normalMap);

					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Button(("X##TNormalMap_ModelInstance_" + std::to_string(p_indexComponent)).c_str()))
					currentMaterial.second.m_normalMap = nullptr;
				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void ECS::ModelInstance::Save(tinyxml2::XMLElement* ComponentXml)
{
	ComponentXml->SetAttribute("light", UseLight);

	if (IndexTransform > 0u)
		ComponentXml->SetAttribute("index-transform", IndexTransform);

	if (Model != nullptr)
	{
		ComponentXml->SetAttribute("obj", Model->GetName().c_str());
		ComponentXml->SetAttribute("path", Model->Path.c_str());

		for (const std::pair<std::string, Resources::DefaultMaterial>& constCurrentMaterial : Model->GetDefaultMaterials())
		{
			bool Difference[15] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
			Material& currentMat = m_materials[constCurrentMaterial.first];

			Difference[14] = constCurrentMaterial.second.m_shininess != currentMat.m_shininess;

			Difference[0] = constCurrentMaterial.second.m_ambient.color != currentMat.m_ambientColor;
			Difference[1] = constCurrentMaterial.second.m_diffuse.color != currentMat.m_diffuseColor;
			Difference[2] = constCurrentMaterial.second.m_specular.color != currentMat.m_specularColor;
			Difference[3] = constCurrentMaterial.second.m_emissive.color != currentMat.m_emissiveColor;

			if (currentMat.m_ambientTexture != nullptr)
				Difference[4] = currentMat.m_ambientTexture->GetName() != constCurrentMaterial.second.m_ambient.name;
			else
				Difference[4] = !constCurrentMaterial.second.m_ambient.name.empty();

			if (currentMat.m_diffuseTexture != nullptr)
				Difference[5] = currentMat.m_diffuseTexture->GetName() != constCurrentMaterial.second.m_diffuse.name;
			else
				Difference[5] = !constCurrentMaterial.second.m_diffuse.name.empty();

			if (currentMat.m_specularTexture != nullptr)
				Difference[6] = currentMat.m_specularTexture->GetName() != constCurrentMaterial.second.m_specular.name;
			else
				Difference[6] = !constCurrentMaterial.second.m_specular.name.empty();

			if (currentMat.m_emissiveTexture != nullptr)
				Difference[7] = currentMat.m_emissiveTexture->GetName() != constCurrentMaterial.second.m_emissive.name;
			else
				Difference[7] = !constCurrentMaterial.second.m_emissive.name.empty();

			if (currentMat.m_normalMap != nullptr)
				Difference[8] = currentMat.m_normalMap->GetName() != constCurrentMaterial.second.m_normalMap.name;
			else
				Difference[8] = !constCurrentMaterial.second.m_normalMap.name.empty();

			if (currentMat.m_ambientTexture != nullptr)
				Difference[9] = currentMat.m_ambientTexture->m_flipOnLoad;
			if (currentMat.m_diffuseTexture != nullptr)
				Difference[10] = currentMat.m_diffuseTexture->m_flipOnLoad;
			if (currentMat.m_specularTexture != nullptr)
				Difference[11] = currentMat.m_specularTexture->m_flipOnLoad;
			if (currentMat.m_emissiveTexture != nullptr)
				Difference[12] = currentMat.m_emissiveTexture->m_flipOnLoad;
			if (currentMat.m_normalMap != nullptr)
				Difference[13] = currentMat.m_normalMap->m_flipOnLoad;

			if (Difference[0]  || Difference[1]  || Difference[2]  || Difference[3]  ||
				Difference[4]  || Difference[5]  || Difference[6]  || Difference[7]  ||
				Difference[8]  || Difference[9]  || Difference[10] || Difference[11] ||
				Difference[12] || Difference[13] || Difference[14])
			{
				tinyxml2::XMLElement* currentMaterialMod = ComponentXml->InsertNewChildElement(("material_" + constCurrentMaterial.first).c_str());

				if (Difference[14])
					currentMaterialMod->SetAttribute("shininess", currentMat.m_shininess);

				if (Difference[0] || Difference[4] || Difference[9])
				{
					tinyxml2::XMLElement* currentAmbient = currentMaterialMod->InsertNewChildElement("ambient");
					if (Difference[0])
					{
						currentAmbient->SetAttribute("color-R", currentMat.m_ambientColor.m_x);
						currentAmbient->SetAttribute("color-G", currentMat.m_ambientColor.m_y);
						currentAmbient->SetAttribute("color-B", currentMat.m_ambientColor.m_z);
						currentAmbient->SetAttribute("color-A", currentMat.m_ambientColor.m_w);
					}
					if (Difference[4]) { currentAmbient->SetAttribute("texture", (currentMat.m_ambientTexture == nullptr ? "" : currentMat.m_ambientTexture->GetName().c_str())); }
					if (Difference[9]) { currentAmbient->SetAttribute("flip", true); }
				}
				if (Difference[1] || Difference[5] || Difference[10])
				{
					tinyxml2::XMLElement* currentDiffuse = currentMaterialMod->InsertNewChildElement("diffuse");
					if (Difference[1])
					{
						currentDiffuse->SetAttribute("color-R", currentMat.m_diffuseColor.m_x);
						currentDiffuse->SetAttribute("color-G", currentMat.m_diffuseColor.m_y);
						currentDiffuse->SetAttribute("color-B", currentMat.m_diffuseColor.m_z);
						currentDiffuse->SetAttribute("color-A", currentMat.m_diffuseColor.m_w);
					}
					if (Difference[5]) { currentDiffuse->SetAttribute("texture", (currentMat.m_diffuseTexture == nullptr ? "" : currentMat.m_diffuseTexture->GetName().c_str())); }
					if (Difference[10]) { currentDiffuse->SetAttribute("flip", true); }
				}
				if (Difference[2] || Difference[6] || Difference[11])
				{
					tinyxml2::XMLElement* currentSpecular = currentMaterialMod->InsertNewChildElement("specular");
					if (Difference[2])
					{
						currentSpecular->SetAttribute("color-R", currentMat.m_specularColor.m_x);
						currentSpecular->SetAttribute("color-G", currentMat.m_specularColor.m_y);
						currentSpecular->SetAttribute("color-B", currentMat.m_specularColor.m_z);
						currentSpecular->SetAttribute("color-A", currentMat.m_specularColor.m_w);
					}
					if (Difference[6]) { currentSpecular->SetAttribute("texture", (currentMat.m_specularTexture == nullptr ? "" : currentMat.m_specularTexture->GetName().c_str())); }
					if (Difference[11]) { currentSpecular->SetAttribute("flip", true); }
				}
				if (Difference[3] || Difference[7] || Difference[12])
				{
					tinyxml2::XMLElement* currentEmissive = currentMaterialMod->InsertNewChildElement("emissive");
					if (Difference[3])
					{
						currentEmissive->SetAttribute("color-R", currentMat.m_emissiveColor.m_x);
						currentEmissive->SetAttribute("color-G", currentMat.m_emissiveColor.m_y);
						currentEmissive->SetAttribute("color-B", currentMat.m_emissiveColor.m_z);
						currentEmissive->SetAttribute("color-A", currentMat.m_emissiveColor.m_w);
					}
					if (Difference[7]) { currentEmissive->SetAttribute("texture", (currentMat.m_emissiveTexture == nullptr ? "" : currentMat.m_emissiveTexture->GetName().c_str())); }
					if (Difference[12]) { currentEmissive->SetAttribute("flip", true); }
				}

				if (Difference[8] || Difference[13])
				{
					tinyxml2::XMLElement* currentNormalMap = currentMaterialMod->InsertNewChildElement("normal_map");
					if (Difference[8]) { currentNormalMap->SetAttribute("texture", (currentMat.m_normalMap == nullptr ? "" : currentMat.m_normalMap->GetName().c_str())); }
					if (Difference[13]) { currentNormalMap->SetAttribute("flip", true); }
				}
			}
		}
	}

	if (Shader != nullptr)
	{
		ComponentXml->SetAttribute("shader", Shader->GetName().c_str());

		if (("resources/shaders/" + Shader->GetName() + ".vert") != Shader->PathVertex ||
			("resources/shaders/" + Shader->GetName() + ".frag") != Shader->PathFragment)
		{
			tinyxml2::XMLElement* shaderElement = ComponentXml->InsertNewChildElement("shader");

			shaderElement->SetAttribute("vertex", Shader->PathVertex.c_str());
			shaderElement->SetAttribute("fragment", Shader->PathFragment.c_str());
		}
	}
}
#endif // MODE_EDITOR

#include "Core/System/LightManager.h"

void ECS::ModelInstance::Draw(Core::GameObject* p_gameObject)
{
	if (Model == nullptr || Shader == nullptr)
		return;
	if (Model->GetAssimpScene() == nullptr)
		return;

	Mat4x4 proj = Core::System::CameraManager::GetInstance().GetProjectionMatrix();

	Mat4x4 view = Mat4x4::ViewRad(0.f, 0.f, { 0.f, 0.f, 5.f });
	Vec3   ViewPos = Vec3::Zero;
	if (Core::System::CameraManager::GetInstance().HasCamera())
	{
		view = Core::System::CameraManager::GetInstance().GetCamera()->GetViewMatrix();
		ViewPos = Core::System::CameraManager::GetInstance().GetCamera()->Position;
	}

	Mat4x4 viewProj = proj * view;

	glEnable(GL_DEPTH_TEST);
	Shader->Use();
	Shader->SetMatrix4x4("uVP", viewProj);
	Shader->SetVec3("uViewPos", ViewPos);

	if (UseLight)
	{
		const ECS::DirectionLight* dirLight = Core::System::LightManager::GetDirectionLight();
		if (dirLight != nullptr)
		{
			Shader->SetVec3("uDirectionLight.direction", dirLight->Direction);

			Shader->SetVec3("uDirectionLight.ambient", dirLight->Ambient);
			Shader->SetVec3("uDirectionLight.diffuse", dirLight->Diffuse);
			Shader->SetVec3("uDirectionLight.specular", dirLight->Specular);
		}
		else
		{
			Shader->SetVec3("uDirectionLight.direction", Vec3::Zero);

			Shader->SetVec3("uDirectionLight.ambient", Vec3::Zero);
			Shader->SetVec3("uDirectionLight.diffuse", Vec3::Zero);
			Shader->SetVec3("uDirectionLight.specular", Vec3::Zero);
		}

		const std::vector<ECS::PointLight*>& currentLights = Core::System::LightManager::GetPointLights();
		size_t skiped = 0u;
		for (size_t i = 0u; i < currentLights.size(); i++)
		{
			if (!(currentLights[i]->Enabled()))
			{
				skiped++;
				continue;
			}

			Shader->SetVec3(fmt::format("uPointLight[{}].position", (i - skiped)).c_str(), currentLights[i]->Position);

			Shader->SetFloat(fmt::format("uPointLight[{}].constant", (i - skiped)).c_str(), currentLights[i]->Constant);
			Shader->SetFloat(fmt::format("uPointLight[{}].linear", (i - skiped)).c_str(), currentLights[i]->Linear);
			Shader->SetFloat(fmt::format("uPointLight[{}].quadratic", (i - skiped)).c_str(), currentLights[i]->Quadratic);

			Shader->SetVec3(fmt::format("uPointLight[{}].ambient", (i - skiped)).c_str(), currentLights[i]->Ambient);
			Shader->SetVec3(fmt::format("uPointLight[{}].diffuse", (i - skiped)).c_str(), currentLights[i]->Diffuse);
			Shader->SetVec3(fmt::format("uPointLight[{}].specular", (i - skiped)).c_str(), currentLights[i]->Specular);
		}

		Shader->SetInt("uPointLightCount", (int)(currentLights.size() - skiped));
	}

	Shader->SetTextureLocation("uMaterial.ambient.texture", 2);
	Shader->SetTextureLocation("uMaterial.diffuse.texture", 0);
	Shader->SetTextureLocation("uMaterial.specular.texture", 1);
	Shader->SetTextureLocation("uMaterial.emissive.texture", 3);
	Shader->SetTextureLocation("uMaterial.normalMap.texture", 4);

	for (unsigned int i = 0; i < m_boneTransforms.size(); i++)
	{
		std::string name = fmt::format("uBones[{}]", i);
		if (m_hasAnim)
			Shader->SetMatrix4x4(name.c_str(), m_boneTransforms[i]);
		else
			Shader->SetMatrix4x4(name.c_str(), Mat4x4::Identity);
	}

	for (unsigned int indexMesh = 0u; indexMesh < Model->GetMeshs().size(); indexMesh++)
	{
		const Resources::Mesh& Mesh = Model->GetMeshs()[indexMesh];

		if (m_materials.find(Mesh.MaterialName) == m_materials.end() || indexMesh >= m_localMatrix.size())
			continue;

		Shader->SetMatrix4x4("uModel", m_worldMatrix * m_localMatrix[indexMesh]);
		glBindVertexArray(Mesh.Gpu.VAO);

		Material& currentMaterial = m_materials[Mesh.MaterialName];

		Shader->SetFloat("uMaterial.shininess", currentMaterial.m_shininess);

		Shader->SetVec4("uMaterial.ambient.color", currentMaterial.m_ambientColor);
		Shader->SetBool("uMaterial.ambient.hasTexture", currentMaterial.m_ambientTexture != nullptr);
		if (currentMaterial.m_ambientTexture != nullptr)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, currentMaterial.m_ambientTexture->Gpu);
		}

		Shader->SetVec4("uMaterial.diffuse.color", currentMaterial.m_diffuseColor);
		Shader->SetBool("uMaterial.diffuse.hasTexture", currentMaterial.m_diffuseTexture != nullptr);
		if (currentMaterial.m_diffuseTexture != nullptr)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, currentMaterial.m_diffuseTexture->Gpu);
		}

		Shader->SetVec4("uMaterial.specular.color", currentMaterial.m_specularColor);
		Shader->SetBool("uMaterial.specular.hasTexture", currentMaterial.m_specularTexture != nullptr);
		if (currentMaterial.m_specularTexture != nullptr)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, currentMaterial.m_specularTexture->Gpu);
		}

		Shader->SetVec4("uMaterial.emissive.color", currentMaterial.m_emissiveColor);
		Shader->SetBool("uMaterial.emissive.hasTexture", currentMaterial.m_emissiveTexture != nullptr);
		if (currentMaterial.m_emissiveTexture != nullptr)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, currentMaterial.m_emissiveTexture->Gpu);
		}

		Shader->SetVec4("uMaterial.normalMap.color", {0.f, 0.f, 0.f, 0.f});
		Shader->SetBool("uMaterial.normalMap.hasTexture", currentMaterial.m_normalMap != nullptr);
		if (currentMaterial.m_normalMap != nullptr)
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, currentMaterial.m_normalMap->Gpu);
		}

		glDrawArrays(GL_TRIANGLES, 0, Mesh.Gpu.VertexCount);
		glBindVertexArray(0);

		for (int i = 0; i < 5; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void ECS::ModelInstance::DuplicatePart()
{
	if (Model == nullptr)
		return;

	for (const std::pair<std::string, Resources::DefaultMaterial>& currentDefaultMaterial : Model->GetDefaultMaterials())
	{
		if (m_materials.find(currentDefaultMaterial.first) != m_materials.end())
			continue;

		m_materials.insert({ currentDefaultMaterial.first, Material() });
		Material& currentMaterial = m_materials[currentDefaultMaterial.first];

		currentMaterial.m_shininess = currentDefaultMaterial.second.m_shininess;

		currentMaterial.m_ambientColor = currentDefaultMaterial.second.m_ambient.color;
		currentMaterial.m_diffuseColor = currentDefaultMaterial.second.m_diffuse.color;
		currentMaterial.m_specularColor = currentDefaultMaterial.second.m_specular.color;
		currentMaterial.m_emissiveColor = currentDefaultMaterial.second.m_emissive.color;

		Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

		if (currentDefaultMaterial.second.m_ambient.name != "")
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>(currentDefaultMaterial.second.m_ambient.name, &currentMaterial.m_ambientTexture))
			{
				currentMaterial.m_ambientTexture = std::make_shared<Resources::Texture>(currentDefaultMaterial.second.m_ambient.name, true, currentDefaultMaterial.second.m_ambient.isEmbedded);
				currentMaterial.m_ambientTexture->Type = Resources::TYPE_TEXTURE::AMBIENT;
				currentMaterial.m_ambientTexture->height = currentDefaultMaterial.second.m_ambient.height;
				currentMaterial.m_ambientTexture->width = currentDefaultMaterial.second.m_ambient.width;
				currentMaterial.m_ambientTexture->texel = currentDefaultMaterial.second.m_ambient.texel;
				rm.RegisterAndLoadResource(currentMaterial.m_ambientTexture);
			}
		}
		else
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>("resources/assets/blackPixel.png", &currentMaterial.m_ambientTexture))
			{
				currentMaterial.m_ambientTexture = std::make_shared<Resources::Texture>("resources/assets/blackPixel.png", true);
				currentMaterial.m_ambientTexture->Type = Resources::TYPE_TEXTURE::AMBIENT;
				currentMaterial.m_ambientTexture->height = 0;
				currentMaterial.m_ambientTexture->width = 0;
				currentMaterial.m_ambientTexture->texel = 0;
				rm.RegisterAndLoadResource(currentMaterial.m_ambientTexture);
			}
		}

		if (currentDefaultMaterial.second.m_diffuse.name != "")
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>(currentDefaultMaterial.second.m_diffuse.name, &currentMaterial.m_diffuseTexture))
			{
				currentMaterial.m_diffuseTexture = std::make_shared<Resources::Texture>(currentDefaultMaterial.second.m_diffuse.name, true, currentDefaultMaterial.second.m_diffuse.isEmbedded);
				currentMaterial.m_diffuseTexture->Type = Resources::TYPE_TEXTURE::DIFFUSE;
				currentMaterial.m_diffuseTexture->height = currentDefaultMaterial.second.m_diffuse.height;
				currentMaterial.m_diffuseTexture->width = currentDefaultMaterial.second.m_diffuse.width;
				currentMaterial.m_diffuseTexture->texel = currentDefaultMaterial.second.m_diffuse.texel;
				rm.RegisterAndLoadResource(currentMaterial.m_diffuseTexture);
			}
		}
		else
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>("resources/assets/blackPixel.png", &currentMaterial.m_diffuseTexture))
			{
				currentMaterial.m_diffuseTexture = std::make_shared<Resources::Texture>("resources/assets/blackPixel.png", true);
				currentMaterial.m_diffuseTexture->Type = Resources::TYPE_TEXTURE::DIFFUSE;
				currentMaterial.m_diffuseTexture->height = 0;
				currentMaterial.m_diffuseTexture->width = 0;
				currentMaterial.m_diffuseTexture->texel = 0;
				rm.RegisterAndLoadResource(currentMaterial.m_diffuseTexture);
			}
		}

		if (currentDefaultMaterial.second.m_specular.name != "")
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>(currentDefaultMaterial.second.m_specular.name, &currentMaterial.m_specularTexture))
			{
				currentMaterial.m_specularTexture = std::make_shared<Resources::Texture>(currentDefaultMaterial.second.m_specular.name, true, currentDefaultMaterial.second.m_specular.isEmbedded);
				currentMaterial.m_specularTexture->Type = Resources::TYPE_TEXTURE::SPECULAR;
				currentMaterial.m_specularTexture->height = currentDefaultMaterial.second.m_specular.height;
				currentMaterial.m_specularTexture->width = currentDefaultMaterial.second.m_specular.width;
				currentMaterial.m_specularTexture->texel = currentDefaultMaterial.second.m_specular.texel;
				rm.RegisterAndLoadResource(currentMaterial.m_specularTexture);
			}
		}
		else
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>("resources/assets/blackPixel.png", &currentMaterial.m_specularTexture))
			{
				currentMaterial.m_specularTexture = std::make_shared<Resources::Texture>("resources/assets/blackPixel.png", true);
				currentMaterial.m_specularTexture->Type = Resources::TYPE_TEXTURE::SPECULAR;
				currentMaterial.m_specularTexture->height = 0;
				currentMaterial.m_specularTexture->width = 0;
				currentMaterial.m_specularTexture->texel = 0;
				rm.RegisterAndLoadResource(currentMaterial.m_specularTexture);
			}
		}

		if (currentDefaultMaterial.second.m_emissive.name != "")
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>(currentDefaultMaterial.second.m_emissive.name, &currentMaterial.m_emissiveTexture))
			{
				currentMaterial.m_emissiveTexture = std::make_shared<Resources::Texture>(currentDefaultMaterial.second.m_emissive.name, true, currentDefaultMaterial.second.m_emissive.isEmbedded);
				currentMaterial.m_emissiveTexture->Type = Resources::TYPE_TEXTURE::EMISSIVE;
				currentMaterial.m_emissiveTexture->height = currentDefaultMaterial.second.m_emissive.height;
				currentMaterial.m_emissiveTexture->width = currentDefaultMaterial.second.m_emissive.width;
				currentMaterial.m_emissiveTexture->texel = currentDefaultMaterial.second.m_emissive.texel;
				rm.RegisterAndLoadResource(currentMaterial.m_emissiveTexture);
			}
		}
		else
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>("resources/assets/blackPixel.png", &currentMaterial.m_emissiveTexture))
			{
				currentMaterial.m_emissiveTexture = std::make_shared<Resources::Texture>("resources/assets/blackPixel.png", true);
				currentMaterial.m_emissiveTexture->Type = Resources::TYPE_TEXTURE::EMISSIVE;
				currentMaterial.m_emissiveTexture->height = 0;
				currentMaterial.m_emissiveTexture->width = 0;
				currentMaterial.m_emissiveTexture->texel = 0;
				rm.RegisterAndLoadResource(currentMaterial.m_emissiveTexture);
			}
		}

		if (currentDefaultMaterial.second.m_normalMap.name != "")
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>(currentDefaultMaterial.second.m_normalMap.name, &currentMaterial.m_normalMap))
			{
				currentMaterial.m_normalMap = std::make_shared<Resources::Texture>(currentDefaultMaterial.second.m_normalMap.name, true, currentDefaultMaterial.second.m_normalMap.isEmbedded);
				currentMaterial.m_normalMap->Type = Resources::TYPE_TEXTURE::NORMAL_MAP;
				currentMaterial.m_normalMap->height = currentDefaultMaterial.second.m_normalMap.height;
				currentMaterial.m_normalMap->width = currentDefaultMaterial.second.m_normalMap.width;
				currentMaterial.m_normalMap->texel = currentDefaultMaterial.second.m_normalMap.texel;
				rm.RegisterAndLoadResource(currentMaterial.m_normalMap);
			}
		}
		else
		{
			if (!rm.TryGetAndLoadResource<Resources::Texture>("resources/assets/blank.png", &currentMaterial.m_normalMap))
			{
				currentMaterial.m_normalMap = std::make_shared<Resources::Texture>("resources/assets/blank.png", true);
				currentMaterial.m_normalMap->Type = Resources::TYPE_TEXTURE::NORMAL_MAP;
				currentMaterial.m_normalMap->height = 0;
				currentMaterial.m_normalMap->width = 0;
				currentMaterial.m_normalMap->texel = 0;
				rm.RegisterAndLoadResource(currentMaterial.m_normalMap);
			}
		}
	}

	m_localMatrix.clear();
	for (const Mat4x4& currentMat : Model->GetDefaultLocalMatrix())
		m_localMatrix.push_back(currentMat);

	m_boneTransforms = Model->m_boneTransforms;
}
