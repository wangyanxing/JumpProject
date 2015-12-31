//
//  Shadows.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/22/14.
//
//

#include "Shadows.h"
#include "DrawNodeEx.h"
#include "VisibleRect.h"
#include "EditorScene.h"
#include "LogicManager.h"
#include "Hero.h"
#include "BlockRenderer.h"

#if EDITOR_MODE

# include "EditorScene.h"

#else
# include "GameScene.h"
#endif

#if USE_SHADOW

USING_NS_CC;

void colorMix(const Color4B &src, const Color4B &dst, float r, Color4B &out) {
  out.r = dst.r * r + src.r * (1 - r);
  out.g = dst.g * r + src.g * (1 - r);
  out.b = dst.b * r + src.b * (1 - r);
  out.a = dst.a * r + src.a * (1 - r);
}

ShadowManager::ShadowManager(cocos2d::Node *parentNode) {
  // Load shaders
  auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/normal_shadow.fsh");
  auto shaderContent = FileUtils::getInstance()->getStringFromFile(shaderfile);
  auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_vert,
                                                 shaderContent.c_str());
  auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);

  for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
    mShadowDrawers[i] = DrawNodeEx::create();
    mShadowDrawers[i]->setGLProgramState(glProgramState);

    Size visibleSize = VisibleRect::getVisibleRect().size;
    mRenderTextures[i] = RenderTexture::create(visibleSize.width,
                                               visibleSize.height,
                                               Texture2D::PixelFormat::RGBA8888);
    mRenderTextures[i]->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    mRenderTextures[i]->getSprite()->setOpacity(255 * mShadowDarkness);
    parentNode->addChild(mRenderTextures[i]);

    mRenderTextures[i]->retain();
    mShadowDrawers[i]->retain();
  }

  mLightPos = VisibleRect::center();
  mLightPos.x = 300;
  mLightPos.y = VisibleRect::top().y - 10;
  mOriginLightPos = mLightPos;
}

ShadowManager::~ShadowManager() {
  for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
    mShadowDrawers[i]->release();
    mRenderTextures[i]->removeFromParent();
    mRenderTextures[i]->release();
  }
}

ShadowManager::ShadowEntry ShadowManager::getShadowEntry(const std::vector<Vec2> &pts,
                                                         const cocos2d::Vec2 &lightPos) {
  ShadowEntry ret;
  CC_ASSERT(pts.size() == 4);
  if (lightPos.x >= pts[0].x && lightPos.x <= pts[1].x) {
    if (lightPos.y >= pts[0].y) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[1];
    } else if (lightPos.y <= pts[2].y) {
      ret.pt1 = pts[2];
      ret.pt2 = pts[3];
    } else {
      ret.noShadow = true;
    }
  } else if (lightPos.x < pts[0].x) {
    if (lightPos.y >= pts[0].y) {
      ret.pt1 = pts[2];
      ret.pt2 = pts[1];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[0];
    } else if (lightPos.y <= pts[2].y) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[3];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[2];
    } else {
      ret.pt1 = pts[0];
      ret.pt2 = pts[2];
    }
  } else if (lightPos.x > pts[1].x) {
    if (lightPos.y >= pts[1].y) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[3];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[1];
    } else if (lightPos.y <= pts[3].y) {
      ret.pt1 = pts[1];
      ret.pt2 = pts[2];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[3];
    } else {
      ret.pt1 = pts[1];
      ret.pt2 = pts[3];
    }
  }
  return ret;
}

