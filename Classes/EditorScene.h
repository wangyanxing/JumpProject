#ifndef __JumpEdt__EditorScene__
#define __JumpEdt__EditorScene__

#include "cocos2d.h"
#include "DrawNodeEx.h"
#include "Defines.h"
#include "Path.h"
#include "Button.h"
#include "Blocks.h"
#include "LogicManager.h"
#include "ShaderLayer.h"

#if EDITOR_MODE

class BlockBase;
class ShadowManager;
class Hero;
class GameLogic;

#if USE_SHADER_LAYER
class EditorScene : public ShaderLayer {
#else
class EditorScene : public cocos2d::Layer {
#endif
public:
  EditorScene() = default;

  virtual ~EditorScene();

  struct PostUpdater {
    void update(float dt) {
      GameLogic::Game->postUpdate(dt);
    }
  };

  static EditorScene* Scene;

  virtual bool init();

  void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

  void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

  void mouseDown(cocos2d::Event* event);

  void mouseUp(cocos2d::Event* event);

  void mouseMove(cocos2d::Event* event);

  void convertMouse(cocos2d::Point& pt);

  bool onContactPreSolve(cocos2d::PhysicsContact& contact, cocos2d::PhysicsContactPreSolve& solve);

  CREATE_FUNC(EditorScene);

public:

  void initDrawNodes();

  void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);

  void onDrawPrimitive(const cocos2d::Mat4 &transform, uint32_t flags);

  void duplicate();

  void update(float dt);

  void updateCamera();

  void enableGame(bool val, bool force = false);

  void setKind(int kind);

  void setShadowLayer(int layer);

  void alignLeft();

  void alignRight();

  void alignUp();

  void alignDown();

  void clean(bool save);

  void group();

  void showDieFullScreenAnim();

  bool mPressingShift{ false };

  bool mPressingCtrl{ false };

  bool mPressingAlt{ false };

  bool mPressingM{ false };

  bool mPressingN{ false };

  bool mPressingB{ false };

  bool mPressingV{ false };

  BlockBase* mMovingBlock{ nullptr };

  cocos2d::Point mLastPoint;

  cocos2d::Vec2 mLastCursorInView;

  cocos2d::Sprite* mSpawnPoint{ nullptr };

  cocos2d::Sprite* mLightPoint{ nullptr };

  cocos2d::Sprite* mGradientCenterPoint{ nullptr };

  std::set<BlockBase*> mSelections;

  BlockBase* mSelectionHead{ nullptr };

  bool mPathMode{ false };

  bool mShowGrid{ false };

  bool mMovingCamera{ false };

  std::string mCurFileName;
  
  PostUpdater mPostUpdater;
  
  GameLogic* mGame{ nullptr };

  cocos2d::DrawNode* mGridNode{ nullptr };

  cocos2d::DrawNode* mGroupNode{ nullptr };

  cocos2d::Camera* mCamera{ nullptr };
};

#endif
#endif
