//
//  WaveEffect.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 11/2/14.
//
//

#include "WaveEffect.h"
#include "VisibleRect.h"

USING_NS_CC;

WaveEffect::WaveEffect(cocos2d::Node* parent) {
  mParentNode = parent;
  init(parent);
}

WaveEffect::~WaveEffect() {
  destory();
}

WaveEffect::WaveLine& WaveEffect::newLine() {
  mLines.push_back(WaveLine());

  mLines.back().node = DrawNodeEx::create("images/eff_line008.jpg");
  mLines.back().node->setBlendFunc(BlendFunc::ADDITIVE);
  mParentNode->addChild(mLines.back().node,50);

  return mLines.back();
}

void WaveEffect::updateLine(WaveEffect::WaveLine& line, float dt) {

  float screenWidth = VisibleRect::right().x;
  float screenHeight = VisibleRect::top().y;

  line.node->clear();
  float curW = 0;
  float twoPI = 3.14159265 * 2;

  std::vector<cocos2d::V2F_C4B_T2F_Triangle> triangles;
  triangles.reserve(500);

  Color4B colorBase = Color4B(200,200,200,255);

  while (true) {
    float nextW = curW + line.seg;
    if(nextW > screenWidth)
      break;

    float ratio0 = curW  / screenWidth;
    float ratio1 = nextW / screenWidth;
    float y0 = sin(ratio0 * twoPI + line.angleBias) * line.length + screenHeight / 2 + line.yBias;
    float y1 = sin(ratio1 * twoPI + line.angleBias) * line.length + screenHeight / 2 + line.yBias;
    float y01 = y0 - line.height;
    float y11 = y1 - line.height;
    float x0 = curW;
    float x1 = nextW;

    V2F_C4B_T2F_Triangle t;
    t.a.vertices.set(x0, y0);
    t.a.colors = colorBase;
    t.a.texCoords = Tex2F(0,ratio0);

    t.b.vertices.set(x0, y01);
    t.b.colors = colorBase;
    t.b.texCoords = Tex2F(1,ratio0);

    t.c.vertices.set(x1, y1);
    t.c.colors = colorBase;
    t.c.texCoords = Tex2F(0,ratio1);

    triangles.push_back(t);

    t.a.vertices.set(x0, y01);
    t.a.colors = colorBase;
    t.a.texCoords = Tex2F(1,ratio0);

    t.b.vertices.set(x1, y11);
    t.b.colors = colorBase;
    t.b.texCoords = Tex2F(1,ratio1);

    t.c.vertices.set(x1, y1);
    t.c.colors = colorBase;
    t.c.texCoords = Tex2F(0,ratio1);

    triangles.push_back(t);

    curW += line.seg;
  }

  line.node->drawTriangles(triangles);
  line.angleBias += dt * line.speed;
  if(line.angleBias > twoPI) {
    line.angleBias -= twoPI;
  }
}

void WaveEffect::update(float dt) {

  for(auto& l : mLines) {
    updateLine(l, dt);
  }
}

void WaveEffect::init(cocos2d::Node* parent) {
}

void WaveEffect::destory() {
  for(auto& l : mLines) {
    l.node->removeFromParent();
  }
}

