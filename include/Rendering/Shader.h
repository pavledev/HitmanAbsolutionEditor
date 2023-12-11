#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "VertexType.h"
#include "Descriptor.h"
#include "InputLayout.h"

class Shader
{
public:
    enum Type : unsigned char
    {
        Unknown = 0,
        Vertex = 1,
        Pixel = 2,
        Compute = 4,
    };

    enum class CompilationState
    {
        Succeeded,
        Failed
    };

    Shader() = default;
    ~Shader();
    void* Compile();
    void Compile(const Type shaderType, const std::string& filePath, const VertexType vertexType = VertexType::Undefined);
    void CompileShader();
    CompilationState GetCompilationState() const;
    bool IsCompiled() const;

    void LoadSource(const std::string& filePath);
    const std::vector<std::string>& GetNames() const;
    const std::vector<std::string>& GetFilePaths() const;
    const std::vector<std::string>& GetSources() const;
    void SetSource(const unsigned int index, const std::string& source);

    void AddDefine(const std::string& define, const std::string& value = "1");
    auto& GetDefines() const;

    unsigned int GetVertexSize() const;
    const std::vector<Descriptor>& GetDescriptors() const;
    const std::shared_ptr<InputLayout>& GetInputLayout() const;
    const auto& GetFilePath() const;
    const char* GetEntryPoint() const;
    const char* GetTargetProfile() const;

    void* GetResource() const;

private:
    void PreprocessIncludeDirectives(const std::string& filePath);

    std::string filePath;
    std::string preprocessedSource;
    std::vector<std::string> names;               // The names of the files from the include directives in the shader
    std::vector<std::string> filePaths;          // The file paths of the files from the include directives in the shader
    std::vector<std::string> sources;             // The source of the files from the include directives in the shader
    std::vector<std::string> filePathsMultiple; // The file paths of include directives which are defined multiple times in the shader
    std::unordered_map<std::string, std::string> defines;
    std::vector<Descriptor> descriptors;
    std::shared_ptr<InputLayout> inputLayout;
    CompilationState compilationState;
    Type shaderType;
    VertexType vertexType;
    void* resource;
};
