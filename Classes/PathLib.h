//
//  PathLib.h
//  JumpEdt
//
//  Created by Yanxing Wang.
//
//

#ifndef PathLib_h__
#define PathLib_h__

#include "Prerequisites.h"

class PathLib {
public:
  static std::vector<std::string> stringSplit(const std::string &source,
                                              const char *delimiter = " ",
                                              bool keepEmpty = false);

  static void replaceString(std::string &subject,
                            const std::string &search,
                            const std::string &replace);

  static bool endsWith(const std::string &a,
                       const std::string &b);

  /** We need absolute path
   */
  static bool fileExisted(const std::string &file);

  static std::vector<std::string> listFiles(const char *path, const char *ext);

  enum FileDlgFlag {
    MULTIPLE_SELECTION = 0x01
  };

  /** Reset the current directory to the binary directory
   */
  static void resetCurrentDir();

  static std::string getMapDir();

#if EDITOR_MODE
  static std::string openJsonFile(const std::string &title, const std::string &path);

  static void openInSystem(const char *file);

  static bool openDirectoryDialog(const void *wndHandle,
                                  const std::string &title,
                                  const std::string &defaultPath,
                                  std::string &outFolderName);

  static bool openFileDialog(const void *wndHandle,
                             const std::string &title,
                             const std::string &defaultPath,
                             const std::string &defaultFile,
                             const std::string &fileTypes,
                             unsigned int flags,
                             std::vector<std::string> &outFiles);

  static bool saveFileDialog(const void *wndHandle,
                             const std::string &dialogTitle,
                             const std::string &defaultPath,
                             const std::string &defaultFile,
                             const std::string &fileTypes,
                             unsigned int flags,
                             std::vector<std::string> &outFilenames);

  static std::string msAppFile;
#endif
};

#endif // PathLib_h__
