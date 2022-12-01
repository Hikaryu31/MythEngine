#pragma once

#include <string>
#include <atomic>
#include <sstream>
#include <filesystem>

#include "imgui.h"

#include "define.h"

#include "Core/BaseSingleton.h"

#ifdef MODE_EDITOR
#define MYTH_DEBUG(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::DEBUG, __FILE__, __LINE__, text, __VA_ARGS__)
#define MYTH_INFO(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::INFO, __FILE__, __LINE__, text, __VA_ARGS__) 
#define MYTH_WARNING(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::WARNING, __FILE__, __LINE__, text, __VA_ARGS__)
#define MYTH_ERROR(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::mERROR, __FILE__, __LINE__, text, __VA_ARGS__) 
#define MYTH_FATAL(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::FATAL, __FILE__, __LINE__, text, __VA_ARGS__) 
#else
#define MYTH_DEBUG(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::DEBUG, text, __VA_ARGS__)
#define MYTH_INFO(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::INFO, text, __VA_ARGS__) 
#define MYTH_WARNING(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::WARNING, text, __VA_ARGS__)
#define MYTH_ERROR(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::mERROR, text, __VA_ARGS__)
#define MYTH_FATAL(text, ...) Core::Debug::Log::PrintLog(Core::Debug::LOG_LEVEL::FATAL, text, __VA_ARGS__)
#endif // !MODE_EDITOR

namespace Core
{
	namespace Debug
	{
		enum class LOG_LEVEL : unsigned int
		{
			DEBUG = 0u,
			INFO,
			WARNING,
			mERROR,
			FATAL
		};

		class ILogger
		{
		public:
			virtual void Print(const LOG_LEVEL p_level, const std::string& p_str) = 0;
			virtual void Clear() = 0;
		};

		struct LogItem
		{
			LOG_LEVEL Level;
			std::string FileLine;
			std::string Text;
		};

		class Log : public BaseSingleton<Log>
		{
		public:
			Log();
			~Log() { singleton = nullptr; }

#ifdef MODE_EDITOR
			// safe for multithreading
			static void PrintLog(const LOG_LEVEL p_level, const char* p_file, const unsigned p_line, const char* p_fmt, ...);

			void ShowEditorWindow(bool* p_opened = nullptr);
#else
			// safe for multithreading
			static void PrintLog(const LOG_LEVEL p_level, const char* p_fmt, ...);
#endif // !MODE_EDITOR

			void Clear();

			void SetLogger(ILogger* p_logger = nullptr) { m_externalLogger = p_logger; }
			void SetShowLevel(const LOG_LEVEL p_level, bool p_show = true) { LOG_LEVEL_show[(unsigned int)p_level] = p_show; }

		private:

			void Print(const LOG_LEVEL p_level, const bool p_writeFL = false, const char* p_file = "", const unsigned p_line = 0u);

			std::stringstream m_cout;
			ILogger* m_externalLogger;
#ifdef MODE_EDITOR
			std::vector<LogItem> m_logList;
			bool m_scrollToBottom;
#endif // MODE_EDITOR
			std::atomic_flag m_writting = ATOMIC_FLAG_INIT;

			bool LOG_LEVEL_show[5] = { true, true, true, true, true };
		};
	}
}