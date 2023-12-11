#include <sstream>
#include <iomanip>

#include "UI/Panels/HexViewerPanel.h"
#include "Editor.h"

HexViewerPanel::HexViewerPanel(const char* name, const char* icon, const bool showResourceData) : BasePanel(name, icon)
{
    this->showResourceData = showResourceData;
    resource = nullptr;
}

void HexViewerPanel::Render()
{
    Begin();

    if (!resource->GetResourceData())
    {
        ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
        ImGui::Text("Loading data...");
        End();

        return;
    }

    if (offsets.empty())
    {
        if (showResourceData)
        {
            GetHexBytes(static_cast<const unsigned char*>(resource->GetResourceData()), resource->GetResourceDataSize());
        }
        else
        {
            GetHexBytes(static_cast<const unsigned char*>(resource->GetHeaderData()), resource->GetHeaderDataSize());
        }
    }

    ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetConsolasRegularFont());

    ImGui::InputTextMultiline("##offsets", offsets.data(), offsets.size(), ImVec2(100, ImGui::GetWindowHeight() - 50));
    ImGui::SameLine();
    ImGui::InputTextMultiline("##bytes", bytes.data(), bytes.size(), ImVec2(500, ImGui::GetWindowHeight() - 50));
    ImGui::SameLine();
    ImGui::InputTextMultiline("##decodedText", decodedText.data(), decodedText.size(), ImVec2(200, ImGui::GetWindowHeight() - 50));

    ImGui::PopFont();

    End();
}

void HexViewerPanel::SetResource(std::shared_ptr<Resource> resource)
{
    this->resource = resource;
}

void HexViewerPanel::GetHexBytes(const unsigned char* data, const size_t dataSize)
{
    std::stringstream stringStream;
    std::stringstream stringStream2;
    std::stringstream stringStream3;

    stringStream << "Offset  \n\n";
    stringStream2 << "00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n\n";
    stringStream3 << "Decoded Text\n\n";

    for (size_t i = 0; i < dataSize; i += 16)
    {
        stringStream << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << i << std::endl;

        for (int j = 0; j < 16; ++j)
        {
            if (i + j == dataSize)
            {
                break;
            }

            stringStream2 << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(data[i + j]) << " ";
        }

        stringStream2 << std::endl;

        for (int j = 0; j < 16; ++j)
        {
            if (i + j < dataSize)
            {
                if (std::isprint(data[i + j]))
                {
                    stringStream3 << static_cast<char>(data[i + j]);
                }
                else
                {
                    stringStream3 << ".";
                }
            }
        }

        stringStream3 << std::endl;
    }

    offsets = stringStream.str();
    bytes = stringStream2.str();
    decodedText = stringStream3.str();
}
