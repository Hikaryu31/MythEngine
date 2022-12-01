#pragma once

#include "tinyxml2.h"

namespace XMLMacro
{
	inline bool GetElement(tinyxml2::XMLDocument& src, tinyxml2::XMLElement** dst, const char* name)
	{
		*dst = src.FirstChildElement(name);
		return (*dst != nullptr);
	}

	inline bool GetElement(tinyxml2::XMLElement* src, tinyxml2::XMLElement** dst, const char* name)
	{
		*dst = src->FirstChildElement(name);
		return (*dst != nullptr);
	}

	inline bool GetNextElement(tinyxml2::XMLElement** element, const char* name)
	{
		*element = (*element)->NextSiblingElement(name);
		return (*element != nullptr);
	}
}