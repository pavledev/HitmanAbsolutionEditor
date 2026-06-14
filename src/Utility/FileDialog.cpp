#include <shlobj.h>

#include "Utility/FileDialog.h"
#include "Utility/StringUtility.h"

std::string FileDialog::OpenFolder()
{
    std::string folderPath;
    IFileOpenDialog* pFileOpen;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        DWORD dwOptions;

        if (SUCCEEDED(hr = pFileOpen->GetOptions(&dwOptions)))
        {
            hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->Show(nullptr);
        }

        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;

            hr = pFileOpen->GetResult(&pItem);

            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath;

                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr))
                {
                    folderPath = StringUtility::WideStringToAnsiString(pszFilePath);

                    CoTaskMemFree(pszFilePath);
                }

                pItem->Release();
            }
        }

        pFileOpen->Release();
    }

    return folderPath;
}

std::string FileDialog::OpenFile(const char* filters)
{
    OPENFILENAMEA ofn;
    char filePath[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filters;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(filePath);
    }
    return "";
}

std::string FileDialog::SaveFile(const char* filters)
{
    OPENFILENAMEA ofn;
    char filePath[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filters;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

    if (GetSaveFileNameA(&ofn))
    {
        return std::string(filePath);
    }
    return "";
}

void FileDialog::OpenSaveFileDialog(const char* defaultFileName, const char* defaultExtension, const char* filters, std::string& outputFilePath, std::string& extension)
{
    OPENFILENAMEA ofn;
    char filePath[MAX_PATH] = "";

    strcpy_s(filePath, MAX_PATH, defaultFileName);

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filters;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = defaultExtension;
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

    if (GetSaveFileNameA(&ofn))
    {
        outputFilePath = filePath;
        extension = outputFilePath.substr(outputFilePath.find_last_of("."));
    }
}
