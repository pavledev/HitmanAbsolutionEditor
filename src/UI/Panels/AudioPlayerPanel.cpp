#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/AudioPlayerPanel.h"
#include "Resources/WaveBankFSBS.h"

AudioPlayerPanel::AudioPlayerPanel(const char* name, const char* icon, std::shared_ptr<WaveBankFSB> waveBankFSBResource) : BasePanel(name, icon)
{
	this->waveBankFSBResource = waveBankFSBResource;
	volume = 30.f;
	selectedAudioSampleIndex = 0;
}

AudioPlayerPanel::~AudioPlayerPanel()
{
	if (sound->getStatus() == sf::Sound::Status::Playing)
	{
		sound->stop();
	}
}

void AudioPlayerPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (soundBuffers.size() == 0)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading FSB...");
		End();

		return;
	}

	const std::vector<std::shared_ptr<WaveBankFSB::AudioSample>>& audioSamples = waveBankFSBResource->GetAudioSamples();

	ImGui::PushItemWidth(-1);

	if (ImGui::BeginListBox("##AudioSamples"))
	{
		for (int i = 0; i < audioSamples.size();++i)
		{
			const bool isSelected = selectedAudioSampleIndex == i;

			if (ImGui::Selectable(audioSamples[i]->name.c_str(), isSelected))
			{
				selectedAudioSampleIndex = i;

				sound->setBuffer(soundBuffers[i]);
			}

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

	static bool isPaused = false;
	const char* buttonLabel = isPaused ? ICON_MDI_PAUSE : ICON_MDI_PLAY;

	if (ImGui::Button(buttonLabel))
	{
		isPaused = !isPaused;

		if (sound->getStatus() == sf::Sound::Status::Stopped)
		{
			sound->stop();
		}

		if (isPaused)
		{
			sound->play();
		}
		else
		{
			sound->pause();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(ICON_MDI_STOP))
	{
		sound->stop();
	}

	const float playingOffset = sound->getPlayingOffset().asSeconds();
	const float duration = soundBuffers[selectedAudioSampleIndex].getDuration().asSeconds();
	const int playingOffset2 = static_cast<int>(playingOffset);
	const int duration2 = static_cast<int>(duration);

	if (duration > 0 && playingOffset2 == duration2)
	{
		isPaused = false;
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetFontSize() * 2.7f);
	ImGui::LabelText("##CurrentTime", "%02d:%02d", playingOffset2 / 60, playingOffset2 % 60);
	ImGui::PopItemWidth();

	float currentTrackPosition = playingOffset;
	ImVec2 playerBarSize = ImVec2(ImGui::GetWindowContentRegionWidth() - ImGui::GetFontSize() * 20.0f, 0.0f);
	ImGui::SameLine();

	if (UI::PlayerBar("PlayerBar", &currentTrackPosition, 0.0f, duration, playerBarSize))
	{
		sound->setPlayingOffset(sf::seconds(currentTrackPosition));
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetFontSize() * 2.7f);
	ImGui::LabelText("##Duration", "%02d:%02d", duration2 / 60, duration2 % 60);
	ImGui::PopItemWidth();

	ImGui::SameLine();

	ImGui::PushItemWidth(ImGui::GetFontSize() * 1.3f);
	ImGui::LabelText("##VolumeIcon", ICON_MDI_VOLUME_HIGH);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::SliderFloat("##Volume", &volume, 0.f, 100.f, "%.0f%%"))
	{
		sound->setVolume(volume);
	}

	ImGui::PopItemWidth();

	End();
}

void AudioPlayerPanel::OnResourceLoaded()
{
	FSB::Format format = waveBankFSBResource->GetFormat();

	if (format == FSB::Format::Vorbis)
	{
		waveBankFSBResource->ConvertFSB5ToOGG();
	}
	else
	{
		waveBankFSBResource->ConvertFSB5ToWAV();
	}

	std::vector<std::shared_ptr<WaveBankFSB::AudioSample>>& audioSamples = waveBankFSBResource->GetAudioSamples();

	if (waveBankFSBResource->GetResourceHeaderHeader().m_type == 'FSBS')
	{
		std::shared_ptr<WaveBankFSBS> waveBankFSBS = std::static_pointer_cast<WaveBankFSBS>(waveBankFSBResource);
		std::vector<std::string> filePaths;

		waveBankFSBS->GetFilePaths(filePaths);

		for (size_t i = 0; i < filePaths.size(); ++i)
		{
			audioSamples[i]->name = filePaths[i].substr(filePaths[i].find_last_of("\\") + 1);
		}
	}

	soundBuffers.resize(audioSamples.size());

	for (int i = 0; i < audioSamples.size(); ++i)
	{
		soundBuffers[i].loadFromMemory(audioSamples[i]->data, audioSamples[i]->dataSize);
	}

	sound->setBuffer(soundBuffers[0]);
}
