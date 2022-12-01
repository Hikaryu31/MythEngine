#include "Core/System/Application.h"

#include "../ScriptManager.h"

#include <iostream>

int main()
{
    Core::System::Application newApp;

    Core::System::Application::Initialization(900, 600, "Myth engine");

    ScriptManager::LoadScript();

    while (!Core::System::Application::ShouldExit())
    {
        Core::System::Application::UpdateAndDraw();
    }

    Core::System::Application::Kill();

    return 0;
}
