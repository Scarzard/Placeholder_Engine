#include "Application.h"
#include "W_Console.h"
#include "ModuleEngineUI.h"
W_Console::W_Console() : Window()
{
}

W_Console::~W_Console()
{
	App->LogInConsole("Created Console Window");
}

bool W_Console::Start()
{
	return true;
}

bool W_Console::Draw()
{
	if (App->gui->console)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

		ImGui::Begin("Console");
		ImGui::Separator(); ImGui::Text(""); ImGui::Separator();

		for (int i = 0; i < App->Logs_Console.size(); ++i)
			ImGui::Text(App->Logs_Console[i].data());

		if (ScrollToBottom || (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);

		ImGui::Separator();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	return true;
}


