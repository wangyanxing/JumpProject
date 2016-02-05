//
//  Compoment.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/13/16.
//
//

#ifndef Component_h
#define Component_h

#include "Prerequisites.h"
#include "Parameter.h"
#include "JsonParser.h"
#include "JsonWriter.h"

#define DECLARE_COMP_TYPE(t) \
  static ComponentType getType() {return t;} \
  ComponentType getComponentType() const override {return t;}

/**
 * Game component, can be dynamically added or removed into an object.
 */
class GameComponent {
public:
  GameComponent(GameObject *parent): mParent(parent) {}

  virtual ~GameComponent() {}

  virtual ComponentType getComponentType() const = 0;

  virtual void update(float dt) = 0;
  
  virtual void beforeRender(float dt) {}

  virtual void updateHelpers() {}

  virtual void initHelpers() {}

  GameObject *getParent() {
    return mParent;
  }

  virtual void load(JsonValueT &json) {}

  virtual void save(JsWriter &writer) {}

  virtual void clone(GameComponent *other) {}

  virtual void reset() {
    setEnabled(true);
  }

  virtual void runCommand(ComponentCommand type, const Parameter &param) {}

  virtual void setEnabled(bool enabled) {
    mEnable = enabled;
  }

  bool isEnabled() const {
    return mEnable;
  }

  virtual bool forceUpdate() const {
    return false;
  }

protected:
  GameObject *mParent{nullptr};

  bool mEnable{true};
};

#endif /* Component_h */
