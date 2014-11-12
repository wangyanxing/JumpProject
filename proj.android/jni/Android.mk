LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
				   ../../Classes/Blocks.cpp \
				   ../../Classes/Button.cpp \
				   ../../Classes/DrawNodeEx.cpp \
				   ../../Classes/EditorScene.cpp \
				   ../../Classes/EffectSprite.cpp \
				   ../../Classes/Events.cpp \
				   ../../Classes/GameScene.cpp \
				   ../../Classes/GameUtils.cpp \
				   ../../Classes/HttpHelper.cpp \
				   ../../Classes/LevelLayer.cpp \
				   ../../Classes/LevelScene.cpp \
				   ../../Classes/LevelSelector.cpp \
				   ../../Classes/LightBeam.cpp \
				   ../../Classes/LogicManager.cpp \
				   ../../Classes/MapSerial.cpp \
				   ../../Classes/Path.cpp \
				   ../../Classes/PathLib.cpp \
				   ../../Classes/Rotator.cpp \
				   ../../Classes/ShaderLayer.cpp \
				   ../../Classes/Shadows.cpp \
				   ../../Classes/Shake.cpp \
				   ../../Classes/SpriteSoft.cpp \
				   ../../Classes/SpriteUV.cpp \
				   ../../Classes/UIColorEditor.cpp \
				   ../../Classes/UILayer.cpp \
				   ../../Classes/VisibleRect.cpp \
				   ../../Classes/WaveEffect.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/external
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/cocos
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/cocos/ui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/cocos/platform/android
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/external/chipmunk/include/chipmunk
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/extensions
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../cocos2d/cocos/network


LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_ui_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
$(call import-module,network)
$(call import-module,extensions)
