#include "AppDelegate.h"
#include "Defines.h"
#include "VisibleRect.h"
#include "LogicManager.h"

#if EDITOR_MODE
#   include "EditorScene.h"
#   include "UILayer.h"
#else
#   include "GameScene.h"
#   include "LevelScene.h"
#   include "ChooseWorldScene.h"
#endif

USING_NS_CC;

Scene *createScene() {
#if EDITOR_MODE == 0
  auto framesize = VisibleRect::getFrameSize();
  float ratio = framesize.width / framesize.height;
  float designRatio = 1;

  if(ratio > 1.7) { // wide
    designRatio = 1.7778f;
  } else if(ratio < 1.4) { // ipad
    designRatio = 1.3333f;
  } else { //ip4
    designRatio = 1.5f;
  }

  Director::getInstance()->getOpenGLView()->setDesignResolutionSize(960, 960 / designRatio,
                                                                    ResolutionPolicy::EXACT_FIT);
#endif

#if EDITOR_MODE
  auto scene = Scene::createWithPhysics();
  scene->getPhysicsWorld()->setGravity(Vec2(0, -200));
  GameLogic::PhysicsWorld = scene->getPhysicsWorld();
  scene->getPhysicsWorld()->setDebugDrawCameraMask((unsigned short) CameraFlag::USER2);

  auto layer = LayerColor::create(Color4B(0x1E, 0xB5, 0xC7, 0xFF));
  scene->addChild(layer);

  auto uiLayer = new UILayer();
  uiLayer->init(scene);

  auto EditorScene = EditorScene::create();
  layer->addChild(EditorScene);

  return scene;
#else

#if GAME_DEBUG_MODE
  auto levels = LevelScene::create();
  levels->retain();
  return levels;
#else
  auto scene = Scene::createWithPhysics();
  scene->getPhysicsWorld()->setGravity(Vec2(0, 0));
  GameLogic::PhysicsWorld = scene->getPhysicsWorld();
  scene->addChild(ChooseWorldScene::create());
  return scene;
#endif
#endif
}

AppDelegate::AppDelegate() {
  srand((unsigned) time(nullptr));
}

AppDelegate::~AppDelegate() {
}

void AppDelegate::initGLContextAttrs() {
  GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
  GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
  auto director = Director::getInstance();
  auto glview = director->getOpenGLView();

  if (!glview) {
#if EDITOR_MODE
#   if EDITOR_RATIO == EDITOR_IP5_MODE
    glview = GLViewImpl::create("Jump Editor - for iPhone 5/5s/6/6+");
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
    glview = GLViewImpl::create("Jump Editor - for iPhone 4/4s");
#   elif EDITOR_RATIO == EDITOR_IPAD_MODE
    glview = GLViewImpl::create("Jump Editor - for iPad");
#   endif
#else
    glview = GLViewImpl::create("Jump Game");
#endif

#if EDITOR_MODE
#   if EDITOR_RATIO == EDITOR_IP4_MODE
    glview->setFrameSize(960, 750);
#   elif EDITOR_RATIO == EDITOR_IPAD_MODE
    glview->setFrameSize(960, 850);
#   endif
#endif

    director->setOpenGLView(glview);
  }

  director->setAnimationInterval(1.0 / 60);
  director->runWithScene(createScene());

  return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
  Director::getInstance()->stopAnimation();

  // if you use SimpleAudioEngine, it must be pause
  // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
  Director::getInstance()->startAnimation();

  // if you use SimpleAudioEngine, it must resume here
  // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
