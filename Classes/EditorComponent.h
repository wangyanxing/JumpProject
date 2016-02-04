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

private:
  cocos2d::DrawNode *mSelectionHelper{nullptr};
};

#endif /* EditorComponent_h */
