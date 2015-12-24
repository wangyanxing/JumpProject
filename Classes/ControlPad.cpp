//
//  ControlPad.cpp
//  jumpproj
//
//  Created by Yanxing Wang on 12/23/15.
//
//

#include "ControlPad.h"

ControlPad::ControlPad() {
}

ControlPad::~ControlPad() {
  clearConfig();
}

void ControlPad::clearConfig() {
  for (auto it = mControlConfig.begin(); it != mControlConfig.end(); ++it) {
    delete (*it);
  }
  mControlConfig.clear();
}

ControlPadConfig *ControlPad::getControlConfig() {
  if (mSelectedConfig <= mControlConfig.size() - 1) {
    return mControlConfig.at(mSelectedConfig);
  }
  return nullptr;
}

ControlPad *ControlPad::controlPadConfig = new ControlPad();
