//
//  LevelScene.cpp
//  JumpEdt
//
//

#include "LevelScene.h"
#include "cocos-ext.h"
#include "VisibleRect.h"
#include "SimpleAudioEngine.h"
#include "PathLib.h"
#include "GameScene.h"
#include "GameLevel.h"
#include "HttpHelper.h"

#if !EDITOR_MODE

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;
using namespace CocosDenshion;

LevelScene *LevelScene::instance = nullptr;

LevelScene *LevelScene::getInstance() {
  return instance;
}

void LevelScene::onEnter() {
  Scene::onEnter();
}

bool LevelScene::init() {
  if (!Scene::init()) {
    return false;
  }
  instance = this;

  HttpHelper::getAllMaps();

  Size size = Director::getInstance()->getWinSize();
  Director::getInstance()->setClearColor(Color4F::WHITE);

  auto p = FileUtils::getInstance()->getWritablePath();
  auto files = PathLib::listFiles(p.c_str(), ".json");

  PageView *pageView = PageView::create();
  pageView->setTouchEnabled(true);
  pageView->setSize(size);
  pageView->removeAllPages();
  pageView->setCustomScrollThreshold(50);

  float buttonWidth = 240, buttonHeight = 60.5f;
  int column = 4, row = 9;
  int pageCount = (int) files.size() / (column * row) + 1;
  if ((int) files.size() % (column * row) == 0) {
    --pageCount;
  }
  int count = 0;

  for (int i = 0; i < pageCount; ++i) {
    HBox *outerBox = HBox::create();
    outerBox->setContentSize(size);

    for (int k = 0; k < column; ++k) {
      VBox *innerBox = VBox::create();

      for (int j = 0; j < row; j++) {
        Button *btn = Button::create("images/button.png",
                                     "images/buttonHighlighted.png");
        btn->setScale9Enabled(true);
        btn->setContentSize(Size(buttonWidth, buttonHeight));

        if (count < files.size()) {
          std::string fileName = files[count++];
          btn->setName(fileName);
          btn->setTitleText(fileName.substr(0, fileName.size() - 5));
        } else {
          btn->setEnabled(false);
        }
        btn->setTitleFontSize(20);
        btn->addTouchEventListener(CC_CALLBACK_2(LevelScene::touchEvent, this));
        innerBox->addChild(btn);
      }

      LinearLayoutParameter *parameter = LinearLayoutParameter::create();
      parameter->setMargin(Margin(0, 0, buttonWidth, 0));
      innerBox->setLayoutParameter(parameter);

      outerBox->addChild(innerBox);

    }
    pageView->insertPage(outerBox, i);
  }

  addChild(pageView);
  return true;
}

void LevelScene::touchEvent(Ref *pSender, Widget::TouchEventType type) {
  if (type == Widget::TouchEventType::ENDED) {
    Node *n = (Node * )(pSender);
    auto levelName = n->getName();
    CCLOG("Loading scene: %s", levelName.c_str());

    auto path = FileUtils::getInstance()->getWritablePath() + levelName;
    if (!FileUtils::getInstance()->isFileExist(path)) {
      CCLOGWARN("Scene not existed!");
      return;
    }

    auto scene = Scene::create();
    auto game = GameScene::create();
    scene->addChild(game);
    game->enterGame(path, true);
    Director::getInstance()->replaceScene(scene);
  }
}

#endif
