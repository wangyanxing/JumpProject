//
//  LevelSelector.cpp
//  JumpEdt
//
//  Created by Yanxing Wang on 10/30/14.
//
//

#include "LevelSelector.h"
#include "Blocks.h"
#include "Shadows.h"
#include "LogicManager.h"
#include "GameUtils.h"
#include "VisibleRect.h"
#include "GameScene.h"
#include "CocosGUI.h"

USING_NS_CC;

LevelSelLayer::~LevelSelLayer() {
    cleanBlocks();
    delete mShadows;
    mShadows = nullptr;
    
    mBack->removeFromParent();
}

bool LevelSelLayer::init() {
    Layer::init();
    
    mShadows = new ShadowManager(this);
    
    mBack = GameUtils::createRect(VisibleRect::getVisibleRect(), Color3B(255,255,255));
    
    auto shaderfile = FileUtils::getInstance()->fullPathForFilename("shaders/back.fsh");
    
    // init shader
    GLchar * fragSource = (GLchar*)String::createWithContentsOfFile(shaderfile.c_str())->getCString();
    auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource);
    auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);
    
    float screenWidth = VisibleRect::getFrameSize().width;
    float screenHeight = VisibleRect::getFrameSize().height;
    
    mBack->setGLProgramState(glProgramState);
    glProgramState->setUniformVec4("data", Vec4(screenWidth, screenHeight, 0, 0));
    glProgramState->setUniformVec4("color", Vec4(50.0/255.0, 201.0/255.0,219.0/255.0, 0.4));
    glProgramState->setUniformVec4("colorDest", Vec4(30.0/255.0, 181.0/255.0,199.0/255.0, 0.4));
    
    addChild(mBack);
    
    auto listener1 = EventListenerTouchAllAtOnce::create();
    listener1->onTouchesBegan = [&](const std::vector<Touch*>& touches, Event* event){
        for(auto& t : touches) {
            onTouch(t->getLocation());
            break;
        }
    };
    listener1->onTouchesMoved = [&](const std::vector<Touch*>& touches, Event*){
        for(auto& t : touches) {
            onTouch(t->getLocation());
            break;
        }
    };
    listener1->onTouchesEnded = [&](const std::vector<Touch*>& touches, Event*){
        for(auto& t : touches) {
            onEndTouch(t->getLocation());
            break;
        }
    };
    listener1->onTouchesCancelled = [&](const std::vector<Touch*>& touches, Event*){
        for(auto& t : touches) {
            onEndTouch(t->getLocation());
            break;
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    return true;
}

void LevelSelLayer::initWorld(WorldID world) {
    mWorld = world;
    
    if(world == TITLE)
        initAsTitle();
    else if(world == WORLD_1)
        initAsWorld1();
    else if(world == WORLD_2)
        initAsWorld2();
}

void LevelSelLayer::createBlock(int id, const cocos2d::Vec2& pt) {
    auto size = Size(65,55);
    //auto block = GameUtils::createRect(r, Color3B::WHITE);
    
    auto block = ui::Button::create("images/rect.png");
    block->setScale(size.width / ORG_RECT_SIZE, size.height / ORG_RECT_SIZE);
    block->setContentSize(Size(ORG_RECT_SIZE,ORG_RECT_SIZE));
    block->addTouchEventListener([&](Ref *pSender, ui::Widget::TouchEventType type){
        if(type == ui::Widget::TouchEventType::ENDED){
            Node* n = (Node*)(pSender);
            if(mWorld == WORLD_1) {
                int levelID = n->getTag();
                char levelName[256];
                sprintf(levelName, "maps/remote/w1_%03d.json",levelID);
                
                auto s = GameScene::createScene();
                GameScene::Scene->enterGame(levelName);
                auto trans = TransitionFade::create(0.5, s);
                Director::getInstance()->replaceScene(trans);
            }
        }
    });
    
    block->setPosition(pt);
    block->setTag(id);
    addChild(block,10);
    mBlocks.push_back(block);
}

void LevelSelLayer::cleanBlocks() {
    for (auto b : mBlocks) {
        b->removeFromParent();
    }
    mBlocks.clear();
    mShadows->reset();
}

void LevelSelLayer::initAsTitle() {
    mTapToMove = true;
    
    auto title = Label::createWithSystemFont("JUMP FGDSB", "HelveticaNeue-Light", 110,
                                              Size::ZERO, TextHAlignment::CENTER,TextVAlignment::CENTER);
    addChild(title,10);
    title->setPosition(VisibleRect::center().x, VisibleRect::center().y + 100);
    
    auto tapHelp = Label::createWithSystemFont("TAP TO PLAY", "HelveticaNeue-Light", 40,
                                             Size::ZERO, TextHAlignment::CENTER,TextVAlignment::CENTER);
    addChild(tapHelp,10);
    tapHelp->setPosition(VisibleRect::center().x, VisibleRect::center().y - 140);
    
    setBackGradientColor(Color3B(50,201,219), Color3B(30,181,199));
    setBackGradientCenter(Vec2(510.625,287.641));
}

void LevelSelLayer::initAsWorld1() {
    mTapToMove = false;
    Vec2 pts[16] = {
        {482.121,481.758},
        {618,459.91},
        {736.621,406.059},
        {822.777,315.379},
        {823.383,207.66},
        {731.27,120.188},
        {619.511,72.7305},
        {483.051,54.9687},
        {343.66,76.6016},
        {222.125,120.808},
        {135.863,210.914},
        {135.019,317.911},
        {219.613,407.504},
        {337.211,464.145},
        {381.742,321.027},
        {568.012,200.77}
    };
    
    for(auto i = 0; i < 16; ++i) {
        createBlock(i+1,pts[i]);
    }
    
    mShadows->mLightPos = Vec2(509.891,285.473);
    mShadows->updateShaderParam(Vec2(510.625,287.641), Color3B(251,3,137), Color3B(173,3,58));
    
    setBackGradientColor(Color3B(251,3,137), Color3B(173,3,58));
    setBackGradientCenter(Vec2(510.625,287.641));
}

void LevelSelLayer::initAsWorld2() {
    mTapToMove = false;
    Vec2 pts[16] = {
        {482.121,481.758},
        {618,459.91},
        {736.621,406.059},
        {822.777,315.379},
        {823.383,207.66},
        {731.27,120.188},
        {619.511,72.7305},
        {483.051,54.9687},
        {343.66,76.6016},
        {222.125,120.808},
        {135.863,210.914},
        {135.019,317.911},
        {219.613,407.504},
        {337.211,464.145},
        {381.742,321.027},
        {568.012,200.77}
    };
    
    for(auto i = 0; i < 16; ++i) {
        createBlock(i+1,pts[i]);
    }
    
    mShadows->mLightPos = Vec2(509.891,285.473);
    mShadows->updateShaderParam(Vec2(510.625,287.641), Color3B(50,201,219), Color3B(30,181,199));
    
    setBackGradientColor(Color3B(50,201,219), Color3B(30,181,199));
    setBackGradientCenter(Vec2(510.625,287.641));
}

void LevelSelLayer::onEnter() {
    Layer::onEnter();
    
    getScheduler()->scheduleUpdate(this, -1, false);
}

void LevelSelLayer::update(float dt) {
    mShadows->updateNodes(dt, mBlocks, true);
    
}

void LevelSelLayer::setBackGradientColor(const cocos2d::Color3B& colorSrc,
                                     const cocos2d::Color3B& colorDst) {

    auto glProgramState = mBack->getGLProgramState();
    glProgramState->setUniformVec4("color", Vec4(colorSrc.r/255.0,
                                                 colorSrc.g/255.0,
                                                 colorSrc.b/255.0, 0.4));
    
    glProgramState->setUniformVec4("colorDest", Vec4(colorDst.r/255.0,
                                                 colorDst.g/255.0,
                                                 colorDst.b/255.0, 0.4));
}

void LevelSelLayer::setBackGradientCenter(const cocos2d::Vec2& pos) {
    Vec2 p = pos;
    p.x -= VisibleRect::center().x;
    p.x *= -1;
    p.x /= VisibleRect::center().x;
    
    p.y -= VisibleRect::center().y;
    p.y *= -1;
    p.y /= VisibleRect::center().x;
    
    float screenWidth = VisibleRect::getFrameSize().width;
    float screenHeight = VisibleRect::getFrameSize().height;
    
    mBack->getGLProgramState()->setUniformVec4("data",
                                               Vec4(screenWidth, screenHeight,
                                                    p.x, p.y));
}

void LevelSelLayer::onTouch(const cocos2d::Vec2& pos) {
    
}

void LevelSelLayer::onEndTouch(const cocos2d::Vec2& pos) {
}
