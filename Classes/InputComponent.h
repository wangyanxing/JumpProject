//
//  InputComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/18/16.
//
//

#ifndef InputComponent_h
#define InputComponent_h

#include "GameComponent.h"

/**
 * Only hero object can hold this component.
 */
class InputComponent : public GameComponent {
public:
  InputComponent(GameObject *parent);

  ~InputComponent();
  
  DECLARE_COMP_TYPE(COMPONENT_INPUT);
  
  void update(float dt) override {}

  void runCommand(ComponentCommand type, const Parameter &param) override;
};

#endif /* InputComponent_h */
