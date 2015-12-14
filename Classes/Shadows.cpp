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

USING_NS_CC;

void colorMix(const Color4B& src, const Color4B& dst, float r, Color4B& out) {
  out.r = dst.r * r + src.r * (1 - r);
  out.g = dst.g * r + src.g * (1 - r);
  out.b = dst.b * r + src.b * (1 - r);
  out.a = dst.a * r + src.a * (1 - r);
}

ShadowManager::ShadowManager(cocos2d::Node* parentNode) {

  mRendererSoft = DrawNodeEx::create("images/rect.png");
  parentNode->addChild(mRendererSoft, 2);

  mRendererNormal = DrawNodeEx::create("images/rect.png");
  parentNode->addChild(mRendererNormal, 2);
  mRendererNormal->setBlendFunc(BlendFunc::DISABLE);

#if EDITOR_MODE
  auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/normal_shadow_editor.fsh");
#else
  auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/normal_shadow.fsh");
#endif

  // Load shaders
  auto shaderContent = FileUtils::getInstance()->getStringFromFile(shaderfile);
  auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_vert,
                                                 shaderContent.c_str());
  auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);

  float screenWidth = VisibleRect::getFrameSize().width;
  float screenHeight = VisibleRect::getFrameSize().height;

  mRendererNormal->setGLProgramState(glProgramState);
  glProgramState->setUniformVec4("data", Vec4(screenWidth, screenHeight, 0, 0));
  glProgramState->setUniformVec4("colorSrc", Vec4(50.0/255.0, 201.0/255.0,219.0/255.0, 0.4));
  glProgramState->setUniformVec4("colorDest", Vec4(30.0/255.0, 181.0/255.0,199.0/255.0, 0.4));

  mLightPos = VisibleRect::center();
  mLightPos.x = 300;
  mLightPos.y = VisibleRect::top().y - 10;

  mOriginLightPos = mLightPos;
}

ShadowManager::~ShadowManager() {
  mRendererSoft->removeFromParent();
  mRendererNormal->removeFromParent();
}

std::pair<Vec2, Vec2> ShadowManager::getShadowEntry(const std::vector<Vec2>& pts) {
  Vec2 minPt, maxPt;

  CC_ASSERT(pts.size() == 4);
  if(mLightPos.x >= pts[0].x && mLightPos.x <= pts[1].x) {
    if(mLightPos.y >= pts[0].y) {
      minPt = pts[0];
      maxPt = pts[1];
    } else if(mLightPos.y <= pts[2].y) {
      minPt = pts[2];
      maxPt = pts[3];
    }
  } else if(mLightPos.x < pts[0].x) {
    if(mLightPos.y >= pts[0].y) {
      minPt = pts[2];
      maxPt = pts[1];
    } else if(mLightPos.y <= pts[2].y) {
      minPt = pts[0];
      maxPt = pts[3];
    } else {
      minPt = pts[0];
      maxPt = pts[2];
    }
  } else if(mLightPos.x > pts[1].x) {
    if(mLightPos.y >= pts[1].y) {
      minPt = pts[0];
      maxPt = pts[3];
    } else if(mLightPos.y <= pts[3].y) {
      minPt = pts[1];
      maxPt = pts[2];
    } else {
      minPt = pts[1];
      maxPt = pts[3];
    }
  }

  return std::make_pair(minPt,maxPt);
}

void ShadowManager::updateShaderParam(const cocos2d::Vec2& center,
                                      const cocos2d::Color3B& colSrc,
                                      const cocos2d::Color3B& colDst) {
  Vec2 p = center;
  p.x -= VisibleRect::center().x;
  p.x *= -1;
  p.x /= VisibleRect::center().x;

  p.y -= VisibleRect::center().y;
  p.y *= -1;
  p.y /= VisibleRect::center().x;

  float screenWidth = VisibleRect::getFrameSize().width;
  float screenHeight = VisibleRect::getFrameSize().height;

  mRendererNormal->getGLProgramState()->setUniformVec4("data",
                                                       Vec4(screenWidth, screenHeight, p.x, p.y));

  mRendererNormal->getGLProgramState()->setUniformVec4("colorSrc", Vec4(colSrc.r/255.0,
                                                                        colSrc.g/255.0,
                                                                        colSrc.b/255.0, 0.4));
  mRendererNormal->getGLProgramState()->setUniformVec4("colorDest", Vec4(colDst.r/255.0,
                                                                         colDst.g/255.0,
                                                                         colDst.b/255.0, 0.4));
}

