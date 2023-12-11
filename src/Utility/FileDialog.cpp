#include <shlobj.h>

#include "Utility/FileDialog.h"
#include "Utility/StringUtility.h"

std::string FileDialog::OpenFolder(HWND owner)
{
    std::string folderPath;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

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

        CoUninitialize();
    }

    return folderPath;
}
