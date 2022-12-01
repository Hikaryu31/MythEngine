#include "Resources/Scene.h"

#include "Core/System/XMLMacro.h"
#include "Core/Debug/Log.h"

//std::unordered_map<size_t, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)>> Resources::BasicScene::m_sComponentLoader;
std::unordered_map<std::string, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)>> Resources::BasicScene::m_sComponentLoader;
std::string Resources::BasicScene::GlobalScenePath = "resources/scene/";

using namespace tinyxml2;

void Resources::BasicScene::RegisterGameObjects(std::vector<std::unique_ptr<Core::GameObject>>& p_GameObjectList)
{
	XMLDocument currentSceneDocument;
	if (currentSceneDocument.LoadFile((GlobalScenePath + GetName() + ".xml").c_str()) != XML_SUCCESS)
	{
		MYTH_ERROR("error on loading file : %d", (int)currentSceneDocument.ErrorID());
		return;
	}

	XMLElement* lstGameObjectsXML = nullptr;
	XMLElement* GameObjectXML = nullptr;
	XMLElement* ComponentXML = nullptr;

	if (XMLMacro::GetElement(currentSceneDocument, &lstGameObjectsXML, "gameobjects"))
		XMLMacro::GetElement(lstGameObjectsXML, &GameObjectXML, "gameobject");

	Core::GameObject* tempNewGameobject = nullptr;
	ECS::Component* tempNewComponent = nullptr;

	while (GameObjectXML != nullptr)
	{
		tempNewGameobject = new Core::GameObject(GameObjectXML->Attribute("name"));

		ComponentXML = nullptr;
		XMLMacro::GetElement(GameObjectXML, &ComponentXML, "component");

		while (ComponentXML != nullptr)
		{
			//size_t hashComponent = ComponentXML->Unsigned64Attribute("type");
			const char* nameComponent = ComponentXML->Attribute("type");
			if (m_sComponentLoader.find(nameComponent) != m_sComponentLoader.end())
			{
				m_sComponentLoader[nameComponent](tempNewGameobject->GetComponents());
				tempNewGameobject->GetComponents().back()->SetEnabled(ComponentXML->BoolAttribute("enabled", true));
				tempNewGameobject->GetComponents().back()->Load(ComponentXML);
				tempNewGameobject->GetComponents().back()->Load(ComponentXML, tempNewGameobject);
			}
			else
				MYTH_WARNING("BasicScene can't find component '%s'", nameComponent);
			
			// end Component loop
			XMLMacro::GetNextElement(&ComponentXML, "component");
		}

		// end GameObject loop
		p_GameObjectList.emplace_back(tempNewGameobject);
		XMLMacro::GetNextElement(&GameObjectXML, "gameobject");
	}
}

/*void Resources::BasicScene::AddComponentType(const size_t& p_classHash, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)> p_callback)
{
	if (m_sComponentLoader.find(p_classHash) != m_sComponentLoader.end())
		return;

	m_sComponentLoader.insert({ p_classHash, p_callback });
}/**/
void Resources::BasicScene::AddComponentType(const char* p_className, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)> p_callback)
{
	if (m_sComponentLoader.find(p_className) != m_sComponentLoader.end())
		return;

	m_sComponentLoader.insert({ p_className, p_callback });
}/**/

#ifdef MODE_EDITOR
void Resources::BasicScene::SaveGameObjects(/*const std::unordered_map<std::string, std::shared_ptr<Resource>>& p_resourcesList,*/const std::vector<std::unique_ptr<Core::GameObject>>& p_gameObjectList)
{
	XMLDocument currentSceneDocument;
	tinyxml2::XMLError documentStatus = currentSceneDocument.Parse("<?xml version=\"1.0\"?><gameobjects></gameobjects>");
	MYTH_ASSERT(documentStatus == XML_SUCCESS);

	XMLElement* lstGameObjectsXML = nullptr;
	XMLElement* GameObjectXML = nullptr;

	XMLElement* ComponentXML = nullptr;
	std::string ComponentName = "";
	size_t indexLastOfSpace = 0Ui64;

	XMLMacro::GetElement(currentSceneDocument, &lstGameObjectsXML, "gameobjects");

	for (const std::unique_ptr<Core::GameObject>& currentObj : p_gameObjectList)
	{
		GameObjectXML = lstGameObjectsXML->InsertNewChildElement("gameobject");
		GameObjectXML->SetAttribute("name", currentObj->GetName().c_str());

		for (const std::unique_ptr<ECS::Component>& currentComponent : currentObj->GetComponents())
		{
			ComponentXML = GameObjectXML->InsertNewChildElement("component");

			//ComponentXML->SetAttribute("type", typeid(*currentComponent).hash_code());
			ComponentXML->SetAttribute("type", currentComponent->GetTypeName());
			ComponentXML->SetAttribute("enabled", currentComponent->Enabled());

			currentComponent->Save(ComponentXML);
		}
	}

	currentSceneDocument.SaveFile((GlobalScenePath + GetName() + ".xml").c_str());
}

