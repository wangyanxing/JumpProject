//
//  ButtonComponent.hpp
//  jumpproj
//
//  Created by Yanxing Wang on 1/20/16.
//
//

#ifndef ButtonComponent_h
#define ButtonComponent_h

#include "Prerequisites.h"
#include "GameComponent.h"

class ButtonComponent : public GameComponent {
public:
  DECLARE_COMP_TYPE(COMPONENT_BUTTON);
  
  ButtonComponent(GameObject *parent);
  
  virtual ~ButtonComponent();
  
  void update(float dt) override;
  
  void load(JsonValueT &json) override;
  
  void reset() override;
  
  void runCommand(ComponentCommand type, const Parameter &param) override;

private:
  cocos2d::Vec2 getDirectionVec();
  
  std::vector<std::string> mPushedEvents;
  
  std::vector<std::string> mRestoredEvents;
  
  std::vector<std::string> mPushingEvents;
  
  ButtonDirection mDirection{BUTTON_DOWN};
};

#endif /* ButtonComponent_h */
