#pragma once

#include "Resource.h"
#include "FSB/FSB.h"

class WaveBankFSB : public Resource
{
public:
	struct AudioSample
	{
		~AudioSample();

		std::string name;
		void* data;
		unsigned int dataSize;
	};

	const std::vector<std::shared_ptr<AudioSample>>& GetAudioSamples() const;
	std::vector<std::shared_ptr<AudioSample>>& GetAudioSamples();
	virtual FSB::Format GetFormat() = 0;
	virtual void GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size) = 0;
	void ConvertFSB5ToOGG();
	void ConvertFSB5ToWAV();

private:
	std::vector<std::shared_ptr<AudioSample>> audioSamples;
};
