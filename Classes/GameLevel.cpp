//
//  GameLevel.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#include "GameLevel.h"
#include "ObjectManager.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "ColorPalette.h"
#include "JsonParser.h"
#include "JsonWriter.h"
#include "JsonFormat.h"
#include "ColorPalette.h"
#include "GameLayerContainer.h"
#include "GameConfig.h"
#include "GameRenderer.h"
#include "VisibleRect.h"
#include "ShadowManager.h"
#include "GameSprite.h"
#include "InputComponent.h"
#include "CameraShake.h"

USING_NS_CC;

#define DIE_FX_TAG 1001

void GameLevel::init(GameLayerContainer *layer) {
  mGameLayer = layer;
  
  GameConfig::instance().load();
  
  CC_ASSERT(!mObjectManager);
  mObjectManager = new ObjectManager();
  mPhysicsManager = new PhysicsManager();

  mBackground = DrawNode::create();
  mBackground->setCameraMask((unsigned short) CameraFlag::USER2);
  layer->addChild(mBackground, ZORDER_BACK);
}

void GameLevel::release() {
  for (auto sm : mShadows) {
    CC_SAFE_FREE(sm);
  }
  mShadows.clear();
  CC_SAFE_DELETE(mObjectManager);
  CC_SAFE_DELETE(mPhysicsManager);
  CC_SAFE_DELETE(mPalette);
}

void GameLevel::update(float dt) {
  if (!mGameEnabled) {
    return;
  }
  if (mDieFlag) {
    dieImpl();
    return;
  }

#if EDITOR_MODE
  updateBounds();
#endif

  for (auto &obj : mObjectManager->mObjects) {
    obj.second->update(dt);
  }

  // Collision detection.
  mPhysicsManager->update(dt);
}

void GameLevel::beforeRender(float dt) {
  mPhysicsManager->beforeRender(dt);
  for (auto &obj : mObjectManager->mObjects) {
    obj.second->beforeRender(dt);
  }
  
  for (auto sm : mShadows) {
    sm->update(dt);
  }
}

GameObject *GameLevel::getHero() {
  auto hero = getObjectManager()->getObjectByID(0);
  CC_ASSERT(hero && hero->getKind() == KIND_HERO);
  return hero;
}

void GameLevel::createHero(const cocos2d::Vec2 &pos) {
  Parameter param;
  param.set(PARAM_BLOCK_KIND, KIND_HERO)
       .set(PARAM_POS, pos)
       .set(PARAM_SIZE, Size(GameConfig::instance().HeroSize, GameConfig::instance().HeroSize));

  auto hero = getObjectManager()->createObject(param);
  hero->getRenderer()->setShadowLayer(1);
  CC_ASSERT(hero->getID() == 0);
}

void GameLevel::load(const std::string &levelFile) {
  unload();
  
  JsonParser parser(levelFile);
  if (!parser) {
    CCLOGERROR("Cannot load the level file: %s", levelFile.c_str());
  }

  CCLOG("Loading level file: %s", levelFile.c_str());
  auto& doc = parser.getCurrentDocument();

  std::string paletteFile = doc[LEVEL_PALETTE_FILE].GetString();
  CC_SAFE_DELETE(mPalette);
  mPalette = new ColorPalette(paletteFile);

  mBackground->clear();
  mBackground->drawSolidRect(Vec2::ZERO, VisibleRect::rightTop(),
                             Color4F(mPalette->getBackgroundColor()));
  
  // Shadows.
  if (doc.HasMember(SHADOW_GROUP)) {
    mNumShadowGroup = doc[SHADOW_GROUP].Size();
    parser.parseArray(doc, SHADOW_GROUP, [&](JsonSizeT i, JsonValueT &val) {
      addShadowGroup();
      mShadows[i]->load(val);
      initShadowGroup(i);
    });
  }

  // Sprites.
  CC_ASSERT(mSpriteList.empty());
  if (doc.HasMember(GAME_SPRITES)) {
    parser.parseArray(doc, GAME_SPRITES, [&](JsonSizeT i, JsonValueT &val) {
      mSpriteList.push_back(new GameSprite());
      mSpriteList.back()->load(val);
      mGameLayer->getBlockRoot()->addChild(mSpriteList.back()->getSprite(),
                                           mSpriteList.back()->ZOrder);
    });
  }

  // Effects.
  CC_ASSERT(mEffects.empty());
  if (doc.HasMember(GAME_FX)) {
    parser.parseArray(doc, GAME_FX, [&](JsonSizeT i, JsonValueT &val) {
      mEffects.push_back(val.GetString());
    });
  }
  loadFx();

  // Objects.
  mHeroSpawnPos = doc[LEVEL_SPAWN_POS].GetVec2();
  createHero(mHeroSpawnPos);
  parser.parseArray(doc, LEVEL_BLOCK_ARRAY, [&](JsonSizeT i, JsonValueT& val) {
    mObjectManager->createObject(val);
  });

  // Update shadow right after loading objects.
  for (auto sm : mShadows) {
    sm->update(0);
  }

  mGameLayer->afterLoad();
  enableGame(true);
}

