//
//  EditorComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/3/16.
//
//

#ifndef EditorComponent_h
#define EditorComponent_h

#include "Prerequisites.h"
#include "GameComponent.h"

#if EDITOR_MODE

class EditorComponent : public GameComponent {
public:
  EditorComponent(GameObject *parent);

  ~EditorComponent();

  DECLARE_COMP_TYPE(COMPONENT_EDITOR);

  void update(float dt) override;

  void beforeRender(float dt) override;

  void runCommand(ComponentCommand type, const Parameter &param) override;
  
  void initHelpers() override;

  void releaseHelpers();

  bool forceUpdate() const override {
    return true;
  }

private:
  void drawSelectionHelper();

private:
  cocos2d::DrawNode *mSelectionHelper{nullptr};

  bool mIsFirstSelection{false};
};

#endif
#endif /* EditorComponent_h */
