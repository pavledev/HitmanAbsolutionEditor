#include "FSB/IO/WAVWriter.h"

namespace FSB
{
    namespace IO
    {
        WAVHeader WAVWriter::GenerateWAVHeader(const FSB::Sample& sample)
        {
            WAVHeader wavHeader;
            IMAADPCMWAVEFORMAT imaAdpcmWaveFormat;

            memcpy_s(wavHeader.chunkID, 4, "RIFF", 4);

            wavHeader.chunkSize = sample.size + sizeof(WAVHeader) - 8;

            memcpy_s(wavHeader.format, 4, "WAVE", 4);
            memcpy_s(wavHeader.subChunk1ID, 4, "fmt ", 4);

            imaAdpcmWaveFormat.waveFormatEX.wBitsPerSample = 4;
            imaAdpcmWaveFormat.waveFormatEX.wFormatTag = 0x11;
            imaAdpcmWaveFormat.waveFormatEX.nChannels = sample.channels;
            //imaAdpcmWaveFormat.waveFormatEX.nBlockAlign = sample.channels * 256;
            imaAdpcmWaveFormat.waveFormatEX.nBlockAlign = sample.channels * 36;
            imaAdpcmWaveFormat.wSamplesPerBlock = (imaAdpcmWaveFormat.waveFormatEX.nBlockAlign - 4 * sample.channels) * 8 /
                (imaAdpcmWaveFormat.waveFormatEX.wBitsPerSample * sample.channels) + 1;
            imaAdpcmWaveFormat.waveFormatEX.nSamplesPerSec = sample.frequency;
            imaAdpcmWaveFormat.waveFormatEX.nAvgBytesPerSec = imaAdpcmWaveFormat.waveFormatEX.nBlockAlign *
                imaAdpcmWaveFormat.waveFormatEX.nSamplesPerSec / imaAdpcmWaveFormat.wSamplesPerBlock;

            wavHeader.subChunk1Size = sizeof(wavHeader.audioFormat) + sizeof(wavHeader.numChannels) + sizeof(wavHeader.sampleRate) +
                sizeof(wavHeader.byteRate) + sizeof(wavHeader.blockAlign) + sizeof(wavHeader.bitsPerSample) +
                sizeof(wavHeader.byteExtraData) + sizeof(wavHeader.extraData);
            wavHeader.audioFormat = imaAdpcmWaveFormat.waveFormatEX.wFormatTag;
            wavHeader.numChannels = sample.channels;
            wavHeader.sampleRate = imaAdpcmWaveFormat.waveFormatEX.nSamplesPerSec;
            wavHeader.byteRate = imaAdpcmWaveFormat.waveFormatEX.nAvgBytesPerSec;
            wavHeader.blockAlign = imaAdpcmWaveFormat.waveFormatEX.nBlockAlign;
            wavHeader.bitsPerSample = imaAdpcmWaveFormat.waveFormatEX.wBitsPerSample;
            wavHeader.byteExtraData = sizeof(IMAADPCMWAVEFORMAT) - sizeof(WAVEFORMATEX) - 2; //2 is for padding
            wavHeader.extraData = imaAdpcmWaveFormat.wSamplesPerBlock;

            memcpy_s(wavHeader.subChunk2ID, 4, "fact", 4);

            wavHeader.subChunk2Size = 4;
            wavHeader.numOfSamples = sample.samples;

            memcpy_s(wavHeader.subChunk3ID, 4, "data", 4);

            wavHeader.subChunk3Size = sample.size; //Or file size - 60

            return wavHeader;
        }

        void WAVWriter::WriteToFile(const FSB::Sample& sample, char* data, BinaryWriter& binaryWriter)
        {
            WAVHeader wavHeader = GenerateWAVHeader(sample);

            binaryWriter.Write(wavHeader);
            binaryWriter.Write(data, sample.size);
        }
    }
}
