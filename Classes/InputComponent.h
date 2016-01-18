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

  void runCommand(ComponentCommand type, const Parameter &param) override;

  void pressLeft();

  void pressRight();

  void pressJump();
};

#endif /* InputComponent_h */
