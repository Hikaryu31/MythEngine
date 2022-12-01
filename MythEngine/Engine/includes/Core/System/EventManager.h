#pragma once

#include "define.h"

#include "Core/BaseSingleton.h"

#include <event_notifier.hpp>
#include <string>
#include <vector>

namespace Core
{
	namespace System
	{
		class EventManager : public BaseSingleton<EventManager>
		{
		public:
			EventManager();
			~EventManager() { m_eventNotifiers.clear(); singleton = nullptr; }

			static std::shared_ptr<stypox::EventNotifier> CreateNewEvent();

			void ClearUnusedEvent();

		private:
			std::vector<std::shared_ptr<stypox::EventNotifier>> m_eventNotifiers;
		};
	}
}