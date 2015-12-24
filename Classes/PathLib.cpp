#include "PathLib.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#   include <io.h>
#   include <windows.h>
#   include <ShlObj.h>

#else
#   include <sys/types.h>
#   include <dirent.h>
#   include <unistd.h>
#endif

#if EDITOR_MODE

std::string PathLib::msAppFile;

void PathLib::openInSystem(const char *file) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
  system(file);
#else
  std::string cmd = "open \"";
  cmd += file;
  cmd += "\"";
  system(cmd.c_str());
#endif
}

#endif

bool PathLib::endsWith(const std::string &a, const std::string &b) {
  if (b.size() > a.size()) {
    return false;
  }
  return std::equal(a.begin() + a.size() - b.size(), a.end(), b.begin());
}

void PathLib::replaceString(std::string &subject,
                            const std::string &search,
                            const std::string &replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

std::vector<std::string> PathLib::stringSplit(const std::string &source, const char *delimiter,
                                              bool keepEmpty) {
  std::vector<std::string> results;

  size_t prev = 0;
  size_t next = 0;

  while ((next = source.find_first_of(delimiter, prev)) != std::string::npos) {
    if (keepEmpty || (next - prev != 0)) {
      results.push_back(source.substr(prev, next - prev));
    }
    prev = next + 1;
  }

  if (prev < source.size()) {
    results.push_back(source.substr(prev));
  }

  return results;
}

std::string GetFileExtension(const std::string &strin) {
  const char *str = strin.c_str();
  const char *ext = strrchr(str, '.');
  if (ext) {
    ext++;
    return std::string(ext);
  }
  return std::string("");
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

const std::string &_GetAppFileName() {
  static std::string appFile;

  char moduleFile[MAX_PATH];
  GetModuleFileNameA(0, moduleFile, MAX_PATH);
  appFile = moduleFile;

  return appFile;
}

#define _SLASH "\\"

static bool FileDialogShared(bool bSave,
                             const void *wndHandle,
                             const std::string &dialogTitle,
                             const std::string &defaultPath,
                             const std::string &defaultFile,
                             const std::string &fileTypes,
                             unsigned int flags, std::vector<std::string> &outFiles) {
  std::string defFile = defaultFile;
  std::string defPath = defaultPath;

  const int MAX_FILENAME_STR = 65536;
  const int MAX_FILETYPES_STR = 4096;

  char Filename[MAX_FILENAME_STR];
  strcpy(Filename, defFile.c_str());

  char Pathname[MAX_FILENAME_STR];
  strcpy(Pathname, defPath.c_str());

  char FileTypeStr[MAX_FILETYPES_STR];
  char *FileTypesPtr = NULL;
  int FileTypesLen = fileTypes.length();

  std::vector<std::string> CleanExtensionList;

  std::vector<std::string> UnformattedExtensions = PathLib::stringSplit(fileTypes, "|");
  for (size_t ExtensionIndex = 1;
       ExtensionIndex < UnformattedExtensions.size(); ExtensionIndex += 2) {
    const std::string &Extension = UnformattedExtensions[ExtensionIndex];
    if (Extension != "*.*") {
      auto WildCardIndex = Extension.find("*");
      CleanExtensionList.push_back(
          WildCardIndex != std::string::npos ? GetFileExtension(Extension) : Extension);
    }
  }

  if (FileTypesLen > 0 && FileTypesLen - 1 < MAX_FILETYPES_STR) {
    FileTypesPtr = FileTypeStr;
    strcpy(FileTypeStr, fileTypes.c_str());

    char *Pos = FileTypeStr;
    while (Pos[0] != 0) {
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
  ofn.hwndOwner = (HWND) wndHandle;
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

  if (flags & PathLib::MULTIPLE_SELECTION)
    ofn.Flags |= OFN_ALLOWMULTISELECT;

  bool bSuccess;
  if (bSave) {
    bSuccess = !!::GetSaveFileName(&ofn);
  } else {
    bSuccess = !!::GetOpenFileName(&ofn);
  }

  if (bSuccess) {
    PathLib::resetCurrentDir();
    if (flags & PathLib::MULTIPLE_SELECTION) {
      // When selecting multiple files, the returned string is a NULL delimited list
      // where the first element is the directory and all remaining elements are filenames.
      // There is an extra NULL character to indicate the end of the list.
      std::string DirectoryOrSingleFileName = Filename;
      CHAR *Pos = Filename + DirectoryOrSingleFileName.length() + 1;

      outFiles.clear();
      if (Pos[0] == 0) {
        // One item selected. There was an extra trailing NULL character.
        outFiles.push_back(DirectoryOrSingleFileName);
      } else {
        // Multiple items selected. Keep adding filenames until two NULL characters.
        std::string SelectedFile;
        do {
          SelectedFile = std::string(Pos);
          outFiles.push_back(SelectedFile);
          Pos += SelectedFile.length() + 1;
        } while (Pos[0] != 0);
      }
    } else {
      outFiles.clear();
      outFiles.push_back(Filename);
    }
  } else {
    unsigned int error = ::CommDlgExtendedError();
    if (error != ERROR_SUCCESS) {
      CCLOG("Cannot get the results from open file dialog");
    }
  }
  return bSuccess;
}

static ::INT CALLBACK
OpenDirCallback(HWND
hwnd,
::UINT uMsg, LPARAM
lParam,
LPARAM lpData
) {
// Set the path to the start path upon initialization.
switch (uMsg) {
case
BFFM_INITIALIZED:
if (lpData) {
SendMessage(hwnd, BFFM_SETSELECTION,
true, lpData);
}
break;
}
return 0;
}

bool PathLib::openDirectoryDialog(const void *wndHandle,
                                  const std::string &title,
                                  const std::string &defaultPath,
                                  std::string &outFolderName) {
  BROWSEINFO bi;
  ZeroMemory(&bi, sizeof(BROWSEINFO));

  char FolderName[MAX_PATH];
  ZeroMemory(FolderName, sizeof(char) * MAX_PATH);

  std::string PathToSelect = defaultPath;

  bi.hwndOwner = (HWND)
  wndHandle;
  bi.pszDisplayName = FolderName;
  bi.lpszTitle = title.c_str();
  bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_SHAREABLE;
  bi.lpfn = OpenDirCallback;
  bi.lParam = (LPARAM)(PathToSelect.c_str());
  bool bSuccess = false;
  LPCITEMIDLIST Folder = ::SHBrowseForFolderA(&bi);
  if (Folder) {
    bSuccess = (::SHGetPathFromIDListA(Folder, FolderName) ? true : false);
    if (bSuccess) {
      outFolderName = FolderName;
      //FPaths::NormalizeFilename(OutFolderName);
    } else {
      CCLOG("Failed to launch the open directory dialog");
    }
  } else {
    CCLOG("Failed to launch the open directory dialog");
  }
  return bSuccess;
}

void PathLib::resetCurrentDir() {
}

bool PathLib::openFileDialog(const void *wndHandle,
                             const std::string &title,
                             const std::string &defaultPath,
                             const std::string &defaultFile,
                             const std::string &fileTypes,
                             unsigned int flags,
                             std::vector<std::string> &outFiles) {
  return FileDialogShared(false, wndHandle, title, defaultPath, defaultFile, fileTypes, flags,
                          outFiles);
}

bool PathLib::saveFileDialog(const void *wndHandle,
                             const std::string &title,
                             const std::string &defaultPath,
                             const std::string &defaultFile,
                             const std::string &fileTypes,
                             unsigned int flags,
                             std::vector<std::string> &outFiles) {
  return FileDialogShared(true, wndHandle, title, defaultPath, defaultFile, fileTypes, flags,
                          outFiles);
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#   include <mach-o/dyld.h>

const std::string& _GetAppFileName() {
  static std::string ret;
  unsigned int pathNameSize = 1024;
  char pathName[1024];

  if (!_NSGetExecutablePath(pathName, &pathNameSize)) {
    char real[PATH_MAX];
    if (realpath(pathName, real) != NULL) {
      pathNameSize = strlen(real);
      strncpy(pathName, real, pathNameSize);
    }
    ret = pathName;
  }
  return ret;
}

#define _SLASH "/"

#endif

bool PathLib::fileExisted(const std::string &file) {
  return (access(file.c_str(), 0) != -1);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

std::vector<std::string> PathLib::listFiles(const char *path, const char *ext) {
  std::vector<std::string> ret;
  struct _finddata_t dirFile;
  long hFile;
  if ((hFile = _findfirst(path, &dirFile)) != -1) {
    do {
      if (!strcmp(dirFile.name, ".")) {
        continue;
      }
      if (!strcmp(dirFile.name, "..")) {
        continue;
      }
      if (dirFile.attrib & _A_HIDDEN) {
        continue;
      }
      if (dirFile.name[0] == '.') {
        continue;
      }

      // dirFile.name is the name of the file. Do whatever string comparison
      // you want here. Something like:
      if (strstr(dirFile.name, ext)) {
        ret.push_back(dirFile.name);
      }

    } while (_findnext(hFile, &dirFile) == 0);
    _findclose(hFile);
  }
  return ret;
}

#else
std::vector<std::string> PathLib::listFiles(const char* path, const char* ext) {
  std::vector<std::string> ret;
  DIR* dirFile = opendir( path );
  if ( dirFile ) {
    struct dirent* hFile;
    errno = 0;
    while (( hFile = readdir( dirFile )) != NULL ) {
      if ( !strcmp( hFile->d_name, "."  )) {
        continue;
      }
      if ( !strcmp( hFile->d_name, ".." )) {
        continue;
      }

      // in linux hidden files all start with '.'
      if ( hFile->d_name[0] == '.' ) {
        continue;
      }

      // dirFile.name is the name of the file. Do whatever string comparison
      // you want here. Something like:
      if ( strstr( hFile->d_name, ext ) ) {
        std::string dir;
        ret.push_back(hFile->d_name);
      }
    }
    closedir(dirFile);
  }
  return ret;
}
#endif