void ShadowManager::updateShaderParam() {
  updateShaderParam(GameLogic::Game->mGradientCenter,
                    GameLogic::Game->mGradientColorSrc,
                    GameLogic::Game->mGradientColorDst);
}

void ShadowManager::updateBlockNormal(BlockBase* block,
                                      std::vector<cocos2d::V2F_C4B_T2F_Triangle>& triangles,
                                      bool clipX) {

  if(!block->mCastShadow || !block->isVisible() || !block->mCanPickup)
    return;

  if(block->getSprite()->getBoundingBox().containsPoint(mLightPos))
    return;

  std::vector<Vec2> pts;
  block->getPointsForShadow(mLightPos, pts);

  auto entries = getShadowEntry(pts);

  Color4B colorBase = Color4B::BLACK;
  colorBase.r = 255 * (1-mShadowDarkness);
  colorBase.g = 255 * (1-mShadowDarkness);
  colorBase.b = 255 * (1-mShadowDarkness);

  const float LENGTH = 1500;

  Vec2 dir0 = entries.first - mLightPos;
  dir0.normalize();
  Vec2 dir1 = entries.second - mLightPos;
  dir1.normalize();

  Vec2 f0 = entries.first + dir0 * LENGTH;
  Vec2 f1 = entries.second + dir1 * LENGTH;

  if(clipX) {

    Vec2 leftupper(0, 2000);
    Vec2 leftlower(0, -1000);

    Vec2 rightupper(VisibleRect::right().x, 2000);
    Vec2 rightlower(VisibleRect::right().x, -1000);

    if(Vec2::isSegmentIntersect(leftlower, leftupper, entries.first, f0)) {
      f0 = Vec2::getIntersectPoint(leftlower, leftupper, entries.first, f0);
    }
    if(Vec2::isSegmentIntersect(leftlower, leftupper, entries.second, f1)) {
      f1 = Vec2::getIntersectPoint(leftlower, leftupper, entries.second, f1);
    }
    if(Vec2::isSegmentIntersect(rightlower, rightupper, entries.first, f0)) {
      f0 = Vec2::getIntersectPoint(rightlower, rightupper, entries.first, f0);
    }
    if(Vec2::isSegmentIntersect(rightlower, rightupper, entries.second, f1)) {
      f1 = Vec2::getIntersectPoint(rightlower, rightupper, entries.second, f1);
    }
  }

  V2F_C4B_T2F_Triangle t;
  t.a.vertices = entries.first;
  t.a.colors = colorBase;

  t.b.vertices = f1;
  t.b.colors = colorBase;

  t.c.vertices = f0;
  t.c.colors = colorBase;

  triangles.push_back(t);

  t.a.vertices = entries.first;
  t.a.colors = colorBase;

  t.b.vertices = entries.second;
  t.b.colors = colorBase;

  t.c.vertices = f1;
  t.c.colors = colorBase;

  triangles.push_back(t);
}

