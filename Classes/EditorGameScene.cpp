//
//  EditorGameScene.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#include "EditorGameScene.h"
#include "GameLevel.h"

#if USE_REFACTOR

EditorGameScene::EditorGameScene() {

}

EditorGameScene::~EditorGameScene() {
  
}

bool EditorGameScene::init() {
  GameLayerContainer::init();
  GameLevel::instance().setGameLayer(this);

  GameLevel::instance().load("maps/local/test_refactor1.json");

  return true;
}

void EditorGameScene::onEnter() {
  GameLayerContainer::onEnter();
}

void EditorGameScene::update(float dt) {
  GameLayerContainer::update(dt);
}

void EditorGameScene::postUpdate(float dt) {
  GameLayerContainer::postUpdate(dt);
}

void EditorGameScene::clean() {
  GameLayerContainer::clean();
}

void EditorGameScene::onWinGame() {
  
}

#endif
