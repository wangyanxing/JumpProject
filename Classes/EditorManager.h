//
//  EditorManager.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/3/16.
//
//

#ifndef EditorManager_h
#define EditorManager_h

#include "Prerequisites.h"
#include "Singleton.h"

class EditorManager {
public:
  DECL_SIMPLE_SINGLETON(EditorManager);

  void init();

  cocos2d::Node *getEditorRootNode() {
    return mEditorRoot;
  }

  cocos2d::DrawNode *getGridNode() {
    return mGridNode;
  }

  void afterLoad();

  void loadLastEdit();

private:
  void registerInputs();

  void openMapFile();

  void initHelpers();

  void toggleHelpersVisible();

private:
  cocos2d::Node *mEditorRoot{nullptr};

  cocos2d::DrawNode *mGridNode{nullptr};
};

#endif /* EditorManager_h */
