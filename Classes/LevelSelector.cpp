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
#include "WaveEffect.h"

USING_NS_CC;

static const char* getLevelSuffix() {
    static std::string suffix;
    
    auto framesize = VisibleRect::getFrameSize();
    float ratio = framesize.width / framesize.height;
    
    if(ratio > 1.7) { // wide
        // ok, do nothing
        suffix = "";
    } else if(ratio < 1.4) { // ipad
        suffix = "_pad";
    } else { //ip4
        suffix = "_ip4";
    }
    
    return suffix.c_str();
}

LevelSelLayer::~LevelSelLayer() {
    cleanBlocks();
    delete mShadows;
    mShadows = nullptr;
    
    delete mWaveFx;
    mWaveFx = nullptr;
    
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
    
    auto block = ui::Button::create("images/rect.png");
    block->setScale(size.width / ORG_RECT_SIZE, size.height / ORG_RECT_SIZE);
    block->setContentSize(Size(ORG_RECT_SIZE,ORG_RECT_SIZE));
    block->addTouchEventListener([&](Ref *pSender, ui::Widget::TouchEventType type){
        if(type == ui::Widget::TouchEventType::ENDED){
            Node* n = (Node*)(pSender);
            if(mWorld == WORLD_1 || mWorld == WORLD_2) {
                int levelID = n->getTag();
                char levelName[256];
                sprintf(levelName, "maps/remote/w%d_%03d%s.json",mWorld,levelID,getLevelSuffix());
                bool absPath = false;
                
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                char newlevelName[256];
                sprintf(newlevelName, "w%d_%03d%s.json",mWorld,levelID,getLevelSuffix());
                
                auto p = FileUtils::getInstance()->getWritablePath();
                p += newlevelName;
                
                if(FileUtils::getInstance()->isFileExist(p)) {
                    strcpy(levelName, p.c_str());
                    absPath = true;
                }
#endif
                
                auto s = GameScene::createScene();
                GameScene::Scene->enterGame(levelName,absPath);
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
    title->setPosition(VisibleRect::center().x, VisibleRect::center().y + 130);
    
    auto tapHelp = Label::createWithSystemFont("TAP TO PLAY", "HelveticaNeue-Light", 40,
                                             Size::ZERO, TextHAlignment::CENTER,TextVAlignment::CENTER);
    addChild(tapHelp,10);
    tapHelp->setPosition(VisibleRect::center().x, VisibleRect::center().y - 160);
    
    setBackGradientColor(Color3B(50,201,219), Color3B(30,181,199));
    setBackGradientCenter(Vec2(510.625,287.641));
    
    mWaveFx = new WaveEffect(this);
    mWaveFx->newLine();
    
    auto& l2 = mWaveFx->newLine();
    l2.phase = 1.1;
    l2.height = 10;
    l2.angleBias = 1.1;
    
#if 0
    auto& l3 = mWaveFx->newLine();
    l3.phase = 1.3;
    l3.height = 8;
    l3.angleBias = 0.4;
#endif
}

void LevelSelLayer::initAsWorld1() {
    mTapToMove = false;
    Vec2 pts[16] = {
        {482.121f,481.758f},
        {618.0f,459.91f},
        {736.621f,406.059f},
        {822.777f,315.379f},
        {823.383f,207.66f},
        {731.27f,120.188f},
        {619.511f,72.7305f},
        {483.051f,54.9687f},
        {343.66f,76.6016f},
        {222.125f,120.808f},
        {135.863f,210.914f},
        {135.019f,317.911f},
        {219.613f,407.504f},
        {337.211f,464.145f},
        {381.742f,321.027f},
        {568.012f,200.77f}
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
        {482.121f,481.758f},
        {618.0f,  459.91f},
        {736.621f,406.059f},
        {822.777f,315.379f},
        {823.383f,207.66f},
        {731.27f, 120.188f},
        {619.511f,72.7305f},
        {483.051f,54.9687f},
        {343.66f, 76.6016f},
        {222.125f,120.808f},
        {135.863f,210.914f},
        {135.019f,317.911f},
        {219.613f,407.504f},
        {337.211f,464.145f},
        {381.742f,321.027f},
        {568.012f,200.77f}
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
    
    if(mWaveFx) {
        mWaveFx->update(dt);
    }
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
