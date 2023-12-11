#pragma once

#include "GLTFSDK/IStreamWriter.h"

class GLTFStreamWriter : public Microsoft::glTF::IStreamWriter
{
public:
    GLTFStreamWriter(const std::string& outputDirectory) : outputDirectory(outputDirectory)
    {
    }

    std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const override
    {
        std::string outputPath = outputDirectory + "/" + filename;
        std::shared_ptr<std::ofstream> stream = std::make_shared<std::ofstream>(outputPath, std::ios::binary);
        std::shared_ptr<std::ostream> baseStream = std::static_pointer_cast<std::ostream>(stream);

        return baseStream;
    }

private:
    std::string outputDirectory;
};
