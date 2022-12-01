#pragma once

#include "define.h"

#define SINGLETON_SETUP(ClassSpace) ClassSpace* Core::BaseSingleton<ClassSpace>::singleton = nullptr;

namespace Core
{
	template<class T>
	class BaseSingleton
	{
	public:
		BaseSingleton() {}
		virtual ~BaseSingleton() {}

		BaseSingleton(const BaseSingleton&) = delete;
		BaseSingleton& operator=(const BaseSingleton&) = delete;
		BaseSingleton(BaseSingleton&&) = delete;
		BaseSingleton& operator=(BaseSingleton&&) = delete;

		static inline T& GetInstance() { MYTH_ASSERT(singleton != nullptr); return *singleton; }
		static inline T* GetPtrInstance() { MYTH_ASSERT(singleton != nullptr); return singleton; }

	protected:
		static T* singleton;
	};
}