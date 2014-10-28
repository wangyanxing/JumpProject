
#include "PathLib.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#   include <io.h>
#   include <windows.h>
#   include <ShlObj.h>
#else
#   include <unistd.h>
#endif

std::string DiPathLib::msAppFile;

std::vector<std::string> DiPathLib::StringSplit(const std::string &source, const char *delimiter, bool keepEmpty)
{
    std::vector<std::string> results;
    
    size_t prev = 0;
    size_t next = 0;
    
    while ((next = source.find_first_of(delimiter, prev)) != std::string::npos)
    {
        if (keepEmpty || (next - prev != 0))
        {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }
    
    if (prev < source.size())
    {
        results.push_back(source.substr(prev));
    }
    
    return results;
}

std::string GetFileExtension(const std::string& strin)
{
    const char* str = strin.c_str();
    const char* ext = strrchr(str, '.');
    if (ext)
    {
        ext++;
        return std::string(ext);
    }
    return std::string("");
}


#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

    const std::string& _GetAppFileName()
    {
        static std::string appFile;

        char moduleFile[MAX_PATH];
        GetModuleFileNameA(0, moduleFile, MAX_PATH);
        appFile = moduleFile;

        return appFile;
    }

#define _SLASH "\\"

    static bool FileDialogShared(bool bSave, const void* wndHandle, const std::string& dialogTitle,
        const std::string& defaultPath, const std::string& defaultFile, const std::string& fileTypes, unsigned int flags, std::vector<std::string>& outFiles)
    {
        std::string defFile = defaultFile;
        std::string defPath = defaultPath;
//        defFile.Replace("/", "\\");
//        defPath.Replace("/", "\\");

        const int MAX_FILENAME_STR = 65536;
        const int MAX_FILETYPES_STR = 4096;

        char Filename[MAX_FILENAME_STR];
        strcpy(Filename, defFile.c_str());

        char Pathname[MAX_FILENAME_STR];
        strcpy(Pathname, defPath.c_str());

        char FileTypeStr[MAX_FILETYPES_STR];
        char* FileTypesPtr = NULL;
        int FileTypesLen = fileTypes.length();

        std::vector<std::string> CleanExtensionList;

        std::vector<std::string> UnformattedExtensions = DiPathLib::StringSplit(fileTypes,"|");
        for (size_t ExtensionIndex = 1; ExtensionIndex < UnformattedExtensions.size(); ExtensionIndex += 2)
        {
            const std::string& Extension = UnformattedExtensions[ExtensionIndex];
            if (Extension != "*.*")
            {
                auto WildCardIndex = Extension.find("*");
                CleanExtensionList.push_back(WildCardIndex != std::string::npos ? GetFileExtension(Extension) : Extension);
            }
        }

        if (FileTypesLen > 0 && FileTypesLen - 1 < MAX_FILETYPES_STR)
        {
            FileTypesPtr = FileTypeStr;
            strcpy(FileTypeStr, fileTypes.c_str());

            char* Pos = FileTypeStr;
            while (Pos[0] != 0)
            {
                if (Pos[0] == '|')
                    Pos[0] = 0;
                Pos++;
            }

            FileTypeStr[FileTypesLen] = 0;
            FileTypeStr[FileTypesLen + 1] = 0;
        }

        OPENFILENAME ofn;
        ::memset(&ofn, 0, sizeof(OPENFILENAME));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = (HWND)wndHandle;
        ofn.lpstrFilter = FileTypesPtr;
        ofn.nFilterIndex = 1;
        ofn.lpstrFile = Filename;
        ofn.nMaxFile = MAX_FILENAME_STR;
        ofn.lpstrInitialDir = Pathname;
        ofn.lpstrTitle = dialogTitle.c_str();

        if (FileTypesLen > 0)
            ofn.lpstrDefExt = &FileTypeStr[0];

        ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;

        if (bSave)
            ofn.Flags |= OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE;
        else
            ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (flags & DiPathLib::MULTIPLE_SELECTION)
            ofn.Flags |= OFN_ALLOWMULTISELECT;

        bool bSuccess;
        if (bSave)
            bSuccess = !!::GetSaveFileName(&ofn);
        else
            bSuccess = !!::GetOpenFileName(&ofn);

        if (bSuccess)
        {
            DiPathLib::ResetCurrentDir();

            if (flags & DiPathLib::MULTIPLE_SELECTION)
            {
                // When selecting multiple files, the returned string is a NULL delimited list
                // where the first element is the directory and all remaining elements are filenames.
                // There is an extra NULL character to indicate the end of the list.
                std::string DirectoryOrSingleFileName = Filename;
                CHAR* Pos = Filename + DirectoryOrSingleFileName.length() + 1;

                outFiles.clear();
                if (Pos[0] == 0)
                {
                    // One item selected. There was an extra trailing NULL character.
                    outFiles.push_back(DirectoryOrSingleFileName);
                }
                else
                {
                    // Multiple items selected. Keep adding filenames until two NULL characters.
                    std::string SelectedFile;
                    do
                    {
                        SelectedFile = std::string(Pos);
                        outFiles.push_back(SelectedFile);
                        Pos += SelectedFile.length() + 1;
                    } while (Pos[0] != 0);
                }
            }
            else
            {
                outFiles.clear();
                outFiles.push_back(Filename);
            }
        }
        else
        {
            unsigned int error = ::CommDlgExtendedError();
            if (error != ERROR_SUCCESS)
            {
                CCLOG("Cannot get the results from open file dialog");
            }
        }

        return bSuccess;
    }

    static ::INT CALLBACK OpenDirCallback(HWND hwnd, ::UINT uMsg, LPARAM lParam, LPARAM lpData)
    {
        // Set the path to the start path upon initialization.
        switch (uMsg)
        {
        case BFFM_INITIALIZED:
            if (lpData)
            {
                SendMessage(hwnd, BFFM_SETSELECTION, true, lpData);
            }
            break;
        }

        return 0;
    }

    bool DiPathLib::OpenDirectoryDialog(const void* wndHandle, const std::string& title, const std::string& defaultPath, std::string& outFolderName)
    {
        BROWSEINFO bi;
        ZeroMemory(&bi, sizeof(BROWSEINFO));

        char FolderName[MAX_PATH];
        ZeroMemory(FolderName, sizeof(char)* MAX_PATH);

        std::string PathToSelect = defaultPath;
//        PathToSelect.Replace("/", "\\");

        bi.hwndOwner = (HWND)wndHandle;
        bi.pszDisplayName = FolderName;
        bi.lpszTitle = title.c_str();
        bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_SHAREABLE;
        bi.lpfn = OpenDirCallback;
        bi.lParam = (LPARAM)(PathToSelect.c_str());
        bool bSuccess = false;
        LPCITEMIDLIST Folder = ::SHBrowseForFolderA(&bi);
        if (Folder)
        {
            bSuccess = (::SHGetPathFromIDListA(Folder, FolderName) ? true : false);
            if (bSuccess)
            {
                outFolderName = FolderName;
                //FPaths::NormalizeFilename(OutFolderName);
            }
            else
            {
                CCLOG("Failed to launch the open directory dialog");
            }
        }
        else
        {
            CCLOG("Failed to launch the open directory dialog");
        }

        return bSuccess;
    }

    void DiPathLib::ResetCurrentDir()
    {
        //std::string path = DiPathLib::GetApplicationPath();
        //::SetCurrentDirectoryA(path.c_str());
    }
    
    bool DiPathLib::OpenFileDialog(const void* wndHandle, const std::string& title,
                                   const std::string& defaultPath, const std::string& defaultFile, const std::string& fileTypes, unsigned int flags, std::vector<std::string>& outFiles)
    {
        return FileDialogShared(false, wndHandle, title, defaultPath, defaultFile, fileTypes, flags, outFiles);
    }
    
    bool DiPathLib::SaveFileDialog(const void* wndHandle, const std::string& title, const std::string& defaultPath,
                                   const std::string& defaultFile, const std::string& fileTypes, unsigned int flags, std::vector<std::string>& outFiles)
    {
        return FileDialogShared(true, wndHandle, title, defaultPath, defaultFile, fileTypes, flags, outFiles);
    }

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#   include <mach-o/dyld.h>
    
    const std::string& _GetAppFileName()
    {
        static std::string ret;
        unsigned int pathNameSize = 1024;
        char pathName[1024];
        
        if (!_NSGetExecutablePath(pathName, &pathNameSize))
        {
            char real[PATH_MAX];
            
            if (realpath(pathName, real) != NULL)
            {
                pathNameSize = strlen(real);
                strncpy(pathName, real, pathNameSize);
            }
            
            ret = pathName;
        }
        
        return ret;
    }

#define _SLASH "/"
    
#endif
    bool DiPathLib::FileExisted(const std::string& file)
    {
        return (access(file.c_str(), 0) != -1);
    }