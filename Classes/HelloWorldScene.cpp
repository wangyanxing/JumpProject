#include "HelloWorldScene.h"
#include "Defines.h"

#if EDITOR_MODE
#   include "EditorScene.h"
#   include "UILayer.h"
#else
#   include "GameScene.h"
#endif

#if 0
#include "VisibleRect.h"
#include "cocos-ext.h"
USING_NS_CC_EXT;
#endif

USING_NS_CC;

HelloWorld::~HelloWorld() {
}

Scene* HelloWorld::createScene()
{
    auto scene = Scene::createWithPhysics();
    
    scene->getPhysicsWorld()->setGravity(Vec2(0,-1000));
    
#if EDITOR_MODE
    auto layer = LayerColor::create(Color4B(0x1E,0xB5,0xC7,0xFF));
    scene->addChild(layer);
    
    auto uiLayer = new UILayer();
    uiLayer->init(scene);
    
    auto EditorScene = EditorScene::create();
    layer->addChild(EditorScene);
#else
    auto game = GameScene::create();
    scene->addChild(game);
#endif
    
    return scene;
}
