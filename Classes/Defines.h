//
//  Defines.h
//  JumpEdt
//
//  Created by Yanxing Wang on 10/18/14.
//
//

#ifndef __JumpEdt__Defines__
#define __JumpEdt__Defines__

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#   define EDITOR_MODE 1
#else
#   define EDITOR_MODE 0
#endif

#define USE_REFACTOR 1

#define GAME_DEBUG_MODE 1

#define EDITOR_IPAD_MODE 1
#define EDITOR_IP5_MODE  2
#define EDITOR_IP4_MODE  3

#define EDITOR_RATIO EDITOR_IP5_MODE

#define GLOW_NODE_TAG 2000

#define ORG_RECT_SIZE 8.0
#define ROTATION_SPEED 80

#define GRAVITY_VAL -1100
#define JUMP_VOL 450

enum FollowMode {
  F_CENTER,
  F_UP, F_DOWN, F_LEFT, F_RIGHT
};

// These macros are used for debug.
#if USE_REFACTOR
# define USE_SHADOW 0
#else
# define USE_SHADOW 1
#endif

#define USE_JUMP_EFFECT 1
#define USE_BACKGROUND 1

#define UI_LAYER_HIGHT 100

#define BORDER_BLOCK_LEFT 3
#define BORDER_BLOCK_RIGHT 4
#define BORDER_BLOCK_TOP 2
#define BORDER_BLOCK_BOTTOM 1
#define BORDER_FRAME_SIZE 10

#define CURTAIN_MOVE_TIME 0.8

#define EDT_UI_YBIAS (VisibleRect::getFrameSize().height - UI_LAYER_HIGHT)

#define GAME_CAMERA GameLogic::Game->getGameLayer()->getCamera()
#define GAME_LAYER GameLogic::Game->getGameLayer()

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

#define TEMPLATE_MAP "maps/blank_map.json"

// Block default attributes
#define DEFAULT_SHADOW_LENGTH 100.0f
#define DEFAULT_ROTATE_SPEED 0
#define DEFAULT_BLOCK_TEXTURE BLOCK_IMAGE
#define DEFAULT_PALETTE_INDEX -1

// Z order
#define ZORDER_BACK 0
#define ZORDER_BLOCK_ROOT 10

#define ZORDER_SHADOW_1 5
#define ZORDER_SHADOW_2 6
#define ZORDER_PARTICLE 15
#define ZORDER_DIE_FX 18
#define ZORDER_BLOCK 20
#define ZORDER_CURTAIN 30
#define ZORDER_PHYSICS_DEBUG 50

#define ZORDER_EDT_GRID 10
#define ZORDER_EDT_PATH_HELPER 30
#define ZORDER_EDT_BORDER 50
#define ZORDER_EDT_BUTTON_HELPER 35
#define ZORDER_EDT_ID_LABEL 100

#define ZORDER_EDT_OBJ_HELPER 30
#define ZORDER_EDT_HELPER_PHYSICS 10
#define ZORDER_EDT_HELPER_PATH 15
#define ZORDER_EDT_HELPER_LABEL 20

#define ZORDER_GAME_CONTROLPAD 100
#define ZORDER_GAME_PAUSELAYER 200

#define DEFAULT_COLOR_ID -1

#define UPDATE_DT 1.0f / 60.0f

#define DEFAULT_SHADOW_SHADER "shaders/normal_shadow.fsh"

#endif /* defined(__JumpEdt__Defines__) */
