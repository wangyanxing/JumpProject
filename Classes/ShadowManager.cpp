//
//  ShadowManager.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/25/16.
//
//

#include "ShadowManager.h"
#include "VisibleRect.h"
#include "GameObject.h"
#include "GameLevel.h"
#include "GameRenderer.h"
#include "JsonFormat.h"
#include "DrawNodeEx.h"
#include "GameLayerContainer.h"

USING_NS_CC;

static void colorMix(const Color4B &src, const Color4B &dst, float r, Color4B &out) {
  out.r = dst.r * r + src.r * (1 - r);
  out.g = dst.g * r + src.g * (1 - r);
  out.b = dst.b * r + src.b * (1 - r);
  out.a = dst.a * r + src.a * (1 - r);
}

ShadowManager::ShadowManager() {
  mLightPos.x = 300;
  mLightPos.y = VisibleRect::top().y - 10;
  mOriginLightPos = mLightPos;

  std::fill(mShadowDrawers, mShadowDrawers + NUM_SHADOW_LAYERS, nullptr);
  std::fill(mRenderTextures, mRenderTextures + NUM_SHADOW_LAYERS, nullptr);
}

ShadowManager::~ShadowManager() {
}

void ShadowManager::load(JsonValueT &group) {
  mPosX = group[SHADOW_GROUP_POSX].GetDouble();
  mWidth = group[SHADOW_GROUP_WIDTH].GetDouble();
  mLightType = group[SHADOW_GROUP_LIGHT_TYPE].getEnum<ShadowLightType>();
  mLightDirDegree = group[SHADOW_GROUP_LIGHT_DIR].GetDouble();
  mLightPos = group[SHADOW_GROUP_LIGHT_POS].GetVec2();
  mShadowDarkness = group[SHADOW_GROUP_LIGHT_DARKNESS].GetDouble();
  updateLightDir();
}

void ShadowManager::save(JsWriter &writer) {
  writer.StartObject();

  writer.String(SHADOW_GROUP_POSX);
  writer.Float(mPosX);

  writer.String(SHADOW_GROUP_WIDTH);
  writer.Float(mWidth);

  writer.String(SHADOW_GROUP_LIGHT_TYPE);
  writer.Enum<ShadowLightType>(mLightType);

  writer.String(SHADOW_GROUP_LIGHT_DIR);
  writer.Float(mLightDirDegree);

  writer.String(SHADOW_GROUP_LIGHT_POS);
  writer.Vec2(mLightPos);

  writer.String(SHADOW_GROUP_LIGHT_DARKNESS);
  writer.Float(mShadowDarkness);

  writer.EndObject();
}

void ShadowManager::init(cocos2d::Node *parentNode) {
  if (mShadowDrawers[0]) {
    return;
  }

  auto shaderfile = FileUtils::getInstance()->fullPathForFilename(DEFAULT_SHADOW_SHADER);
  auto content = FileUtils::getInstance()->getStringFromFile(shaderfile);
  auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, content.c_str());
  auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);

  for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
    mShadowDrawers[i] = DrawNodeEx::create();
    mShadowDrawers[i]->setGLProgramState(glProgramState);

    Size visibleSize = VisibleRect::getVisibleRect().size;
    Size visibleSizePixel = visibleSize * Director::getInstance()->getContentScaleFactor();

    mRenderTextures[i] = RenderTexture::create(visibleSize.width * mWidth,
                                               visibleSize.height,
                                               Texture2D::PixelFormat::RGBA8888);
    mRenderTextures[i]->setVirtualViewport(Vec2(0, 0),
                                           Rect(0,
                                                0,
                                                visibleSize.width,
                                                visibleSize.height),
                                           Rect(0,
                                                0,
                                                visibleSizePixel.width * mWidth,
                                                visibleSizePixel.height));

    if (GameLevel::instance().getNumShadowGroup() == 1) {
      mRenderTextures[i]->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    } else {
      mRenderTextures[i]->getSprite()->setPosition((mPosX - 0.375f) * visibleSize.width, 0);
    }
    mRenderTextures[i]->getSprite()->setOpacity(255 * mShadowDarkness);
    mRenderTextures[i]->setCameraMask((unsigned short) CameraFlag::USER2);
    parentNode->addChild(mRenderTextures[i]);

    mRenderTextures[i]->retain();
    mShadowDrawers[i]->retain();
  }
}

void ShadowManager::reset() {
  mLightPos = mOriginLightPos;
}

