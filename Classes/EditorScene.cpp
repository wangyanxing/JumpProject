#include "EditorScene.h"
#include "VisibleRect.h"
#include "GameUtils.h"
#include "MapSerial.h"
#include "UILayer.h"
#include "Events.h"
#include "Shadows.h"
#include "LogicManager.h"
#include "UIColorEditor.h"
#include "EffectSprite.h"
#include "Hero.h"
#include "Defines.h"
#include "BlockRenderer.h"

#include "cocos-ext.h"

#if EDITOR_MODE

#include "PathLib.h"

#include <iostream>

USING_NS_CC;
USING_NS_CC_EXT;

EditorScene::EditorScene() : GameLayerContainer() {
}

EditorScene::~EditorScene() {
}

EditorScene *EditorScene::Scene = nullptr;

bool EditorScene::init() {
  Scene = this;
  GameLayerContainer::init();

  MapSerial::saveRemoteMaps();

  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(EditorScene::keyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(EditorScene::keyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto mouseListener = EventListenerMouse::create();
  mouseListener->onMouseDown = CC_CALLBACK_1(EditorScene::mouseDown, this);
  mouseListener->onMouseUp = CC_CALLBACK_1(EditorScene::mouseUp, this);
  mouseListener->onMouseMove = CC_CALLBACK_1(EditorScene::mouseMove, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

  mLightPoint = Sprite::create("images/sun.png");
  mLightPoint->setCameraMask((unsigned short) CameraFlag::USER2);
  addChild(mLightPoint, 100);
  mLightPoint->setPosition(300, 520);

  mLightArraw = Sprite::create("images/arraw.png");
  mLightArraw->setCameraMask((unsigned short) CameraFlag::USER2);
  addChild(mLightArraw, 100);
  mLightArraw->setPosition(35, 35);
  mLightArraw->setScale(0.1f);
  mLightArraw->setVisible(false);
  mLightArraw->setOpacity(128);

  mSpawnPoint = Sprite::create("images/cross.png");
  mSpawnPoint->setCameraMask((unsigned short) CameraFlag::USER2);
  addChild(mSpawnPoint, 100);
  mSpawnPoint->setPosition(50, 100);
  getGame()->mSpawnPos = mSpawnPoint->getPosition();
  mSpawnPoint->setScale(0.3f);

  MapSerial::loadLastEdit();

  UIColorEditor::colorEditor->onSetColorFunc = [&](int index, cocos2d::Color3B color) {
      for (auto sel : mSelections) {
        sel->setColor(index);
      }
  };

  initDrawNodes();
  return true;
}

void EditorScene::onWinGame() {
  enableGame(false, true);
}

void EditorScene::mouseDown(cocos2d::Event *event) {
  auto mouse = (EventMouse *) event;
  Point pt(mouse->getCursorX(), mouse->getCursorY());
  Point ptInView = pt;
  convertMouse(pt);
  convertMouse(ptInView, false);

  auto bounds = getGame()->mBounds;
  Rect rect = Rect(0, 0, bounds.size.width, bounds.size.height);
  if (!rect.containsPoint(ptInView)) {
    return;
  }

  if (mPressingM && !getGame()->mGameMode) {
    mSpawnPoint->setPosition(pt);
    getGame()->mSpawnPos = mSpawnPoint->getPosition();
    return;
  }

#if USE_SHADOW
  if (mPressingN && !getGame()->mGameMode) {
    mLightPoint->setPosition(pt);
    getGame()->mShadows[0]->mLightPos = mLightPoint->getPosition();
    getGame()->mShadows[0]->mOriginLightPos = mLightPoint->getPosition();
    return;
  }
#endif
  
  if (mPressingV) {
    getGame()->createParticle(pt);
    getGame()->mStarList.push_back(pt);
    return;
  }

  if (mPressingShift) {
    // Create block
    getGame()->createBlock(pt, KIND_BLOCK);
    mMovingBlock = nullptr;
  } else if (mPressingAlt) {
    // Move camrea
    mMovingCamera = true;
  } else {
    // Move blocks
    if (!mPressingCtrl) {
      mSelections.clear();
      mPathMode = false;
      mSelectionHead = nullptr;
    }

    getGame()->blockTraversal([&](BlockBase *bl) {
        bl->switchToNormalImage();
        auto box = bl->getRenderer()->getBoundingBox();
        if (box.containsPoint(pt) && bl->mCanPickup) {
          mSelections.insert(bl);
          mMovingBlock = bl;
          mSelectionHead = bl;
        }
    });

    mLastPoint = pt;

    for (auto sel : mSelections) {
      sel->switchToSelectionImage();
    }
    if (mSelectionHead) {
      mSelectionHead->getRenderer()->setColor(Color3B(200, 0, 255));
    }
  }
}

void EditorScene::mouseUp(cocos2d::Event *event) {
  auto mouse = (EventMouse *) event;
  Point pt(mouse->getCursorX(), mouse->getCursorY());
  Point ptInView = pt;
  convertMouse(pt);
  convertMouse(ptInView, false);

  auto bounds = getGame()->mBounds;
  Rect rect = Rect(0, 0, bounds.size.width, bounds.size.height);
  if (!rect.containsPoint(ptInView)) {
    return;
  }

  mMovingBlock = nullptr;
  mMovingCamera = false;
}

void EditorScene::mouseMove(cocos2d::Event *event) {
  auto mouse = (EventMouse *) event;
  auto location = mouse->getLocationInView();
  auto delta = location - mLastCursorInView;
  mLastCursorInView = location;

  Point pt(mouse->getCursorX(), mouse->getCursorY());
  Point ptInView = pt;
  convertMouse(pt);
  convertMouse(ptInView, false);
  Point dt = pt - mLastPoint;
  mLastPoint = pt;

  auto bounds = getGame()->mBounds;
  Rect rect = Rect(0, 0, bounds.size.width, bounds.size.height);
  if (!rect.containsPoint(ptInView)) {
    return;
  }

  if (mMovingBlock) {
    for (auto sel: mSelections) {
      sel->moveX(dt.x);
      sel->moveY(dt.y);
    }
  } else if (mMovingCamera) {
    auto camPos = getCamera()->getPosition();
    getCamera()->setPosition(camPos - Vec2(delta.x, delta.y));
  }
  
  updateMousePosLabel(ptInView);
}

void EditorScene::convertMouse(cocos2d::Point &pt, bool cameraRelative) {
  auto visRect = Director::getInstance()->getOpenGLView()->getVisibleRect();
  auto height = visRect.origin.y + visRect.size.height;

  pt.y = height + pt.y;
  pt = convertToNodeSpace(pt);
  if (cameraRelative) {
    auto camRelative = getCamera()->getPosition() - visRect.size / 2;
    pt += camRelative;
  }
}

void EditorScene::keyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
  if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
    mPressingShift = true;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) {
    mPressingCtrl = true;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_ALT) {
    mPressingAlt = true;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_V && mPressingCtrl) {
    duplicate();
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_EQUAL) {
    for (auto sel : mSelections) {
      sel->addWidth(mPressingAlt ? 20 : 5);
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_MINUS) {
    for (auto sel : mSelections) {
      sel->subWidth(mPressingAlt ? 20 : 5);
    }
  }

  if (keyCode >= EventKeyboard::KeyCode::KEY_1 &&
      keyCode <= EventKeyboard::KeyCode::KEY_7) {
    if (mPressingShift) {
      setShadowLayer((int) keyCode - (int) EventKeyboard::KeyCode::KEY_1);
    } else {
      setKind((int) keyCode - (int) EventKeyboard::KeyCode::KEY_1 + 1);
    }
  }

  // Set as exit door
  if (keyCode == EventKeyboard::KeyCode::KEY_0) {
    for (auto sel : mSelections) {
      sel->setKind(KIND_DEATH);
      sel->getRenderer()->setTexture(EXIT_IMAGE);
      sel->mTriggerEvents = {"exit"};
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_BRACKET) {
    for (auto sel : mSelections) {
      sel->subThickness(mPressingAlt ? 10 : 5);
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_BRACKET) {
    for (auto sel : mSelections) {
      sel->addThickness(mPressingAlt ? 10 : 5);
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
    if (mPathMode && mSelectionHead) {
      auto p = mSelectionHead->mPath.getBackPos();
      p.y += mPressingAlt ? 20 : 1;
      mSelectionHead->mPath.setBackPos(p);
    } else {
      if (mPressingShift) {
        if (mSelectionHead) {
          alignUp();
        }
      } else {
        for (auto sel : mSelections) {
          sel->moveY(mPressingAlt ? 20 : 1);
        }
      }
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
    if (mPathMode && mSelectionHead) {
      auto p = mSelectionHead->mPath.getBackPos();
      p.y -= mPressingAlt ? 20 : 1;
      mSelectionHead->mPath.setBackPos(p);
    } else {
      if (mPressingShift) {
        if (mSelectionHead) {
          alignDown();
        }
      } else {
        for (auto sel : mSelections) {
          sel->moveY(mPressingAlt ? -20 : -1);
        }
      }
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
    if (mPathMode && mSelectionHead) {
      auto p = mSelectionHead->mPath.getBackPos();
      p.x -= mPressingAlt ? 20 : 1;
      mSelectionHead->mPath.setBackPos(p);
    } else {
      if (mPressingShift) {
        if (mSelectionHead) {
          alignLeft();
        }
      } else {
        for (auto sel : mSelections) {
          sel->moveX(mPressingAlt ? -20 : -1);
        }
      }
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
    if (mPathMode && mSelectionHead) {
      auto p = mSelectionHead->mPath.getBackPos();
      p.x += mPressingAlt ? 20 : 1;
      mSelectionHead->mPath.setBackPos(p);
    } else {
      if (mPressingShift) {
        if (mSelectionHead) {
          alignRight();
        }
      } else {
        for (auto sel : mSelections) {
          sel->moveX(mPressingAlt ? 20 : 1);
        }
      }
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_F1) {
    auto d = getScene()->getPhysicsWorld()->getDebugDrawMask();
    if (d == PhysicsWorld::DEBUGDRAW_ALL) {
      getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
    } else {
      getScene()->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_F2) {
    mBorderNode->setVisible(!mBorderNode->isVisible());
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_P) {
    for (auto sel : mSelections) {
      sel->rotate();
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_N) {
    if (mPressingCtrl) {
      save();
      clean();
      getGame()->createFixedBlocks();
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_I) {
    for (auto sel : mSelections) {
      sel->mUVFlipped = !sel->mUVFlipped;
      sel->normalizeUV();
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_O) {
    if (mPressingCtrl) {
      MapSerial::loadMap();
    } else {
      for (auto sel : mSelections) {
        if (sel->mButton) {
          sel->mButton->rotateDir();
        }
      }
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_S) {
    if (mPressingCtrl) {
      // Save map
      if (!mCurFileName.empty())
        MapSerial::saveMap(mCurFileName.c_str());
      else
        MapSerial::saveMap();
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_R) {
    if (!getGame()->mGameMode) {
      getCamera()->setPosition(VisibleRect::getFrameSize() / 2);
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_F5) {
    MapSerial::loadMap(std::string(mCurFileName).c_str());
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_F4) {
    MapSerial::saveRemoteMaps();
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_G) {
    mShowGrid = !mShowGrid;
    mGridNode->setVisible(mShowGrid);
    mMousePosLabel->setVisible(mShowGrid);
    // Also show or hide ID labels
    getGame()->blockTraversal([&](BlockBase *b) {
        b->mIDLabel->setVisible(mShowGrid);
        b->mShowIDLabel = mShowGrid;
    });
  }

  // Path mode
  if (keyCode == EventKeyboard::KeyCode::KEY_F && mSelectionHead && !getGame()->mGameMode) {
    // Only work for one selection
    mPathMode = true;

    if (mSelectionHead->mPath.empty()) {
      auto pos = mSelectionHead->getPosition();
      mSelectionHead->mPath.push(pos);
      mSelectionHead->mPath.push(pos + (mPressingShift ? Vec2(0, 50) : Vec2(50, 0)));
    } else {
      auto pos = mSelectionHead->mPath.getBackPos();
      mSelectionHead->mPath.push(pos + (mPressingShift ? Vec2(0, 50) : Vec2(50, 0)));
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
    // Only work for one selection
    if (mPathMode) {
      mPathMode = false;
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_SPACE) {
    if (getGame()->mGameMode) {
      getGame()->mJumpFlag = true;
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_RETURN ||
      keyCode == EventKeyboard::KeyCode::KEY_ENTER ||
      keyCode == EventKeyboard::KeyCode::KEY_L) {
    enableGame(!getGame()->mGameMode);
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_M) {
    mPressingM = true;
  }
  if (keyCode == EventKeyboard::KeyCode::KEY_N) {
    mPressingN = true;
  }
  if (keyCode == EventKeyboard::KeyCode::KEY_B) {
    mPressingB = true;
  }
  if (keyCode == EventKeyboard::KeyCode::KEY_V) {
    mPressingV = true;
  }
  if (keyCode == EventKeyboard::KeyCode::KEY_COMMA) {
    mPressingComma = true;
  }
  if (keyCode == EventKeyboard::KeyCode::KEY_PERIOD) {
    mPressingPeriod = true;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_J) {
    group();
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_DELETE ||
      keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
    if (mPathMode && mSelectionHead) {
      mSelectionHead->mPath.pop();
      if (mSelectionHead->mPath.empty()) {
        mPathMode = false;
      }
    } else {
      for (auto sel : mSelections) {
        if (sel->mCanDelete) {
          getGame()->deleteBlock(sel);
        }
      }
    }
    mSelections.clear();
  }

  if (getGame()->mGameMode) {
    if (keyCode == EventKeyboard::KeyCode::KEY_A) {
      getGame()->mMoveLeft = true;
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_D) {
      getGame()->mMoveRight = true;
    }
  } else {
    if (keyCode == EventKeyboard::KeyCode::KEY_A && mPressingCtrl) {
      mSelections.clear();
      for (auto b : getGame()->mBlocks) {
        if (!b.second->mCanPickup) continue;
        mSelections.insert(b.second);
        mSelectionHead = b.second;
        b.second->switchToSelectionImage();
      }
    }
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_E && mPressingCtrl) {
    if (!MapSerial::CurrentEditingFile.empty()) {
      PathLib::openInSystem(MapSerial::CurrentEditingFile.c_str());
    }
  }
}

void EditorScene::keyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
  if (keyCode == EventKeyboard::KeyCode::KEY_SHIFT) {
    mPressingShift = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_CTRL) {
    mPressingCtrl = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_ALT) {
    mPressingAlt = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_M) {
    mPressingM = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_N) {
    mPressingN = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_B) {
    mPressingB = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_V) {
    mPressingV = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_COMMA) {
    mPressingComma = false;
  }

  if (keyCode == EventKeyboard::KeyCode::KEY_PERIOD) {
    mPressingPeriod = false;
  }

  if (getGame()->mGameMode) {
    if (keyCode == EventKeyboard::KeyCode::KEY_A) {
      getGame()->mMoveLeft = false;
    }
    if (keyCode == EventKeyboard::KeyCode::KEY_D) {
      getGame()->mMoveRight = false;
    }
  }
}

void EditorScene::enableGame(bool val, bool force) {
  getGame()->enableGame(val, force);
  mSpawnPoint->setVisible(!val);
  updateLightHelper();

  if (mBorderNode) {
    mBorderNode->setVisible(!val);
  }
  if (mGroupNode) {
    mGroupNode->setVisible(!val);
  }
  if (mMousePosLabel) {
    mMousePosLabel->setVisible(!val);
  }
  if (val) {
    mPressingV = mPressingB = mPressingN = mPressingM = false;    
  } else {
    getCamera()->setPosition(VisibleRect::getFrameSize() / 2);
    getGame()->restoreBackgroundPos();
  }
}

void EditorScene::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
  Layer::draw(renderer, transform, flags);

  static CustomCommand _customCommand;
  _customCommand.init(400);
  _customCommand.func = CC_CALLBACK_0(EditorScene::onDrawPrimitive, this, transform, flags);
  renderer->addCommand(&_customCommand);
}

void EditorScene::onDrawPrimitive(const Mat4 &transform, uint32_t flags) {
  Director *director = Director::getInstance();
  director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
  director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
  director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void EditorScene::update(float dt) {
  GameLayerContainer::update(dt);

  updateCamera();
  updateGroupDrawNode();
  if (mBorderNode->isVisible()) {
    drawBorder();
  }

  if (!getGame()->mGameMode && mPressingCtrl) {
    if (mPressingComma || mPressingPeriod) {
      getGame()->mShadows[0]->mLightDirDegree += dt * 100 * (mPressingComma ? 1 : -1);
      getGame()->mShadows[0]->updateLightDir();
      updateLightHelper();
    }
  }
}

void EditorScene::updateCamera() {
  getGame()->updateCamera(getCamera());
}

void EditorScene::setShadowLayer(int layer) {
#if USE_SHADOW
  if (layer >= ShadowManager::NUM_SHADOW_LAYERS) {
    return;
  }

  for (auto sel : mSelections) {
    sel->mShadowLayerID = layer;
  }
#endif
}

void EditorScene::setKind(int kind) {
  if (kind >= KIND_MAX) {
    return;
  }

  for (auto sel : mSelections) {
    sel->setKind(BlockKind(kind));
  }
}

void EditorScene::duplicate() {
  std::set<BlockBase *> duplicated;
  Vec2 bias(20, 20);

  for (auto sel : mSelections) {
    BlockBase *block = new BlockBase();
    auto pos = sel->getPosition() + bias;
    block->mRestorePosition = pos;
    block->mRestoreSize = sel->getSize();
    block->setKind(sel->mKind);
    block->setColor(sel->mPaletteIndex);
    block->mPath.cloneFrom(sel->mPath, bias);

    block->mRestoreSize = block->getSize();
    block->mRestorePosition = block->getPosition();

    getGame()->mBlocks[block->mID] = block;

    duplicated.insert(block);
  }

  for (auto sel : mSelections) {
    sel->getRenderer()->setColor(sel->getColor());
  }

  mSelections.clear();

  mSelections = duplicated;
  for (auto sel : mSelections) {
    sel->switchToSelectionImage();
  }
}

void EditorScene::alignLeft() {
  auto p = mSelectionHead->getPosition();
  auto w = mSelectionHead->getSize().width;
  float mostLeft = p.x - w / 2;

  for (auto sel : mSelections) {
    if (sel != mSelectionHead) {
      auto selWid = sel->getSize().width;
      sel->setPositionX(mostLeft + selWid / 2);
      sel->mRestorePosition = sel->getPosition();
    }
  }
}

void EditorScene::alignRight() {
  auto p = mSelectionHead->getPosition();
  auto w = mSelectionHead->getSize().width;
  float mostRight = p.x + w / 2;

  for (auto sel : mSelections) {
    if (sel != mSelectionHead) {
      auto selWid = sel->getSize().width;
      sel->setPositionX(mostRight - selWid / 2);
      sel->mRestorePosition = sel->getPosition();
    }
  }
}

void EditorScene::alignUp() {
  auto p = mSelectionHead->getPosition();
  auto h = mSelectionHead->getSize().height;
  float mostUp = p.y + h / 2;

  for (auto sel : mSelections) {
    if (sel != mSelectionHead) {
      auto selHei = sel->getSize().height;
      sel->setPositionY(mostUp - selHei / 2);
      sel->mRestorePosition = sel->getPosition();
    }
  }
}

void EditorScene::alignDown() {
  auto p = mSelectionHead->getPosition();
  auto h = mSelectionHead->getSize().height;
  float mostDown = p.y - h / 2;

  for (auto sel : mSelections) {
    if (sel != mSelectionHead) {
      auto selHei = sel->getSize().height;
      sel->setPositionY(mostDown + selHei / 2);
      sel->mRestorePosition = sel->getPosition();
    }
  }
}

void EditorScene::group() {
  if (mSelections.empty() || !mSelectionHead) {
    return;
  }

  auto it = getGame()->mGroups.find(mSelectionHead);
  if (it != getGame()->mGroups.end()) {
    for (auto s : it->second) {
      s->reset();
    }
    getGame()->mGroups.erase(it);
    UILayer::Layer->addMessage("Ungroup");
  } else {
    getGame()->mGroups[mSelectionHead].clear();
    for (auto s : mSelections) {
      if (s == mSelectionHead) {
        continue;
      }
      getGame()->mGroups[mSelectionHead].push_back(s);
    }
    UILayer::Layer->addMessage("Group");
  }
}

void EditorScene::save() {
  if (!mCurFileName.empty()) {
    MapSerial::saveMap(mCurFileName.c_str());
  }
}

void EditorScene::clean() {
  mSelectionHead = nullptr;
  mSelections.clear();

  GameLayerContainer::clean();

  mCurFileName = "";
  UILayer::Layer->setFileName("Untitled");
}

void EditorScene::initDrawNodes() {
  auto size = getGame()->mHero->getSize();
  Color4F gridColor(0.8f, 0.8f, 0.8f, 1);

  mGridNode = DrawNode::create();
  float y = 0;
  while (y < VisibleRect::top().y) {
    mGridNode->drawLine(Vec2(0, y), Vec2(VisibleRect::right().x, y), gridColor);
    y += size.height;
  }

  float x = 0;
  while (x < VisibleRect::right().x + 50) {
    mGridNode->drawLine(Vec2(x, 0), Vec2(x, VisibleRect::top().y), gridColor);
    x += size.width;
  }

  mBorderNode = DrawNode::create();
  mBorderNode->setPosition(0, 0);

  mGridNode->setCameraMask((unsigned short) CameraFlag::USER2);
  mBorderNode->setCameraMask((unsigned short) CameraFlag::USER2);

  addChild(mGridNode, ZORDER_EDT_GRID);
  addChild(mBorderNode, ZORDER_EDT_BORDER);

  mGridNode->setVisible(false);
  
  TTFConfig config("fonts/Square.ttf", 25);
  mMousePosLabel = Label::createWithTTF(config, "");
  mMousePosLabel->setScale(0.7f);
  mMousePosLabel->setOpacity(200);
  mMousePosLabel->setCameraMask((unsigned short) CameraFlag::USER2);
  mMousePosLabel->setVisible(false);
  addChild(mMousePosLabel, 50);
}

void EditorScene::updateGroupDrawNode() {
  if (!mGroupNode) {
    mGroupNode = DrawNode::create();
    addChild(mGroupNode, 50);
    mGroupNode->setCameraMask((unsigned short) CameraFlag::USER2);
  }
  mGroupNode->clear();
  Color4F lineColor(0.06f, 0.18f, 0.96f, 1);
  for (auto g : getGame()->mGroups) {
    auto head = g.first;
    for (auto m : g.second) {
      auto headPos = head->getPosition();
      mGroupNode->drawSolidCircle(m->getPosition(), 3, 0, 20, 1, 1, lineColor);
      mGroupNode->drawSegment(headPos, m->getPosition(), 1, lineColor);
    }
  }
}

void EditorScene::drawBorder() {
  getGame()->updateBounds();
  auto bounds = getGame()->mBounds;
  float left = bounds.origin.x, bottom = bounds.origin.y;
  float right = left + bounds.size.width, top = bottom + bounds.size.height;
  mBorderNode->clear();
  mBorderNode->drawSegment(Vec2(left, top), Vec2(left, bottom), 1, Color4F::RED);
  mBorderNode->drawSegment(Vec2(right, top), Vec2(right, bottom), 1, Color4F::RED);
  mBorderNode->drawSegment(Vec2(left, top), Vec2(right, top), 1, Color4F::RED);
  mBorderNode->drawSegment(Vec2(left, bottom), Vec2(right, bottom), 1, Color4F::RED);
}

void EditorScene::updateLightHelper() {
  if (getGame()->mShadows[0]->mLightType == ShadowManager::LIGHT_POINT) {
    mLightPoint->setVisible(!getGame()->mGameMode);
    mLightArraw->setVisible(false);
    mLightPoint->setPosition(GameLogic::Game->mShadows[0]->mOriginLightPos);
  } else if (getGame()->mShadows[0]->mLightType == ShadowManager::LIGHT_DIR) {
    mLightPoint->setVisible(false);
    mLightArraw->setVisible(!getGame()->mGameMode);
    mLightArraw->setRotation(getGame()->mShadows[0]->mLightDirDegree);
  }
}

void EditorScene::updateMousePosLabel(const cocos2d::Point &pt) {
  if (getGame()->mGameMode || !mShowGrid) {
    mMousePosLabel->setVisible(false);
    return;
  }
  char posBuffer[32];
  sprintf(posBuffer, "(%.1f, %.1f)", pt.x, pt.y);
  mMousePosLabel->setVisible(true);
  mMousePosLabel->setPosition(pt.x, pt.y + 10);
  mMousePosLabel->setString(posBuffer);
}

#endif
