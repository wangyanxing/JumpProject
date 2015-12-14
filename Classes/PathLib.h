
#ifndef PathLib_h__
#define PathLib_h__

#include "Defines.h"

#include <string>
#include <vector>

class DiPathLib
{
public:

  static std::vector<std::string> StringSplit(const std::string &source, const char *delimiter = " ", bool keepEmpty = false);

  static void ReplaceString(std::string& subject, const std::string& search,
                            const std::string& replace);

  static bool EndsWith(const std::string& a, const std::string& b);

  /** we need a absolute path
   */
  static bool FileExisted(const std::string& file);

  enum FileDlgFlag
  {
    MULTIPLE_SELECTION = 0x01
  };

  /** Reset the current directory to the binary directory
   */
  static void ResetCurrentDir();

#if EDITOR_MODE

  static bool OpenDirectoryDialog(const void* wndHandle, const std::string& title, const std::string& defaultPath, std::string& outFolderName);

  static bool OpenFileDialog(const void* wndHandle, const std::string& title, const std::string& defaultPath, const std::string& defaultFile,
                             const std::string& fileTypes, unsigned int flags, std::vector<std::string>& outFiles);

  static bool SaveFileDialog(const void* wndHandle, const std::string& dialogTitle, const std::string& defaultPath,
                             const std::string& defaultFile, const std::string& fileTypes, unsigned int flags, std::vector<std::string>& outFilenames);

  static std::string msAppFile;

#endif
};

#endif // PathLib_h__