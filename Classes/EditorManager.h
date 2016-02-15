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

#if EDITOR_MODE

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

  void update(float dt);

  void afterLoad();

  void loadLastEdit();

  void onMouseDown(const MouseEvent &event);

  void onMouseUp(const MouseEvent &event);

  void onMouseMove(const MouseEvent &event);

  void onGameEnabled(bool val);

private:
  void registerInputs();

  void openMapFile();

  void saveMapFile();

  void newMapFile();

  void initHelpers();

  void toggleHelpersVisible();

  void clearSelections();

  void pathEditorMode(bool mode);

  void addPathPoint();

  void groupObjects();

  void alignObjects(const cocos2d::Vec2 &dir);

  void moveObjects(const cocos2d::Vec2 &dir);

  void movePathNode(const cocos2d::Vec2 &dir);

  void moveThings(cocos2d::EventKeyboard::KeyCode key);

  void resizeObjects(cocos2d::EventKeyboard::KeyCode key);

  void changeKindOrShadowLayer(cocos2d::EventKeyboard::KeyCode key);

  GameObject *createDefaultObject(const cocos2d::Vec2 &pos);

private:
  cocos2d::Node *mEditorRoot{nullptr};

  cocos2d::DrawNode *mGridNode{nullptr};

  cocos2d::DrawNode *mHeroSpawnNode{nullptr};

  std::vector<GameObject*> mSelections;

  bool mPathEditMode{false};
};

#endif
#endif /* EditorManager_h */