void ShadowManager::updateBlockSoft(BlockBase* block,
                                    std::vector<cocos2d::V2F_C4B_T2F_Triangle>& triangles,
                                    bool clipX) {
  Color4B colorBase = Color4B::BLACK;
  colorBase.a = 255 * mShadowDarkness;

  if(!block->mCastShadow || !block->isVisible() || !block->mCanPickup)
    return;

  if(block->getSprite()->getBoundingBox().containsPoint(mLightPos))
    return;

  std::vector<Vec2> pts;
  block->getPointsForShadow(mLightPos, pts);

  auto entries = getShadowEntry(pts);

  Vec2 pa0 = pts[0].getMidpoint(pts[2]);
  Vec2 pa1 = pts[1].getMidpoint(pts[3]);

  Vec2 pb0 = pts[0].getMidpoint(pts[1]);
  Vec2 pb1 = pts[2].getMidpoint(pts[3]);

  Vec2 OA = entries.first - mLightPos;
  Vec2 OB = entries.second - mLightPos;

  Vec2 midLine = OA + OB;
  midLine.normalize();
  Vec2 loneFar = mLightPos + midLine * 2000; // make it long enough

  bool intersetA = false;
  bool intersetB = false;
  Vec2 interA, interB;
  Vec2 intersection;

  if (Vec2::isSegmentIntersect(pa0, pa1, mLightPos, loneFar)) {
    intersetA = true;
    interA = Vec2::getIntersectPoint(pa0, pa1, mLightPos, loneFar);
  }
  if (Vec2::isSegmentIntersect(pb0, pb1, mLightPos, loneFar)) {
    intersetB = true;
    interB = Vec2::getIntersectPoint(pb0, pb1, mLightPos, loneFar);
  }
  if(intersetA && intersetB) {
    intersection = interA.getMidpoint(interB);
  } else if(!intersetA && !intersetB) {
    return;
  } else {
    intersection = intersetA ? interA : interB;
  }

  float OX = intersection.distance(mLightPos);

  const float PI = 3.14159265;
  float lenOA = OA.length();
  float lenOB = OB.length();

  float ag = midLine.getAngle(OA);
  if(ag < 0) {
    ag += PI;
  }
  if(ag > PI / 2) {
    ag = PI - ag;
  }
  float cosTheta = cos(ag);
  float lengthA = (OX + block->mShadowLength) / cosTheta - lenOA;
  float lengthB = (OX + block->mShadowLength) / cosTheta - lenOB;

  auto dir0 = OA/lenOA;
  auto dir1 = OB/lenOB;
  auto curPt0 = entries.first;
  auto curPt1 = entries.second;
  auto len0 = lengthA;
  auto len1 = lengthB;

  for (int i = 0; i < block->mShadowFadeSegments + 1; i++) {

    float curlen0 = (i==0) ? block->mShadowFadeRatio * len0 :
    (1.0-block->mShadowFadeRatio) * len0 / block->mShadowFadeSegments;

    float curlen1 = (i==0) ? block->mShadowFadeRatio * len1 :
    (1.0-block->mShadowFadeRatio) * len0 / block->mShadowFadeSegments;

    Color4B color0 = colorBase;
    Color4B color1 = colorBase;

    if(i > 0) {
      colorMix(colorBase, Color4B(0,0,0,0), (i-1.0) / block->mShadowFadeSegments, color0);
      colorMix(colorBase, Color4B(0,0,0,0), (i-1.0) / block->mShadowFadeSegments, color1);
    }

    auto pt0 = curPt0;
    auto pt1 = curPt1;

    curPt0 = curPt0 + dir0 * curlen0;
    curPt1 = curPt1 + dir1 * curlen1;

    V2F_C4B_T2F_Triangle t;
    t.a.vertices = pt0;
    t.a.colors = color0;

    t.b.vertices = curPt0;
    t.b.colors = color0;

    t.c.vertices = pt1;
    t.c.colors = color0;

    triangles.push_back(t);

    t.a.vertices = pt1;
    t.a.colors = color1;

    t.b.vertices = curPt0;
    t.b.colors = color1;

    t.c.vertices = curPt1;
    t.c.colors = color1;

    triangles.push_back(t);
  }
}