void ShadowManager::updateBlock(BlockBase *block,
                                std::vector<cocos2d::V2F_C4B_T2F_Triangle> &triangles,
                                bool clipX) {
  if (!block->mCastShadow || !block->isVisible() || !block->mCanPickup) {
    return;
  }

  if (block->getRenderer()->getBoundingBox().containsPoint(mLightPos)) {
    return;
  }

  auto bounds = GameLogic::Game->mBounds;

#if EDITOR_MODE
  auto camera = EditorScene::Scene->mCamera;
  auto camRelative = camera->getPosition() - VisibleRect::getVisibleRect().size / 2 -
                     Vec2(0, UI_LAYER_HIGHT / 2);
#else
  auto camera = GameScene::Scene->getCamera();
  auto camRelative = camera->getPosition() - VisibleRect::getVisibleRect().size / 2;
#endif

  auto lightPos = mLightPos - camRelative;

  std::vector<Vec2> pts;
  block->getPointsForShadow(lightPos, pts);
  for (auto &p : pts) {
    p -= camRelative;
  }
  auto entries = getShadowEntry(pts, lightPos);
  if (entries.noShadow) {
    return;
  }

  Color4B colorBase = Color4B::BLACK;
  const float LENGTH = 1500;

  Vec2 dir0, dir1;
  if (mLightType == LIGHT_POINT) {
    dir0 = entries.pt1 - lightPos;
    dir0.normalize();
    dir1 = entries.pt2 - lightPos;
    dir1.normalize();
  } else if (mLightType == LIGHT_DIR) {
    dir0 = dir1 = mLightDir;
  }

  Vec2 f0 = entries.pt1 + dir0 * LENGTH;
  Vec2 f1 = entries.pt2 + dir1 * LENGTH;

  if (clipX) {
    Vec2 leftupper(0, 2000);
    Vec2 leftlower(0, -1000);

    Vec2 rightupper(bounds.size.width, 2000);
    Vec2 rightlower(bounds.size.width, -1000);

    if (Vec2::isSegmentIntersect(leftlower, leftupper, entries.pt1, f0)) {
      f0 = Vec2::getIntersectPoint(leftlower, leftupper, entries.pt1, f0);
    }
    if (Vec2::isSegmentIntersect(leftlower, leftupper, entries.pt2, f1)) {
      f1 = Vec2::getIntersectPoint(leftlower, leftupper, entries.pt2, f1);
    }
    if (Vec2::isSegmentIntersect(rightlower, rightupper, entries.pt1, f0)) {
      f0 = Vec2::getIntersectPoint(rightlower, rightupper, entries.pt1, f0);
    }
    if (Vec2::isSegmentIntersect(rightlower, rightupper, entries.pt2, f1)) {
      f1 = Vec2::getIntersectPoint(rightlower, rightupper, entries.pt2, f1);
    }
  }

  V2F_C4B_T2F_Triangle t;
  t.a.vertices = entries.pt1;
  t.a.colors = colorBase;

  t.b.vertices = f1;
  t.b.colors = colorBase;

  t.c.vertices = f0;
  t.c.colors = colorBase;

  triangles.push_back(t);

  t.a.vertices = entries.pt1;
  t.a.colors = colorBase;

  t.b.vertices = entries.pt2;
  t.b.colors = colorBase;

  t.c.vertices = f1;
  t.c.colors = colorBase;

  triangles.push_back(t);

  if (entries.needMakeUp) {
    t.a.vertices = entries.pt1;
    t.a.colors = colorBase;

    t.b.vertices = entries.pt2;
    t.b.colors = colorBase;

    t.c.vertices = entries.makeUpPt;
    t.c.colors = colorBase;

    triangles.push_back(t);
  }
}

void ShadowManager::update(float dt) {
  std::vector<V2F_C4B_T2F_Triangle> triangles[NUM_SHADOW_LAYERS];
  for (auto b : GameLogic::Game->mBlocks) {
    auto block = b.second;
    updateBlock(block, triangles[block->mShadowLayerID]);
  }

  updateBlock(GameLogic::Game->mHero, triangles[SHADOW_LAYER_HERO]);

  for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
#if EDITOR_MODE
    mRenderTextures[i]->setPosition(EditorScene::Scene->mCamera->getPosition() -
                                    Vec2(0, UI_LAYER_HIGHT / 2));
#else
    mRenderTextures[i]->setPosition(GameScene::Scene->getCamera()->getPosition());
#endif
    mShadowDrawers[i]->clear();
    if (!triangles[i].empty()) {
      mShadowDrawers[i]->drawTriangles(triangles[i]);
    }
    mRenderTextures[i]->beginWithClear(0, 0, 0, 0);
    mShadowDrawers[i]->visit();
    mRenderTextures[i]->end();
  }

  if (!mShadowMovingEnable || !GameLogic::Game->mGameMode) {
    return;
  }

  mLightMoveTimer += dt;

  bool lightLeft = mLightPos.x <= VisibleRect::center().x;
  bool heroLeft = GameLogic::Game->mHero->getPosition().x <= VisibleRect::center().x;

  float dis = std::abs(mOriginLightPos.x - VisibleRect::center().x);

  if (lightLeft == heroLeft && mLightMoveTimer > 10) {
    mLightMoveTimer = 0;
    mMoveTarget = heroLeft ? VisibleRect::center().x + dis : VisibleRect::center().x - dis;
    mMovingSpeed = (dis * 2) / 3.0f;
    if (!heroLeft) {
      mMovingSpeed *= -1;
    }
    mMoving = true;
  }

  if (mMoving) {
    mLightPos.x += mMovingSpeed * dt;
    if (mMovingSpeed > 0) {
      if (mLightPos.x >= VisibleRect::center().x + dis) {
        mLightPos.x = VisibleRect::center().x + dis;
        mMoving = false;
      }
    } else {
      if (mLightPos.x <= VisibleRect::center().x - dis) {
        mLightPos.x = VisibleRect::center().x - dis;
        mMoving = false;
      }
    }
  }
}

void ShadowManager::reset() {
  mLightMoveTimer = 10;
  mLightPos = mOriginLightPos;
  mMoving = false;
  mMoveTarget = 0;
  mMovingSpeed = 0;
}

void ShadowManager::updateLightDir() {
  mLightDir = Vec2(1, 0);
  mLightDir.rotate(Vec2::ZERO, MATH_DEG_TO_RAD(-mLightDirDegree));
}

#endif
