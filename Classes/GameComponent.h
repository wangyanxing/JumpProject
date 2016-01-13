//
//  GameComponent.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/12/16.
//
//

#ifndef GameComponent_h
#define GameComponent_h

class GameComponent {
public:
  GameComponent();

  virtual ~GameComponent();

  virtual void update(float dt) = 0;
};

#endif /* GameComponent_h */
