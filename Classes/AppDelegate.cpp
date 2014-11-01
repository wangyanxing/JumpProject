#include "AppDelegate.h"
#include "Defines.h"

#if EDITOR_MODE
#   include "EditorScene.h"
#   include "UILayer.h"
#else
#   include "GameScene.h"
#   include "LevelScene.h"
#endif

USING_NS_CC;

Scene* createScene()
{
#if EDITOR_MODE == 0
    Director::getInstance()->getOpenGLView()->setDesignResolutionSize(960, 960/1.7778f, ResolutionPolicy::EXACT_FIT);
#endif
    
#if EDITOR_MODE
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Vec2(0,-1000));
    
    auto layer = LayerColor::create(Color4B(0x1E,0xB5,0xC7,0xFF));
    scene->addChild(layer);
    
    auto uiLayer = new UILayer();
    uiLayer->init(scene);
    
    auto EditorScene = EditorScene::create();
    layer->addChild(EditorScene);
    
    return scene;
#else
    auto levels = LevelScene::create();
    levels->retain();
    return levels;
#endif
}

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        
        
#if EDITOR_MODE
#   if EDITOR_RATIO == EDITOR_IP5_MODE
        glview = GLView::create("Jump Editor - for iPhone 5/5s/6/6+");
#   elif EDITOR_RATIO == EDITOR_IP4_MODE
        glview = GLView::create("Jump Editor - for iPhone 4/4s");
#   elif EDITOR_RATIO == EDITOR_IPAD_MODE
        glview = GLView::create("Jump Editor - for iPad");
#   endif
#else
        glview = GLView::create("Jump Game");
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
    
    //glview->setFrameSize(1336, 540);


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
