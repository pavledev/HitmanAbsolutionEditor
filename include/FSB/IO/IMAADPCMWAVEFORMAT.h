#pragma once

#include "WAVEFORMATEX.h"

namespace FSB
{
    namespace IO
    {
        struct IMAADPCMWAVEFORMAT
        {
            WAVEFORMATEX waveFormatEX;
            unsigned short wSamplesPerBlock;
        };
    }
}
