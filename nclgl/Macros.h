#pragma once

////
//Shader variable names
////
//
//Common Matrix
#define NCLGL_SHADER_MODEL_MATRIX "modelMatrix"
#define NCLGL_SHADER_VIEW_MATRIX "viewMatrix"
#define NCLGL_SHADER_PROJECTION_MATRIX "projMatrix"
#define NCLGL_SHADER_TEXTURE_MATRIX "textureMatrix"
#define NCLGL_SHADER_SHADOW_MATRIX "shadowMatrix"
//Texture
#define NCLGL_SHADER_TEXTURE_TOUSE_APPENDIX "_use"
#define NCLGL_SHADER_TEXTURE_SCALE_APPENDIX "_texScale"
#define NCLGL_SHADER_TEXTURE_MATRIX_APPENDIX "_texMatrix"
//
//Light Data
//Light Global Setting
#define NCLGL_LIGHT_MAX_PERPIXEL 8
#define NCLGL_LIGHT_MAX_PERVERTEX 16
//Common Light Data
#define NCLGL_SHADER_LIGHT_COUNT "lightCount"
#define NCLGL_SHADER_LIGHT_TYPE "lights"
#define NCLGL_SHADER_LIGHT_COLOUR "lightColours"
#define NCLGL_SHADER_LIGHT_POSITIONS "lightPositions"
#define NCLGL_SHADER_LIGHT_POSITION "lightPos"
#define NCLGL_SHADER_LIGHT_PARAMETERS "lightParams"
#define NCLGL_SHADER_CAMERA_POSITION "cameraPos"
//Shadow data
#define NCLGL_SHADER_LIGHT_SHADOW_TO_CAST "castShadow"
#define NCLGL_SHADER_LIGHT_SHADOW_MATRIX "shadowMatrix"
#define NCLGL_SHADER_LIGHT_SHADOW_MATRICES "shadowMatrices"
#define NCLGL_SHADER_LIGHT_SHADOW_MAP "shadowTex"
#define NCLGL_SHADER_LIGHT_SHADOW_MAPS "shadowTexs"
#define NCLGL_SHADER_LIGHT_SHADOW_CUBEMAP "shadowCubeTex"
#define NCLGL_SHADER_LIGHT_SHADOW_CUBEMAPS "shadowCubeTexs"
//Instancing data
#define NCLGL_SHADER_INSTANCING_UB_CAMERA_DATA "CameraBlock"
#define NCLGL_SHADER_INSTANCING_UB_CAMERA_DATA_BP 2
#define NCLGL_SHADER_INSTANCING_UB_INSTANCE "InstanceBlock"
#define NCLGL_SHADER_INSTANCING_UB_INSTANCE_BP 1

////
//Texture slots
////
#define NCLGL_TEXTURESLOT_SHADOWMAP 30
#define NCLGL_TEXTURESLOT_SHADOWMAPS_START (30 - NCLGL_LIGHT_MAX_PERPIXEL)
#define NCLGL_TEXTURESLOT_SHADOWCUBEMAPS_START (30 - 2 * NCLGL_LIGHT_MAX_PERPIXEL)

////
//Const value
////
//Shadow
#define NCLGL_SHADOWMAP_SIZE 2048

////
//This part is here only because in OpenGL, a cube map cannot overwrite a 2d texture in the same slot. 
////
//Texture
#define NCLGL_CUBETEX_UNIT_BEGIN 10
#define NCLGL_CUBETEX_UNIT_LOAD 31