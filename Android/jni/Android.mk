### Get the local dir
THIS_DIR := $(call my-dir)

### build OGLES2 Tools lib
include $(THIS_DIR)/../../3rdParty/PowerVR-2.10/Tools/OGLES2/Build/Android/Android.mk

### local path changed because of include, modify it
LOCAL_PATH := $(THIS_DIR)/../../Source

### 3rdParty paths
3RDPARTY := $(LOCAL_PATH)/../3rdParty
PVRSDK := $(3RDPARTY)/PowerVR-2.10
GLM := $(3RDPARTY)/glm-0.9.3.3
GLSDK := $(3RDPARTY)/glsdk_0.4.2

### clear state
include $(CLEAR_VARS)

### name of resulting lib
LOCAL_MODULE := VTest

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := \
					VApp.cpp \
					VGlobal.cpp \
					VShader.cpp \
					VTexture.cpp \
					VTrajectory.cpp \
					VVertexBufferObject.cpp \
					VShadowMap.cpp \
					VShell.cpp \
					$(PVRSDK)/Shell/PVRShell.cpp \
					$(PVRSDK)/Shell/API/KEGL/PVRShellAPI.cpp \
					$(PVRSDK)/Shell/OS/Android/PVRShellOS.cpp \
					$(GLSDK)/glutil/source/MousePoles.cpp \
					$(GLSDK)/glutil/source/MatrixStack.cpp
LOCAL_C_INCLUDES :=	\
					$(PVRSDK)/Shell	\
					$(PVRSDK)/Shell/API/KEGL \
					$(PVRSDK)/Shell/OS/Android \
					$(PVRSDK)/Builds/OGLES2/Include \
					$(PVRSDK)/Tools \
					$(PVRSDK)/Tools/OGLES2 \
					$(GLM) \
					$(GLSDK)/glutil/include
LOCAL_CFLAGS := -DBUILD_OGLES2

### dependencies
LOCAL_LDLIBS := \
				-llog \
				-landroid \
				-lEGL \
				-lGLESv2
LOCAL_STATIC_LIBRARIES := \
						android_native_app_glue \
						ogles2tools 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
