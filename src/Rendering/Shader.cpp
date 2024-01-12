#include <d3d11.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>

#include "Rendering/Shader.h"
#include "Rendering/InputLayout.h"
#include "Logger.h"
#include "Utility/StringUtility.h"
#include "Utility/FileSystem.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/D3D11Utility.h"
#include "Editor.h"

Shader::~Shader()
{
    switch (shaderType)
    {
        case Shader::Type::Vertex:
            D3D11Utility::Release(static_cast<ID3D11VertexShader*>(resource));
            break;
        case Shader::Type::Pixel:
            D3D11Utility::Release(static_cast<ID3D11PixelShader*>(resource));
            break;
        case Shader::Type::Compute:
            D3D11Utility::Release(static_cast<ID3D11ComputeShader*>(resource));
            break;
    }
}

void* Shader::Compile()
{
    unsigned int compileFlags = 0;
#ifdef _DEBUG
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
    compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    std::vector<D3D_SHADER_MACRO> defines =
    {
        D3D_SHADER_MACRO{ "VS", shaderType == Type::Vertex ? "1" : "0" },
        D3D_SHADER_MACRO{ "PS", shaderType == Type::Pixel ? "1" : "0" },
        D3D_SHADER_MACRO{ "CS", shaderType == Type::Compute ? "1" : "0" },
        D3D_SHADER_MACRO{ "DEBUG", "1" }
    };

    for (const auto& define : this->defines)
    {
        defines.emplace_back(D3D_SHADER_MACRO{ define.first.c_str(), define.second.c_str() });
    }

    defines.emplace_back(D3D_SHADER_MACRO{ nullptr, nullptr });

    ID3DBlob* blobError = nullptr;
    ID3DBlob* shaderBlob = nullptr;
    HRESULT result = D3DCompile
    (
        preprocessedSource.c_str(),
        static_cast<SIZE_T>(preprocessedSource.size()),
        nullptr,
        defines.data(),
        nullptr,
        GetEntryPoint(),
        GetTargetProfile(),
        compileFlags,
        0,
        &shaderBlob,
        &blobError
    );

    if (blobError)
    {
        std::stringstream ss(static_cast<char*>(blobError->GetBufferPointer()));
        std::string line;

        while (getline(ss, line, '\n'))
        {
            const auto isError = line.find("error") != std::string::npos;
            const std::string message = std::format("{} {}", filePath, line.substr(line.find("(")));

            if (isError)
            {
                Logger::GetInstance().Log(Logger::Level::Error, message);
            }
            else
            {
                Logger::GetInstance().Log(Logger::Level::Warning, message);
            }
        }

        blobError->Release();
        blobError = nullptr;
    }

    if (FAILED(result) || !shaderBlob)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "An error occurred when trying to load and compile {}!", filePath);
    }

    void* shaderView = nullptr;

    if (shaderBlob)
    {
        HRESULT result;

        if (shaderType == Type::Vertex)
        {
            result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11VertexShader**>(&shaderView));

            if (FAILED(result))
            {
                Logger::GetInstance().Log(Logger::Level::Error, "Failed to create vertex shader!");

                return nullptr;
            }

            inputLayout->Create(vertexType, shaderBlob);
        }
        else if (shaderType == Type::Pixel)
        {
            result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11PixelShader**>(&shaderView));

            if (FAILED(result))
            {
                Logger::GetInstance().Log(Logger::Level::Error, "Failed to create pixel shader!");

                return nullptr;
            }
        }
        else if (shaderType == Type::Compute)
        {
            result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, reinterpret_cast<ID3D11ComputeShader**>(&shaderView));

            if (FAILED(result))
            {
                Logger::GetInstance().Log(Logger::Level::Error, "Failed to create compute shader!");

                return nullptr;
            }
        }

        shaderBlob->Release();
        shaderBlob = nullptr;
    }

    return shaderView;
}

void Shader::Compile(const Type shaderType, const std::string& filePath, const VertexType vertexType)
{
    this->shaderType = shaderType;
    this->vertexType = vertexType;

    if (shaderType == Type::Vertex)
    {
        inputLayout = std::make_shared<InputLayout>();
    }

    if (!std::filesystem::exists(filePath))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "{} doesn't exist!", filePath);

        return;
    }

    LoadSource(filePath);
    CompileShader();
}

