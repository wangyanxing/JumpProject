//
//  EditorGameScene.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#ifndef EditorGameScene_h
#define EditorGameScene_h

#include "Prerequisites.h"
#include "GameLayerContainer.h"

#if USE_REFACTOR

class EditorGameScene : public GameLayerContainer {
public:
  EditorGameScene();

  ~EditorGameScene();

public:
  CREATE_FUNC(EditorGameScene);

  bool init() override;

  void onEnter() override;

  void update(float dt) override;

  void postUpdate(float dt) override;

  void clean() override;

  void onWinGame() override;
};

#endif

#endif /* EditorGameScene_h */
