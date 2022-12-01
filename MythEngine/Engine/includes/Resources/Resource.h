#pragma once

#include "define.h"

#include "tinyxml2.h"

#include <string>

#define REGISTER_RESOURCE_HPP() static const char* TypeName; \
	virtual const char* GetTypeName() const override { return TypeName; }
#define REGISTER_RESOURCE_CPP(TheResource) const char* TheResource::TypeName = #TheResource;

namespace Resources
{
	class Resource
	{
	public:
		Resource(const std::string& p_name) : m_name(p_name), m_loaded(false) {}
		virtual ~Resource() {}

		virtual const char* GetTypeName() const = 0;

		const std::string& GetName() const { return m_name; }
		bool IsLoaded() const { return m_loaded; }

		virtual void LoadResource() { m_loaded = true; }
		virtual void UnloadResource() { m_loaded = false; }
	//	virtual void Load(tinyxml2::XMLElement* GameObjectXml) {}
#ifdef MODE_EDITOR
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
		virtual void ShowEditorControl() {}
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
	//	virtual void Save(tinyxml2::XMLElement* GameObjectXml) {}
#endif // MODE_EDITOR

	protected:
		bool m_loaded;

	private:
		std::string m_name;
	};
}