void ShadowManager::update(float dt) {
  std::vector<V2F_C4B_T2F_Triangle> triangles[NUM_SHADOW_LAYERS];

  GameLevel::instance().traverseObjects([&](GameObject* block){
    updateBlock(block, triangles[block->getRenderer()->getShadowLayer()]);
  }, true);

  auto cameraPos = GameLevel::instance().getGameLayer()->getCamera()->getPosition();
  for (int i = 0; i < NUM_SHADOW_LAYERS; ++i) {
#if EDITOR_MODE
    mRenderTextures[i]->setPosition(cameraPos - Vec2(0, UI_LAYER_HIGHT / 2));
#else
    mRenderTextures[i]->setPosition(cameraPos);
#endif

    mShadowDrawers[i]->clear();
    if (!triangles[i].empty()) {
      mShadowDrawers[i]->drawTriangles(triangles[i]);
    }
    mRenderTextures[i]->beginWithClear(0, 0, 0, 0);
    mShadowDrawers[i]->visit();
    mRenderTextures[i]->end();
  }
}

void ShadowManager::addLightDirDegree(float delta) {
  mLightDirDegree += delta;
  updateLightDir();
}

void ShadowManager::updateLightDir() {
  if (mLightDirDegree < 0) {
    mLightDirDegree += 360;
  } else if (mLightDirDegree >= 360) {
    mLightDirDegree -= 360;
  }
  mLightDir = Vec2(1, 0);
  mLightDir.rotate(Vec2::ZERO, MATH_DEG_TO_RAD(-mLightDirDegree));
}

ShadowManager::ShadowEntry ShadowManager::getShadowEntry(const std::vector<cocos2d::Vec2> &pts,
                                                         const cocos2d::Vec2 &lightPos) {
  ShadowEntry ret;
  CC_ASSERT(pts.size() == 4);
  if (mLightType == LIGHT_POINT) {
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
  } else if (mLightType == LIGHT_DIR) {
    int degree = mLightDirDegree; // Floor to integer
    if (degree == 0) {
      ret.pt1 = pts[1];
      ret.pt2 = pts[3];
    } else if (degree > 0 && degree < 90) {
      ret.pt1 = pts[2];
      ret.pt2 = pts[1];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[0];
    } else if (degree == 90) {
      ret.pt1 = pts[2];
      ret.pt2 = pts[3];
    } else if (degree > 90 && degree < 180) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[3];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[1];
    } else if (degree == 180) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[2];
    } else if (degree > 180 && degree < 270) {
      ret.pt1 = pts[1];
      ret.pt2 = pts[2];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[3];
    } else if (degree == 270) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[1];
    } else if (degree > 270 && degree < 360) {
      ret.pt1 = pts[0];
      ret.pt2 = pts[3];
      ret.needMakeUp = true;
      ret.makeUpPt = pts[2];
    }
  }
  return ret;
}

void ShadowManager::updateBlock(GameObject *block,
                                std::vector <cocos2d::V2F_C4B_T2F_Triangle> &triangles,
                                bool clipX) {
  auto renderer = block->getRenderer();
  if (!renderer->isShadowEnabled() || !renderer->isVisible() || !block->isRemovable()) {
    return;
  }
  if (renderer->getBoundingBox().containsPoint(mLightPos)) {
    return;
  }
  if (renderer->getShadowGroup() != mShadowGroup) {
    return;
  }

  auto bounds = GameLevel::instance().getBounds();
  auto camera = GameLevel::instance().getGameLayer()->getCamera();
#if EDITOR_MODE
  auto camRelative = camera->getPosition() - VisibleRect::getVisibleRect().size / 2 -
    Vec2(0, UI_LAYER_HIGHT / 2);
#else
  auto camRelative = camera->getPosition() - VisibleRect::getVisibleRect().size / 2;
#endif

  auto lightPos = mLightPos - camRelative;

  std::vector<Vec2> pts;
  block->getRenderer()->getPointsForShadow(lightPos, pts);
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

  Size visibleSize = VisibleRect::getVisibleRect().size;
  float center = visibleSize.width / 2;
  Vec2 offset(-center - visibleSize.width * (mPosX - 0.5f), 0);

  V2F_C4B_T2F_Triangle t;
  t.a.vertices = entries.pt1 + offset;
  t.a.colors = colorBase;

  t.b.vertices = f1 + offset;
  t.b.colors = colorBase;

  t.c.vertices = f0 + offset;
  t.c.colors = colorBase;

  triangles.push_back(t);

  t.a.vertices = entries.pt1 + offset;
  t.a.colors = colorBase;

  t.b.vertices = entries.pt2 + offset;
  t.b.colors = colorBase;

  t.c.vertices = f1 + offset;
  t.c.colors = colorBase;

  triangles.push_back(t);

  if (entries.needMakeUp) {
    t.a.vertices = entries.pt1 + offset;
    t.a.colors = colorBase;

    t.b.vertices = entries.pt2 + offset;
    t.b.colors = colorBase;

    t.c.vertices = entries.makeUpPt + offset;
    t.c.colors = colorBase;
    
    triangles.push_back(t);
  }
}
