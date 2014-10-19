#include "HelloWorldScene.h"
#include "GameScene.h"
#include "UILayer.h"

USING_NS_CC;

HelloWorld::~HelloWorld() {
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    
    scene->getPhysicsWorld()->setGravity(Vec2(0,-1000));
    
    // 'layer' is an autorelease object
    auto layer = LayerColor::create(Color4B(0x1E,0xB5,0xC7,0xFF));

    // add layer as a child to scene
    scene->addChild(layer);
    
    auto uiLayer = new UILayer();
    uiLayer->init(scene);
    
    auto gameScene = GameScene::create();
    
    layer->addChild(gameScene);

    // return the scene
    return scene;
}
