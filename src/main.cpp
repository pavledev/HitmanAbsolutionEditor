#include <filesystem>

#include "Editor.h"
#include "Utility/StringUtility.h"

int main()
{
    //std::filesystem::current_path(StringUtility::GetExecutableFolderPath());

    Editor& editor = Editor::GetInstance();

    if (!editor.Setup())
    {
        return 0;
    }

    editor.Start();

    return 0;
}
