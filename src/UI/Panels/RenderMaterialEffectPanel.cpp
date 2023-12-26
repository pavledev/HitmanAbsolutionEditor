#include "UI/Panels/RenderMaterialEffectPanel.h"

RenderMaterialEffectPanel::RenderMaterialEffectPanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialEffect> renderMaterialEffectResource) : BasePanel(name, icon)
{
	this->renderMaterialEffectResource = renderMaterialEffectResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void RenderMaterialEffectPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!renderMaterialEffectResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading render material effect...");
		End();

		return;
	}

	if (!UI::BeginProperties("MaterialEffectProperties", tableColumns))
	{
		End();

		return;
	}

	std::vector<std::string>& constantBufferNames = renderMaterialEffectResource->GetConstantBufferNames();
	std::vector<std::string>& globalVariables = renderMaterialEffectResource->GetGlobalVariables();
	std::vector<RenderMaterialEffect::Technique>& techniques = renderMaterialEffectResource->GetTechniques();

	const bool isconstantBufferNamesTreeNodeOpen = UI::BeginTreeNodeProperty("Constant Buffer Names", nullptr);

	if (isconstantBufferNamesTreeNodeOpen)
	{
		for (size_t i = 0; i < constantBufferNames.size(); ++i)
		{
			std::string index = std::to_string(i);

			UI::StringProperty(index.c_str(), constantBufferNames[i]);
		}
	}

	UI::EndTreeNodeProperty(isconstantBufferNamesTreeNodeOpen);

	const bool isGlobalVariablesTreeNodeOpen = UI::BeginTreeNodeProperty("Global Variables", nullptr);

	if (isGlobalVariablesTreeNodeOpen)
	{
		for (size_t i = 0; i < globalVariables.size(); ++i)
		{
			std::string index = std::to_string(i);

			UI::StringProperty(index.c_str(), globalVariables[i]);
		}
	}

	UI::EndTreeNodeProperty(isGlobalVariablesTreeNodeOpen);

	const bool isTechniquesTreeNodeOpen = UI::BeginTreeNodeProperty("Techniques", nullptr);

	if (isTechniquesTreeNodeOpen)
	{
		for (size_t i = 0; i < techniques.size(); ++i)
		{
			UI::StringProperty("name", techniques[i].name);

			const bool isPassesTreeNodeOpen = UI::BeginTreeNodeProperty("Passes", nullptr);

			if (isPassesTreeNodeOpen)
			{
				for (size_t j = 0; j < techniques[i].passes.size(); ++j)
				{
					std::string index = std::to_string(j);

					UI::StringProperty(index.c_str(), techniques[i].passes[j]);
				}
			}

			UI::EndTreeNodeProperty(isPassesTreeNodeOpen);

			const bool isAnnotationsTreeNodeOpen = UI::BeginTreeNodeProperty("Annotations", nullptr);

			if (isAnnotationsTreeNodeOpen)
			{
				for (size_t j = 0; j < techniques[i].annotations.size(); ++j)
				{
					std::string index = std::to_string(j);

					UI::StringProperty(index.c_str(), techniques[i].annotations[j]);
				}
			}

			UI::EndTreeNodeProperty(isAnnotationsTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isTechniquesTreeNodeOpen);

	UI::EndProperties();

	End();
}

void RenderMaterialEffectPanel::OnResourceLoaded()
{
	renderMaterialEffectResource->Deserialize();
}
