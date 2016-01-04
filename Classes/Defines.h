//
//  Defines.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/18/14.
//
//

#ifndef __JumpEdt__Defines__
#define __JumpEdt__Defines__

#include "cocos2d.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#   define EDITOR_MODE 1
#else
#   define EDITOR_MODE 0
#endif

#define EDITOR_IPAD_MODE 1
#define EDITOR_IP5_MODE  2
#define EDITOR_IP4_MODE  3

#define EDITOR_RATIO EDITOR_IP5_MODE

#define GLOW_NODE_TAG 2000

#define ORG_RECT_SIZE 8.0
#define ROTATION_SPEED 80

#define GRAVITY_VAL -1100
#define JUMP_VOL 450

enum BlockKind {
  KIND_HERO = 0,
  KIND_BLOCK,
  KIND_DEATH,
  KIND_DEATH_CIRCLE,
  KIND_BUTTON,
  KIND_PUSHABLE,

  KIND_MAX
};

enum FollowMode {
  F_CENTER,
  F_UP, F_DOWN, F_LEFT, F_RIGHT
};

#define USE_SHADER_LAYER 0
#define USE_SHADOW 1
#define USE_JUMP_EFFECT 1
#define USE_BACKGROUND 1
#define USE_GRADIENT 0

#define UI_LAYER_HIGHT 100

#define BORDER_BLOCK_LEFT 3
#define BORDER_BLOCK_RIGHT 4
#define BORDER_BLOCK_TOP 2
#define BORDER_BLOCK_BOTTOM 1
#define BORDER_FRAME_SIZE 10

#define CURTAIN_MOVE_TIME 0.8

#define EDT_UI_YBIAS (VisibleRect::getFrameSize().height - UI_LAYER_HIGHT)

#if EDITOR_MODE
# define GAME_CAMERA EditorScene::Scene->getCamera()
#else
# define GAME_CAMERA GameScene::Scene->getCamera()
#endif

// Control pad
#if !EDITOR_MODE
# define CONTROL_BUTTON_WIDTH 300
# define CONTROL_BUTTON_HEIGHT 200
# define CONTROL_BUTTON_OPACITY 80
# define CONTROL_BUTTON_COLOR Color3B(200, 200, 200)
#endif

// Art resource
#define BLOCK_IMAGE "images/rect.png"
#define DEATH_IMAGE "images/saw.png"
#define DEATH_CIRCLE_IMAGE "images/saw3.png"
#define EXIT_IMAGE "images/exit.png"

// Block default attributes
#define DEFAULT_SHADOW_LENGTH 100
#define DEFAULT_ROTATE_SPEED 0
#define DEFAULT_BLOCK_TEXTURE BLOCK_IMAGE
#define DEFAULT_PALETTE_INDEX -1

// Z order
#define ZORDER_BACK 0
#define ZORDER_SHADOW_1 5
#define ZORDER_SHADOW_2 6
#define ZORDER_PARTICLE 15
#define ZORDER_PARTICLE_STAR 16
#define ZORDER_DIE_FX 18
#define ZORDER_BLOCK 20
#define ZORDER_CURTAIN 30

#define ZORDER_EDT_GRID 10
#define ZORDER_EDT_PATH_HELPER 30
#define ZORDER_EDT_BORDER 50
#define ZORDER_EDT_BUTTON_HELPER 35
#define ZORDER_EDT_ID_LABEL 100

#define ZORDER_GAME_CONTROLPAD 100

#endif /* defined(__JumpEdt__Defines__) */