void GameLevel::save(const std::string &levelFile) {
  JsonWriter jsonWriter;
  jsonWriter.writeTime();
  jsonWriter.writeAuthor();

  auto &writer = jsonWriter.getWriter();
  writer.String(LEVEL_PALETTE_FILE);
  writer.String(mPalette->getFileName());

  writer.String(LEVEL_SPAWN_POS);
  writer.Vec2(mHeroSpawnPos);

  // Shadow groups.
  writer.String(SHADOW_GROUP);
  writer.StartArray();
  for (auto sm : mShadows) {
    sm->save(writer);
  }
  writer.EndArray();

  // Sprites.
  writer.String(GAME_SPRITES);
  writer.StartArray();
  for (auto sp : mSpriteList) {
    sp->save(writer);
  }
  writer.EndArray();

  // Effects.
  writer.String(GAME_FX);
  writer.StartArray();
  for (auto sp : mEffects) {
    writer.String(sp);
  }
  writer.EndArray();

  // Blocks.
  writer.String(LEVEL_BLOCK_ARRAY);
  writer.StartArray();
  for (auto &obj : mObjectManager->mObjects) {
    if (obj.second->getID() == 0) {
      continue;
    }
    obj.second->save(writer);
  }
  writer.EndArray();

  jsonWriter.save(levelFile);
}

void GameLevel::unload() {
  mObjectManager->cleanUp();
  mPhysicsManager->cleanUp();

  unloadFx();

  for (auto &sprite : mSpriteList) {
    sprite->clean();
    CC_SAFE_DELETE(sprite);
  }
  mSpriteList.clear();

  mShadowNode.clear();
  for (auto &shadow : mShadows) {
    CC_SAFE_DELETE(shadow);
  }
  mShadows.clear();

  mGameLayer->getBlockRoot()->removeAllChildren();
  mGameEnabled = false;
}

void GameLevel::die() {
  mDieFlag = true;
}

void GameLevel::dieImpl() {
  mDieFlag = false;

  CCLOG("Die!");

  enableGame(false);

  // Camera effect.
  getGameLayer()->getBlockRoot()->runAction(CameraShake::create(0.3f, 10));

  auto hero = getHero();

  // Particle effect.
  ParticleSystem *ps = ParticleSystemQuad::create("fx/diefx.plist");
  ParticleBatchNode *batch = ParticleBatchNode::createWithTexture(ps->getTexture());
  batch->addChild(ps);
  batch->setPosition(hero->getRenderer()->getPosition());
  batch->setCameraMask((unsigned short) CameraFlag::USER2);
  mGameLayer->getBlockRoot()->addChild(batch, ZORDER_DIE_FX, DIE_FX_TAG);

  hero->getRenderer()->getNode()->runAction(Sequence::create(ScaleTo::create(0.2, 0.1, 0.1),
                                                             CallFunc::create([this] {
    getHero()->getRenderer()->setVisible(false);
  }), nullptr));

  mGameLayer->runAction(Sequence::create(DelayTime::create(0.5), CallFunc::create([this] {
    enableGame(true);
  }), nullptr));
}

void GameLevel::enableGame(bool enable) {
  if (mGameEnabled == enable) {
    return;
  }

  mGameEnabled = enable;

  if (mGameEnabled) {
    reset();
    getHero()->getRenderer()->setVisible(true);
  } else {
    getHero()->getComponent<InputComponent>()->setEnabled(false);
  }

  for (auto &obj : mObjectManager->mObjects) {
    obj.second->setEnabled(enable);
  }
}

