#pragma once

#include <d3d11.h>

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/GFXMovie.h"

class GFXMoviePanel : public BasePanel
{
public:
	using SelectedDDSTextureCallback = std::function<void(ID3D11ShaderResourceView* textureView, const unsigned int textureWidth, const unsigned int textureHeight)>;

	GFXMoviePanel(const char* name, const char* icon, std::shared_ptr<GFXMovie> gfxMovieResource);
	void Render() override;
	void OnResourceLoaded();
	void SetSelectedDDSTextureCallback(SelectedDDSTextureCallback selectedDDSTextureCallback);

private:
	void LoadDDSTextureFromMemory();

	struct Texture
	{
		~Texture();

		ID3D11Resource* texture;
		ID3D11ShaderResourceView* textureView;
		unsigned int width;
		unsigned int height;
	};

	std::shared_ptr<GFXMovie> gfxMovieResource;
	unsigned int selectedFileNameIndex;
	std::vector<std::shared_ptr<Texture>> ddsTextures;
	SelectedDDSTextureCallback selectedDDSTextureCallback;
};
