#pragma once

#include <atomic>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/WaveBankFSB.h"

class AudioPlayerPanel : public BasePanel
{
public:
	AudioPlayerPanel(const char* name, const char* icon, std::shared_ptr<WaveBankFSB> waveBankFSBResource);
	~AudioPlayerPanel();
	void Render() override;
	void OnResourceLoaded();
	void Refresh();
	unsigned int GetSelectedAudioSampleIndex() const;

private:
	std::shared_ptr<WaveBankFSB> waveBankFSBResource;
	std::unique_ptr<sf::Sound> sound;
	std::vector<sf::SoundBuffer> soundBuffers;
	std::atomic<bool> loaded{false};
	float volume;
	unsigned int selectedAudioSampleIndex;
};
