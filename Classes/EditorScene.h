#ifndef __JumpEdt__EditorScene__
#define __JumpEdt__EditorScene__

#include "cocos2d.h"
#include "GameLayerContainer.h"
#include "DrawNodeEx.h"
#include "Defines.h"
#include "Path.h"
#include "Button.h"
#include "Blocks.h"
#include "LogicManager.h"

#if EDITOR_MODE

class BlockBase;
class ShadowManager;
class Hero;
class GameLogic;

class EditorScene : public GameLayerContainer {
public:
  EditorScene();

  virtual ~EditorScene();

  static EditorScene *Scene;

  virtual bool init() override;

  void keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

  void keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

  void mouseDown(cocos2d::Event *event);

  void mouseUp(cocos2d::Event *event);

  void mouseMove(cocos2d::Event *event);

  void convertMouse(cocos2d::Point &pt, bool cameraRelative = true);

  void onWinGame() override;

  CREATE_FUNC(EditorScene);

public:

  void initDrawNodes();

  void updateGroupDrawNode();

  void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

  void onDrawPrimitive(const cocos2d::Mat4 &transform, uint32_t flags);

  void duplicate();

  void update(float dt) override;

  void updateCamera();

  void updateLightHelper();
  
  void updateMousePosLabel(const cocos2d::Point &pt);

  void enableGame(bool val, bool force = false);

  void setKind(int kind);

  void setShadowLayer(int layer);

  void drawBorder();

  void alignLeft();

  void alignRight();

  void alignUp();

  void alignDown();

  void clean() override;

  void save();

  void group();

  bool mPressingShift{false};

  bool mPressingCtrl{false};

  bool mPressingAlt{false};

  bool mPressingM{false};

  bool mPressingN{false};

  bool mPressingB{false};

  bool mPressingV{false};

  bool mPressingComma{false};

  bool mPressingPeriod{false};

  BlockBase *mMovingBlock{nullptr};

  cocos2d::Point mLastPoint;

  cocos2d::Vec2 mLastCursorInView;

  cocos2d::Sprite *mSpawnPoint{nullptr};

  cocos2d::Sprite *mLightPoint{nullptr};

  cocos2d::Sprite *mLightArraw{nullptr};
  
  cocos2d::Label *mMousePosLabel{nullptr};

  std::set<BlockBase *> mSelections;

  BlockBase *mSelectionHead{nullptr};

  bool mPathMode{false};

  bool mShowGrid{false};

  bool mMovingCamera{false};

  std::string mCurFileName;

  cocos2d::DrawNode *mGridNode{nullptr};

  cocos2d::DrawNode *mGroupNode{nullptr};

  cocos2d::DrawNode *mBorderNode{nullptr};
};

#endif
#endif
