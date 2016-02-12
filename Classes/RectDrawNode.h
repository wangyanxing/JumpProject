//
//  RectDrawNode.h
//  jumpproj
//
//  Created by Yanxing Wang on 2/12/16.
//
//

#ifndef RectDrawNode_h
#define RectDrawNode_h

class RectDrawNode : public cocos2d::DrawNode {
public:
  static RectDrawNode* create(const cocos2d::Size &size, const cocos2d::Color3B& color);

  void setColor(const cocos2d::Color3B& color) override;

  void setSize(const cocos2d::Size& size);

private:
  void redraw();

  cocos2d::Size mSize;
};

#endif /* RectDrawNode_h */