// ----------------------------------------------

void Resources::TemporaryScene::RegisterGameObjects(std::vector<std::unique_ptr<Core::GameObject>>& p_GameObjectList)
{
	XMLDocument tempXmlDocument;
	tinyxml2::XMLError documentStatus = tempXmlDocument.Parse(m_data.c_str());
	MYTH_ASSERT(documentStatus == XML_SUCCESS);

	XMLElement* lstGameObjectsXML = nullptr;
	XMLElement* GameObjectXML = nullptr;
	XMLElement* ComponentXML = nullptr;

	if (XMLMacro::GetElement(tempXmlDocument, &lstGameObjectsXML, "gameobjects"))
		XMLMacro::GetElement(lstGameObjectsXML, &GameObjectXML, "gameobject");

	Core::GameObject* tempNewGameobject = nullptr;
	ECS::Component* tempNewComponent = nullptr;

	while (GameObjectXML != nullptr)
	{
		tempNewGameobject = new Core::GameObject(GameObjectXML->Attribute("name"));

		ComponentXML = nullptr;
		XMLMacro::GetElement(GameObjectXML, &ComponentXML, "component");

		while (ComponentXML != nullptr)
		{
			//size_t hashComponent = ComponentXML->Unsigned64Attribute("type");
			const char* nameComponent = ComponentXML->Attribute("type");
			if (BasicScene::m_sComponentLoader.find(nameComponent) != BasicScene::m_sComponentLoader.end())
			{
				BasicScene::m_sComponentLoader[nameComponent](tempNewGameobject->GetComponents());
				tempNewGameobject->GetComponents().back()->SetEnabled(ComponentXML->BoolAttribute("enabled", true));
				tempNewGameobject->GetComponents().back()->Load(ComponentXML);
				tempNewGameobject->GetComponents().back()->Load(ComponentXML, tempNewGameobject);
			}
			else
				MYTH_WARNING("BasicScene can't find component '%s'", nameComponent);

			// end Component loop
			XMLMacro::GetNextElement(&ComponentXML, "component");
		}

		// end GameObject loop
		p_GameObjectList.emplace_back(tempNewGameobject);
		XMLMacro::GetNextElement(&GameObjectXML, "gameobject");
	}
}

void Resources::TemporaryScene::SaveGameObjects(const std::vector<std::unique_ptr<Core::GameObject>>& p_gameObjectList)
{
	XMLDocument tempXmlDocument;
	tinyxml2::XMLError documentStatus = tempXmlDocument.Parse("<?xml version=\"1.0\"?><gameobjects></gameobjects>");
	MYTH_ASSERT(documentStatus == XML_SUCCESS);

	XMLElement* lstGameObjectsXML = nullptr;
	XMLElement* GameObjectXML = nullptr;

	XMLElement* ComponentXML = nullptr;
	std::string ComponentName = "";
	size_t indexLastOfSpace = 0Ui64;

	XMLMacro::GetElement(tempXmlDocument, &lstGameObjectsXML, "gameobjects");

	for (const std::unique_ptr<Core::GameObject>& currentObj : p_gameObjectList)
	{
		GameObjectXML = lstGameObjectsXML->InsertNewChildElement("gameobject");
		GameObjectXML->SetAttribute("name", currentObj->GetName().c_str());

		for (const std::unique_ptr<ECS::Component>& currentComponent : currentObj->GetComponents())
		{
			ComponentXML = GameObjectXML->InsertNewChildElement("component");

			//ComponentXML->SetAttribute("type", typeid(*currentComponent).hash_code());
			ComponentXML->SetAttribute("type", currentComponent->GetTypeName());
			ComponentXML->SetAttribute("enabled", currentComponent->Enabled());

			currentComponent->Save(ComponentXML);
		}
	}

	tinyxml2::XMLPrinter printerToStr;
	tempXmlDocument.Accept(&printerToStr);
	m_data = printerToStr.CStr();
}
#endif // MODE_EDITOR