#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#include "Resources/FlashMovie.h"
#include "Editor.h"

void FlashMovie::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		void* textureData = nullptr;
		unsigned int textureDataSize;
		bool isDDSTexture;

		GetTextureData(textureData, textureDataSize, isDDSTexture);

		BinaryWriter binaryWriter = BinaryWriter(outputPath);

		binaryWriter.Write(textureData, textureDataSize);
	}
}

void FlashMovie::GetTextureData(void*& textureData, unsigned int& textureDataSize, bool& isDDSTexture)
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned char type = binaryReader.Read<unsigned char>();
	const unsigned int fileOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(fileOffset + 1);

	size_t currentPosition = binaryReader.GetPosition();

	binaryReader.Seek(0, SeekOrigin::End);

	textureDataSize = binaryReader.GetPosition() - currentPosition;

	binaryReader.Seek(fileOffset + 1);

	textureData = binaryReader.GetBuffer(true);
	isDDSTexture = type == 'd';
}

void FlashMovie::CreateTextureFromMemory()
{
	void* textureData = nullptr;
	unsigned int textureDataSize;
	bool isDDSTexture;

	GetTextureData(textureData, textureDataSize, isDDSTexture);

	if (isDDSTexture)
	{
		DirectX::CreateDDSTextureFromMemory(Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), static_cast<unsigned char*>(textureData), textureDataSize, &texture, &textureView);
	}
	else
	{
		DirectX::CreateWICTextureFromMemory(Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), static_cast<unsigned char*>(textureData), textureDataSize, &texture, &textureView);
	}

	ID3D11Texture2D* texture2D = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc;

	texture->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));
	texture2D->GetDesc(&textureDesc);

	textureWidth = textureDesc.Width;
	textureHeight = textureDesc.Height;

	texture2D->Release();
}

const FlashMovie::Format FlashMovie::GetFormat() const
{
	const unsigned char type = *static_cast<unsigned char*>(resourceData);

	switch (type)
	{
		case 'd':
			return Format::DDS;
		case 'p':
			return Format::PNG;
		case 's':
			return Format::SWF;
		default:
			return Format::None;
	}
}

const unsigned int FlashMovie::GetTextureWidth() const
{
	return textureWidth;
}

const unsigned int FlashMovie::GetTextureHeight() const
{
	return textureHeight;
}

ID3D11Resource* FlashMovie::GetTexture() const
{
	return texture;
}

ID3D11ShaderResourceView* FlashMovie::GetTextureView() const
{
	return textureView;
}
