#pragma once

namespace FSB
{
    namespace IO
    {
        struct WAVHeader
        {
            unsigned char chunkID[4];
            unsigned int chunkSize;
            unsigned char format[4];
            unsigned char subChunk1ID[4];
            unsigned int subChunk1Size;
            unsigned short audioFormat;
            unsigned short numChannels;
            unsigned int sampleRate;
            unsigned int byteRate;
            unsigned short blockAlign;
            unsigned short bitsPerSample;
            unsigned short byteExtraData;
            unsigned short extraData;
            unsigned char subChunk2ID[4];
            unsigned int subChunk2Size;
            unsigned int numOfSamples;
            unsigned char subChunk3ID[4];
            unsigned int subChunk3Size;
        };
    }
}
