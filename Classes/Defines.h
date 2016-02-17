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

#define GAME_DEBUG_MODE 1

#define EDITOR_IPAD_MODE 1
#define EDITOR_IP5_MODE  2
#define EDITOR_IP4_MODE  3

#define EDITOR_RATIO EDITOR_IP5_MODE

// UI and graphics params
#define ORG_RECT_SIZE 8.0
#define UI_LAYER_HIGHT 100
#define EDT_UI_YBIAS (VisibleRect::getFrameSize().height - UI_LAYER_HIGHT)

// Border ids
#define BORDER_BLOCK_LEFT 3
#define BORDER_BLOCK_RIGHT 4
#define BORDER_BLOCK_TOP 2
#define BORDER_BLOCK_BOTTOM 1
#define BORDER_FRAME_SIZE 10

// Curtain effect
#define CURTAIN_MOVE_TIME 0.8

// Control pad
#if !EDITOR_MODE
# define CONTROL_BUTTON_WIDTH 300
# define CONTROL_BUTTON_HEIGHT 200
# define CONTROL_BUTTON_OPACITY 80
# define CONTROL_BUTTON_COLOR Color3B(200, 200, 200)
#endif

// Art resource
#define EXIT_IMAGE "images/exit.png"
#define BLOCK_IMAGE "images/rect.png"
#define DEATH_IMAGE "images/saw.png"
#define DEATH_CIRCLE_IMAGE "images/saw3.png"
#define DEFAULT_SHADOW_SHADER "shaders/normal_shadow.fsh"
#define DIE_FX_NAME "fx/diefx.plist"

// Block default attributes
#define DEFAULT_SHADOW_LENGTH 100.0f
#define DEFAULT_ROTATE_SPEED 0
#define DEFAULT_DEATH_ROTATOR_SPEED 80
#define DEFAULT_BLOCK_TEXTURE BLOCK_IMAGE
#define DEFAULT_PALETTE_INDEX -1

// Z orders
#define ZORDER_BACK 0
#define ZORDER_BLOCK_ROOT 10

#define ZORDER_SHADOW 5
#define ZORDER_PARTICLE 15
#define ZORDER_DIE_FX 18
#define ZORDER_BLOCK 20
#define ZORDER_CURTAIN 30

#define ZORDER_EDT_HELPER_EDITOR 15
#define ZORDER_EDT_OBJ_HELPER 30
#define ZORDER_EDT_OBJ_SELECTION 35
#define ZORDER_EDT_GRID 10
#define ZORDER_EDT_HELPER_PHYSICS 10
#define ZORDER_EDT_HELPER_PATH 12
#define ZORDER_EDT_HELPER_LABEL 20

#define ZORDER_GAME_CONTROLPAD 100
#define ZORDER_GAME_PAUSELAYER 200

#define DEFAULT_COLOR_ID -1
#define UPDATE_DT 1.0f / 60.0f

#endif /* defined(__JumpEdt__Defines__) */