void ShadowManager::updateNodes(float dt, std::vector<cocos2d::Node*>& nodes, bool clipX) {
  mRendererSoft->clear();
  mRendererNormal->clear();

  std::vector<V2F_C4B_T2F_Triangle> triangles;

  mRendererNormal->setVisible(true);
  mRendererSoft->setVisible(false);

  for(auto block : nodes) {
    if(!block->isVisible()) {
      return;
    }

    if(block->getBoundingBox().containsPoint(mLightPos)) {
      return;
    }

    std::vector<Vec2> pts;
    auto size = block->getBoundingBox().size;
    auto p = block->getPosition();
    pts.resize(4);
    pts[0] = p + Vec2(-size.width/2,  size.height/2);
    pts[2] = p + Vec2(-size.width/2, -size.height/2);
    pts[1] = p + Vec2( size.width/2,  size.height/2);
    pts[3] = p + Vec2( size.width/2, -size.height/2);

    auto entries = getShadowEntry(pts);

    Color4B colorBase = Color4B::BLACK;
    colorBase.r = 255 * (1-mShadowDarkness);
    colorBase.g = 255 * (1-mShadowDarkness);
    colorBase.b = 255 * (1-mShadowDarkness);

    const float LENGTH = 1500;

    Vec2 dir0 = entries.first - mLightPos;
    dir0.normalize();
    Vec2 dir1 = entries.second - mLightPos;
    dir1.normalize();

    Vec2 f0 = entries.first + dir0 * LENGTH;
    Vec2 f1 = entries.second + dir1 * LENGTH;

    if(clipX) {
      Vec2 leftupper(0, 2000);
      Vec2 leftlower(0, -1000);

      Vec2 rightupper(VisibleRect::right().x, 2000);
      Vec2 rightlower(VisibleRect::right().x, -1000);

      if(Vec2::isSegmentIntersect(leftlower, leftupper, entries.first, f0)) {
        f0 = Vec2::getIntersectPoint(leftlower, leftupper, entries.first, f0);
      }
      if(Vec2::isSegmentIntersect(leftlower, leftupper, entries.second, f1)) {
        f1 = Vec2::getIntersectPoint(leftlower, leftupper, entries.second, f1);
      }
      if(Vec2::isSegmentIntersect(rightlower, rightupper, entries.first, f0)) {
        f0 = Vec2::getIntersectPoint(rightlower, rightupper, entries.first, f0);
      }
      if(Vec2::isSegmentIntersect(rightlower, rightupper, entries.second, f1)) {
        f1 = Vec2::getIntersectPoint(rightlower, rightupper, entries.second, f1);
      }
    }

    V2F_C4B_T2F_Triangle t;
    t.a.vertices = entries.first;
    t.a.colors = colorBase;

    t.b.vertices = f1;
    t.b.colors = colorBase;

    t.c.vertices = f0;
    t.c.colors = colorBase;

    triangles.push_back(t);

    t.a.vertices = entries.first;
    t.a.colors = colorBase;

    t.b.vertices = entries.second;
    t.b.colors = colorBase;

    t.c.vertices = f1;
    t.c.colors = colorBase;

    triangles.push_back(t);
  }

  if(!triangles.empty()) {
    mRendererNormal->drawTriangles(triangles);
  }
}

void ShadowManager::update(float dt) {

  mRendererSoft->clear();
  mRendererNormal->clear();

  std::vector<V2F_C4B_T2F_Triangle> triangles;

  if(mUseSoftShadow) {
    mRendererNormal->setVisible(false);
    mRendererSoft->setVisible(true);

    for(auto b : GameLogic::Game->mBlocks) {
      auto block = b.second;
      updateBlockSoft(block, triangles);
    }
    updateBlockSoft(GameLogic::Game->mHero, triangles);

    if(!triangles.empty())
      mRendererSoft->drawTriangles(triangles);

  } else {
    mRendererNormal->setVisible(true);
    mRendererSoft->setVisible(false);

    for(auto b : GameLogic::Game->mBlocks) {
      auto block = b.second;
      updateBlockNormal(block, triangles);
    }
    updateBlockNormal(GameLogic::Game->mHero, triangles);

    if(!triangles.empty())
      mRendererNormal->drawTriangles(triangles);
  }

  if(!mShadowMovingEnable || !GameLogic::Game->mGameMode) {
    return;
  }

  mLightMoveTimer += dt;

  bool lightLeft = mLightPos.x <= VisibleRect::center().x;
  bool heroLeft = GameLogic::Game->mHero->getPosition().x <= VisibleRect::center().x;

  float dis = std::abs(mOriginLightPos.x - VisibleRect::center().x);

  if(lightLeft == heroLeft && mLightMoveTimer > 10) {
    mLightMoveTimer = 0;

    mMoveTarget = heroLeft ? VisibleRect::center().x + dis : VisibleRect::center().x - dis;
    mMovingSpeed = (dis * 2) / 3.0f;
    if(!heroLeft) mMovingSpeed*=-1;
    mMoving = true;
  }

  if(mMoving) {
    mLightPos.x += mMovingSpeed * dt;
    if(mMovingSpeed > 0) {
      if(mLightPos.x >= VisibleRect::center().x + dis) {
        mLightPos.x = VisibleRect::center().x + dis;
        mMoving = false;
      }
    } else {
      if(mLightPos.x <= VisibleRect::center().x - dis) {
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
