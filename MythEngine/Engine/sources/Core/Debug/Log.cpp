#include "Core/Debug/Log.h"

#include <vector>
#include <fmt/format.h>

SINGLETON_SETUP(Core::Debug::Log)

ImGuiTextBuffer tempTextBuffer = ImGuiTextBuffer();

#ifdef MODE_EDITOR
const char* LOG_LEVEL_char[] = {
	" DEBUG ",
	" INFO  ",
	"WARNING",
	" ERROR ",
	" FATAL ",
};

ImVec4 LOG_LEVEL_color[] = {
	{0.7f, 0.7f, 0.7f, 1.f},
	{1.f, 1.f, 1.f, 1.f},
	{1.f, 1.f, 0.f, 1.f},
	{1.f, 0.f, 0.f, 1.f},
	{0.6f, 0.f, 0.f, 1.f},
};
#endif // MODE_EDITOR

void Core::Debug::Log::Print(const LOG_LEVEL p_level, const bool p_writeFL, const char* p_file, const unsigned p_line)
{
	std::string formatedFileLine = "";
	if(p_writeFL)
		formatedFileLine = fmt::format("[{}][line {}]", std::filesystem::path(p_file).filename().string(), p_line);

	if (singleton->m_externalLogger != nullptr)
		singleton->m_externalLogger->Print(p_level, fmt::format("{}{}{}", formatedFileLine, (p_writeFL ? " : " : ""), m_cout.str()));

#ifdef MODE_EDITOR
	singleton->m_logList.push_back({ p_level, formatedFileLine, m_cout.str() });
	singleton->m_scrollToBottom = true;
#endif

	m_cout.str("");
}

#ifdef MODE_EDITOR
void Core::Debug::Log::PrintLog(const LOG_LEVEL p_level, const char* p_file, const unsigned p_line, const char* p_fmt, ...)
{
	MYTH_ASSERT(singleton != nullptr);

	while (singleton->m_writting.test_and_set()) {}

	va_list args;
	va_start(args, p_fmt);
	tempTextBuffer.appendfv(p_fmt, args);
	va_end(args);

	singleton->m_cout << tempTextBuffer.c_str();
	singleton->Print(p_level, true, p_file, p_line);

	tempTextBuffer.clear();

	singleton->m_writting.clear();
}

void Core::Debug::Log::ShowEditorWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::SetNextWindowSize(ImVec2(500, 400));
	ImGui::Begin("Console", p_opened);

	if (ImGui::Button("Clear")) Clear();
	ImGui::SameLine();
	if (ImGui::Button("\\/##LogScrollToBottom"))
		m_scrollToBottom = true;

	ImGui::SameLine();
	ImGui::Checkbox("Debug##Log", &(LOG_LEVEL_show[0u]));
	ImGui::SameLine();
	ImGui::Checkbox("Info##Log", &(LOG_LEVEL_show[1u]));
	ImGui::SameLine();
	ImGui::Checkbox("Warning##Log", &(LOG_LEVEL_show[2u]));
	ImGui::SameLine();
	ImGui::Checkbox("Error##Log", &(LOG_LEVEL_show[3u]));
	ImGui::SameLine();
	ImGui::Checkbox("Fatal##Log", &(LOG_LEVEL_show[4u]));

	ImGui::Separator();

	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

	for (LogItem& logToDisplay : m_logList)
		if (LOG_LEVEL_show[(unsigned int)logToDisplay.Level])
		{
			ImGui::TextColored(LOG_LEVEL_color[(unsigned int)logToDisplay.Level], "[%s]%s : %s", LOG_LEVEL_char[(unsigned int)logToDisplay.Level], logToDisplay.FileLine.c_str(), logToDisplay.Text.c_str());
			//ImGui::Text("[%s]%s : %s", LOG_LEVEL_char[(unsigned int)logToDisplay.Level], logToDisplay.FileLine.c_str(), logToDisplay.Text.c_str());
		}

	if (m_scrollToBottom)
		ImGui::SetScrollHereY(1.f);
	m_scrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}
#else
void Core::Debug::Log::PrintLog(const LOG_LEVEL p_level, const char* p_fmt, ...)
{
	MYTH_ASSERT(singleton != nullptr);

	while (singleton->m_writting.test_and_set()) {}

	va_list args;
	va_start(args, p_fmt);
	tempTextBuffer.appendfv(p_fmt, args);
	va_end(args);

	singleton->m_cout << tempTextBuffer.c_str();
	singleton->Print(p_level);

	tempTextBuffer.clear();

	singleton->m_writting.clear();
}
#endif // !MODE_EDITOR


void Core::Debug::Log::Clear()
{
#ifdef MODE_EDITOR
	m_logList.clear();
#endif
	if (singleton->m_externalLogger != nullptr)
		singleton->m_externalLogger->Clear();
}

Core::Debug::Log::Log() :
#ifdef MODE_EDITOR
	m_scrollToBottom(true),
	m_logList(),
#endif
	m_externalLogger(nullptr),
	m_cout()
{
	MYTH_ASSERT(singleton == nullptr);
	singleton = this;
}