void GameLevel::reset() {
  if (mGameLayer->getBlockRoot()->getChildByTag(DIE_FX_TAG)) {
    mGameLayer->getBlockRoot()->removeChildByTag(DIE_FX_TAG);
  }

  auto hero = getHero();
  hero->getRenderer()->setPosition(mHeroSpawnPos);
  hero->getRenderer()->setOpacity(255);
  mDieFlag = false;

  mBackground->setPosition(Vec2::ZERO);
}

void GameLevel::updateBounds() {
  float halfBorderSize = BORDER_FRAME_SIZE / 2;

  float left = mObjectManager->getObjectByID(BORDER_BLOCK_LEFT)->getRenderer()->getPosition().x;
  left += halfBorderSize;
  float right = mObjectManager->getObjectByID(BORDER_BLOCK_RIGHT)->getRenderer()->getPosition().x;
  right -= halfBorderSize;
  float top = mObjectManager->getObjectByID(BORDER_BLOCK_TOP)->getRenderer()->getPosition().y;
  top -= halfBorderSize;
  float bottom = mObjectManager->getObjectByID(BORDER_BLOCK_BOTTOM)->getRenderer()->getPosition().y;
  bottom += halfBorderSize;

  mBounds = Rect(left, bottom, right - left, top - bottom);
}

void GameLevel::updateCamera(cocos2d::Camera *cam, bool forceUpdate) {
  if (!mGameEnabled && !forceUpdate) {
    return;
  }
  auto halfFrame = VisibleRect::getFrameSize() / 2;
  auto heroRel = getHero()->getRenderer()->getPosition() - halfFrame;
  auto newPos = Vec2(halfFrame) + heroRel;

#if EDITOR_MODE
  newPos.x = std::max(newPos.x, halfFrame.width);
  newPos.x = std::min(newPos.x, mBounds.size.width - halfFrame.width);
  newPos.y = std::max(newPos.y, halfFrame.height);
  newPos.y = std::min(newPos.y, mBounds.size.height + UI_LAYER_HIGHT - halfFrame.height);
  auto camRelative = newPos - VisibleRect::getVisibleRect().size / 2 - Vec2(0, UI_LAYER_HIGHT / 2);
#else
  auto center = VisibleRect::center();
  newPos.x = std::max(newPos.x, center.x);
  newPos.x = std::min(newPos.x, mBounds.size.width - center.x);
  newPos.y = std::max(newPos.y, center.y);
  newPos.y = std::min(newPos.y, mBounds.size.height - center.y);
  auto camRelative = newPos - VisibleRect::getVisibleRect().size / 2;
#endif

  mBackground->setPosition(Vec2::ZERO + camRelative);
  cam->setPosition(newPos);
}

void GameLevel::traverseObjects(std::function<void(GameObject*)> func, bool containsHero) {
  for (auto &p : mObjectManager->mObjects) {
    if (!containsHero && p.second->getID() == 0) {
      continue;
    }
    func(p.second);
  }
}

void GameLevel::addShadowGroup() {
  mShadowNode.push_back(Node::create());
  mShadows.push_back(new ShadowManager());
  if (!mShadowNode.back()->getParent()) {
    mGameLayer->getBlockRoot()->addChild(mShadowNode.back(), ZORDER_SHADOW_1);
  }
}

void GameLevel::initShadowGroup(int groupId) {
  mShadows[groupId]->init(mShadowNode[groupId]);
}

void GameLevel::loadFx() {
  for (auto i : mEffects) {
    ParticleSystem *ps = ParticleSystemQuad::create(i);
    ParticleBatchNode *batch = ParticleBatchNode::createWithTexture(ps->getTexture());
    batch->addChild(ps);
    mGameLayer->getBlockRoot()->addChild(batch, ZORDER_PARTICLE);
    batch->setCameraMask((unsigned short) CameraFlag::USER2);
    mFxNodes.push_back(batch);
  }
}

void GameLevel::unloadFx() {
  mFxNodes.clear();
  mEffects.clear();
}
