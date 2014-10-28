#include "HelloWorldScene.h"
#include "EditorScene.h"
#include "UILayer.h"

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
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    
    scene->getPhysicsWorld()->setGravity(Vec2(0,-1000));
    
    // 'layer' is an autorelease object
    auto layer = LayerColor::create(Color4B(0x1E,0xB5,0xC7,0xFF));

    // add layer as a child to scene
    scene->addChild(layer);
    
    auto uiLayer = new UILayer();
    uiLayer->init(scene);
    
#if EDITOR_MODE
    auto EditorScene = EditorScene::create();
    layer->addChild(EditorScene);
#endif
    
    
#if 0
    auto c = ControlColourPicker::create();
    c->setColor(Color3B(37, 46, 252));
    c->setPosition(VisibleRect::center().x, 50);
    c->setEnabled(true);
//    c->setScale(0.8);
    layer->addChild(c);
#endif

    // return the scene
    return scene;
}
