//
//  JsonFormat.h
//  jumpproj
//
//  Created by Yanxing Wang on 1/15/16.
//
//

#ifndef JsonFormat_h
#define JsonFormat_h

#define MAP_AUTHOR "author"
#define MAP_TIME "time"

#define PALETTE_BACKGROUND "backgroundColor"
#define PALETTE_HERO "heroColor"
#define PALETTE_NORMAL "normalBlockColor"
#define PALETTE_DEATH "deathBlockColor"
#define PALETTE_DEATH_CIRCLE "deathCircleColor"
#define PALETTE_BUTTON "buttonColor"
#define PALETTE_PUSHABLE "pushableBlockColor"
#define PALETTE_PALETTE_ARRAY "palette"
#define PALETTE_PALETTE_ID "index"
#define PALETTE_PALETTE_COLOR "color"

#define LEVEL_PALETTE_FILE "paletteFile"
#define LEVEL_SPAWN_POS "spawnPosition"
#define LEVEL_BLOCK_ARRAY "blocks"

#define LEVEL_BLOCK_ID "id"
#define LEVEL_BLOCK_SIZE "size"
#define LEVEL_BLOCK_POS "position"
#define LEVEL_BLOCK_REMOVABLE "removable"
#define LEVEL_BLOCK_SHADOW_ENABLE "shadowEnable"
#define LEVEL_BLOCK_KIND "kind"
#define LEVEL_BLOCK_PALETTE_ID "paletteIndex"
#define LEVEL_BLOCK_CHILDREN "children"

#define RENDERER_SHADOW "shadowEnable"
#define RENDERER_SHADOW_LAYER "shadowLayer"
#define RENDERER_ROT_SPEED "rotatespeed"
#define RENDERER_FLIP_UV "flipUV"
#define RENDERER_TEXTURE "textureName"

#define SHADOW_GROUP "shadowGroup"
#define SHADOW_GROUP_POSX "posx"
#define SHADOW_GROUP_WIDTH "width"
#define SHADOW_GROUP_LIGHT_TYPE "lightType"
#define SHADOW_GROUP_LIGHT_DIR "lightDir"
#define SHADOW_GROUP_LIGHT_POS "lightPosition"
#define SHADOW_GROUP_LIGHT_MOV "lightMoving"
#define SHADOW_GROUP_LIGHT_DARKNESS "shadowDarkness"

#define GAME_SPRITES "sprites"
#define SPRITE_IMAGE "image"
#define SPRITE_SIZE "size"
#define SPRITE_POSITION "position"
#define SPRITE_ZORDER "zorder"
#define SPRITE_OPACITY "opacity"
#define SPRITE_COLOR "color"

#define GAME_FX "fx"

#define PHYSICS_EVENTS "triggerEvents"

#define PATH_SPEED "pathSpeed"
#define PATH_PINGPONG "pingpong"
#define PATH_PAUSE "pause"
#define PATH_WAIT_TIME "pathWaitTime"
#define PATH_ARRAY "pathes"
#define PATH_PT_POSITION "position"
#define PATH_PT_WAIT_TIME "waittime"
#define PATH_PT_WIDTH "width"
#define PATH_PT_HEIGHT "height"

#define BUTTON_DIR "direction"
#define BUTTON_CAN_RESTORE "canRestore"
#define BUTTON_PUSHED_EVENT "pushedEvent"
#define BUTTON_RESTORED_EVENT "restoredEvent"
#define BUTTON_PUSHING_EVENT "pushingEvent"

#endif /* JsonFormat_h */
