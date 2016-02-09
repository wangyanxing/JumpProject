//
//  GameLevel.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef GameLevel_h
#define GameLevel_h

#include "Prerequisites.h"
#include "Singleton.h"

class GameLevel {
  DECL_SINGLETON(GameLevel)

public:
  void init(GameLayerContainer *layer);

  void release();

  void update(float dt);
  
  void beforeRender(float dt);

  void updateCamera(cocos2d::Camera *cam, bool forceUpdate = false);

  void load(const std::string &levelFile);

  void save(const std::string &levelFile);

  void unload();

  void enableGame(bool enable);

  void die();

  void setHeroSpawnPosition(const cocos2d::Vec2 &pos);
  
  bool isGameEnabled() const {
    return mGameEnabled;
  }

  ObjectManager *getObjectManager() {
    return mObjectManager;
  }

  PhysicsManager *getPhysicsManager() {
    return mPhysicsManager;
  }

  ColorPalette *getPalette() {
    return mPalette;
  }

  GameLayerContainer *getGameLayer() {
    return mGameLayer;
  }

  int getNumShadowGroup() const {
    return mNumShadowGroup;
  }
  
  cocos2d::Rect getBounds() const {
    return mBounds;
  }

  std::string getCurrentLevelFile() const {
    return mCurrentLevelFile;
  }

  void reset();

  void traverseObjects(std::function<void(GameObject*)> func, bool containsHero);
  
  GameObject *getHero();

  ShadowManager *getShadowManager(int id) {
    return mShadows[id];
  }
  
private:

  void createHero(const cocos2d::Vec2 &pos);

  void updateBounds();
  
  void addShadowGroup();
  
  void initShadowGroup(int groupId);

  void dieImpl();

  void loadFx();

  void unloadFx();

  void setCurrentFile(const std::string &file);

private:
  ObjectManager *mObjectManager{nullptr};

  GameLayerContainer *mGameLayer{nullptr};

  PhysicsManager *mPhysicsManager{nullptr};

  ColorPalette *mPalette{nullptr};
  
  bool mGameEnabled{false};

  bool mDieFlag{false};

  int mNumShadowGroup{1};
  
  std::vector<ShadowManager*> mShadows;
  
  std::vector<cocos2d::Node*> mShadowNode;

  std::vector<GameSprite*> mSpriteList;

  std::vector<std::string> mEffects;

  std::vector<cocos2d::ParticleBatchNode*> mFxNodes;

  cocos2d::Vec2 mHeroSpawnPos;

  cocos2d::Rect mBounds;

  cocos2d::DrawNode *mBackground{nullptr};

  std::string mCurrentLevelFile;
};

#endif /* GameLevel_h */
