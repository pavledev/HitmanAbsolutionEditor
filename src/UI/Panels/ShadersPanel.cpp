#include "UI/Panels/ShadersPanel.h"
#include "HLSL/HLSLDecompiler.h"
#include "Editor.h"

ShadersPanel::ShadersPanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialEffect> renderMaterialEffectResource) : BasePanel(name, icon)
{
	this->renderMaterialEffectResource = renderMaterialEffectResource;

	textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
	textEditor.SetTabSize(4);
	textEditor.SetShowWhitespaces(false);
}

void ShadersPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

	if (!renderMaterialEffectResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading render material effect...");

		ImGui::PopFont();

		End();

		return;
	}

	std::vector<RenderMaterialEffect::Shader>& shaders = renderMaterialEffectResource->GetShaders();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Select shader to decompile:");

	ImGui::PushItemWidth(-1);

	if (ImGui::BeginListBox("##Shaders"))
	{
		for (int i = 0; i < shaders.size(); ++i)
		{
			const bool isSelected = selectedShaderIndex == i;

			ImGui::PushID(&shaders[i]);

			if (ImGui::Selectable(shaders[i].name.c_str(), isSelected))
			{
				selectedShaderIndex = i;

				if (shaders[i].hlslCode.empty())
				{
					std::string model;

					Decompile(shaders[i].byteCode, shaders[i].byteCodeLength, &shaders[i].hlslCode, &model);
				}

				textEditor.SetText(shaders[i].hlslCode);
			}

			ImGui::PopID();

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndListBox();
	}

	ImGui::PopItemWidth();
	ImGui::Spacing();
	ImGui::Spacing();

	textEditor.Render("HLSL Editor");

	ImGui::PopFont();

	End();
}
