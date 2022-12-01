#include "Core/System/EventManager.h"

SINGLETON_SETUP(Core::System::EventManager)

Core::System::EventManager::EventManager()
{
	MYTH_ASSERT(singleton == nullptr);
	singleton = this;
}

std::shared_ptr<stypox::EventNotifier> Core::System::EventManager::CreateNewEvent()
{
	singleton->m_eventNotifiers.push_back(std::make_shared<stypox::EventNotifier>());

	return singleton->m_eventNotifiers.back();
}

void Core::System::EventManager::ClearUnusedEvent()
{
	for (unsigned int i = 0; i < m_eventNotifiers.size(); i++)
	{
		if (m_eventNotifiers[i].use_count() > 1)
			continue;

		m_eventNotifiers.erase(m_eventNotifiers.begin() + i);
		i--;
	}
}
