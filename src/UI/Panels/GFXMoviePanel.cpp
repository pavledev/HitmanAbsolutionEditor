#include <DDSTextureLoader.h>

#include "UI/Panels/GFXMoviePanel.h"
#include "Editor.h"

GFXMoviePanel::Texture::~Texture()
{
	D3D11Utility::Release(textureView);
	D3D11Utility::Release(texture);
}

GFXMoviePanel::GFXMoviePanel(const char* name, const char* icon, std::shared_ptr<GFXMovie> gfxMovieResource) : BasePanel(name, icon)
{
	this->gfxMovieResource = gfxMovieResource;
	selectedFileNameIndex = 0;
}

void GFXMoviePanel::Render()
{
	if (!Begin())
	{
		return;
	}

	std::shared_ptr<SGfxResource> gfxResource = gfxMovieResource->GetGfxResource();

	if (!gfxResource)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading gfx movie...");
		End();

		return;
	}

	ImGui::PushItemWidth(-1);

	if (ImGui::BeginListBox("##FileNames"))
	{
		for (int i = 0; i < gfxResource->filenames.Size(); ++i)
		{
			const bool isSelected = selectedFileNameIndex == i;

			if (ImGui::Selectable(gfxResource->filenames[i].ToCString(), isSelected))
			{
				selectedFileNameIndex = i;

				if (!ddsTextures[i]->texture)
				{
					LoadDDSTextureFromMemory();
				}

				selectedDDSTextureCallback(ddsTextures[i]->textureView, ddsTextures[i]->width, ddsTextures[i]->height);
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndListBox();
	}

	ImGui::PopItemWidth();

	End();
}

void GFXMoviePanel::OnResourceLoaded()
{
	gfxMovieResource->Deserialize();

	const size_t fileNameCount = gfxMovieResource->GetGfxResource()->filenames.Size();

	ddsTextures.reserve(fileNameCount);

	for (size_t i = 0; i < fileNameCount; ++i)
	{
		ddsTextures.push_back(std::make_shared<Texture>());
	}

	LoadDDSTextureFromMemory();
}

void GFXMoviePanel::SetSelectedDDSTextureCallback(SelectedDDSTextureCallback selectedDDSTextureCallback)
{
	this->selectedDDSTextureCallback = selectedDDSTextureCallback;
}

void GFXMoviePanel::LoadDDSTextureFromMemory()
{
	const std::shared_ptr<SGfxResource> gfxResource = gfxMovieResource->GetGfxResource();
	const unsigned char* ddsData = gfxResource->fileContents[selectedFileNameIndex].GetStart();
	const size_t ddsDataSize = gfxResource->fileContents[selectedFileNameIndex].Size();

	ID3D11Resource*& texture = ddsTextures[selectedFileNameIndex]->texture;
	ID3D11ShaderResourceView*& textureView = ddsTextures[selectedFileNameIndex]->textureView;

	DirectX::CreateDDSTextureFromMemory(Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), ddsData, ddsDataSize, &texture, &textureView);

	ID3D11Texture2D* texture2D = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc;

	texture->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));
	texture2D->GetDesc(&textureDesc);

	ddsTextures[selectedFileNameIndex]->width = textureDesc.Width;
	ddsTextures[selectedFileNameIndex]->height = textureDesc.Height;

	texture2D->Release();
}