void Shader::CompileShader()
{
    resource = Compile();
    compilationState = resource ? CompilationState::Succeeded : CompilationState::Failed;

    std::string prefix = (compilationState == CompilationState::Succeeded) ? "Successfully compiled" : "Failed to compile";
    std::string type = "unknown";
    type = shaderType == Type::Vertex ? "vertex" : type;
    type = shaderType == Type::Pixel ? "pixel" : type;
    type = shaderType == Type::Compute ? "compute" : type;

    std::string defines2;

    for (const auto& define : defines)
    {
        if (!defines2.empty())
        {
            defines2 += ", ";
        }

        defines2 += define.first + " = " + define.second;
    }

    if (defines2.empty())
    {
        Logger::GetInstance().Log(Logger::Level::Info, "{} {} shader {}.", prefix, type, filePath);
    }
    else
    {
        Logger::GetInstance().Log(Logger::Level::Info, "{} {} shader {} with definitions {}.", prefix, type, filePath, defines2);
    }
}

Shader::CompilationState Shader::GetCompilationState() const
{
    return compilationState;
}

bool Shader::IsCompiled() const
{
    return compilationState == CompilationState::Succeeded;
}

void Shader::LoadSource(const std::string& filePath)
{
    // Initialise a couple of things
    this->filePath = filePath;
    preprocessedSource.clear();
    names.clear();
    filePaths.clear();
    sources.clear();
    filePathsMultiple.clear();

    // Construct the source by recursively processing all include directives, starting from the actual file path.
    PreprocessIncludeDirectives(filePath);

    // Reverse the vectors so they have the main shader before the subsequent include directives.
    // This also helps with the editor's shader editor where you are interested more in the first source.
    std::reverse(names.begin(), names.end());
    std::reverse(filePaths.begin(), filePaths.end());
    std::reverse(sources.begin(), sources.end());
}

const std::vector<std::string>& Shader::GetNames() const
{
    return names;
}

const std::vector<std::string>& Shader::GetFilePaths() const
{
    return filePaths;
}

const std::vector<std::string>& Shader::GetSources() const
{
    return sources;
}

void Shader::SetSource(const unsigned int index, const std::string& source)
{
    if (index >= sources.size())
    {
        Logger::GetInstance().Log(Logger::Level::Error, "No source with index {} exists!", index);

        return;
    }

    sources[index] = source;
}

void Shader::AddDefine(const std::string& define, const std::string& value)
{
    defines[define] = value;
}

auto& Shader::GetDefines() const
{
    return defines;
}

unsigned int Shader::GetVertexSize() const
{
    return inputLayout->GetVertexSize();
}

const std::vector<Descriptor>& Shader::GetDescriptors() const
{
    return descriptors;
}

const std::shared_ptr<InputLayout>& Shader::GetInputLayout() const
{
    return inputLayout; // only valid for a vertex shader
}

const auto& Shader::GetFilePath() const
{
    return filePath;
}

const char* Shader::GetEntryPoint() const
{
    if (shaderType == Type::Vertex)  return "mainVS";
    if (shaderType == Type::Pixel)   return "mainPS";
    if (shaderType == Type::Compute) return "mainCS";

    return nullptr;
}

const char* Shader::GetTargetProfile() const
{
    if (shaderType == Type::Vertex)  return "vs_5_0";
    if (shaderType == Type::Pixel)   return "ps_5_0";
    if (shaderType == Type::Compute) return "cs_5_0";

    return nullptr;
}

void* Shader::GetResource() const
{
    return resource;
}

void Shader::PreprocessIncludeDirectives(const std::string& filePath)
{
    static std::string include_directive_prefix = "#include \"";

    // Skip already parsed include directives (avoid recursive include directives)
    if (std::find(filePathsMultiple.begin(), filePathsMultiple.end(), filePath) == filePathsMultiple.end())
    {
        filePathsMultiple.emplace_back(filePath);
    }
    else
    {
        return;
    }

    // Load source
    std::ifstream in(filePath);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string source = buffer.str();

    // Go through every line
    std::istringstream stream(source);
    std::string source_line;

    while (getline(stream, source_line))
    {
        // Add the line to the preprocessed source
        bool is_include_directive = source_line.find(include_directive_prefix) != std::string::npos;
        if (!is_include_directive)
        {
            preprocessedSource += source_line + "\n";
        }
        // If the line is an include directive, process it recursively
        else
        {
            // Construct include file
            std::string file_name = StringUtility::GetStringBetweenExpressions(source_line, include_directive_prefix, "\"");
            std::string include_file_path = FileSystem::GetDirectoryFromFilePath(filePath) + file_name;

            // Process
            PreprocessIncludeDirectives(include_file_path);
        }
    }

    // Save name
    names.emplace_back(FileSystem::GetFileNameFromFilePath(filePath));

    // Save file path
    filePaths.emplace_back(filePath);

    // Save source
    sources.emplace_back(source);
}
