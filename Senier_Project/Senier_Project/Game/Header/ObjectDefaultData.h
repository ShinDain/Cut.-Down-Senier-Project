#ifndef OBJECTDATA_H

#define OBJECTDATA_H

#include "../../Common/Header/D3DUtil.h"
#include "../../Common/Header/Struct.h"
#include "../../DirectXRendering/Header/Global.h"

#include "../../DataDefine/CharacterData.h"
#include "../../DataDefine/EffectData.h"
#include "../../DataDefine/ItemAndEventData.h"
#include "../../DataDefine/ProjectileData.h"
#include "../../DataDefine/OutSideObjectData.h"
#include "../../DataDefine/InsideObjectData.h"
#include "../../DataDefine/DungeonObjectData.h"

using namespace DirectX;

#define WEAPON_MODEL_NAME "Katana"
#define WEAPON_MODEL_OBJECTPATH ""
#define WEAPON_MODEL_TEXPATH "Katana"
#define WEAPON_MODEL_EXTENTS XMFLOAT3(0.3f, 3.2f, 1)
#define WEAPON_MODEL_TYPE Object_Weapon
#define WEAPON_MODEL_COLLIDERTYPE Collider_Box
#define WEAPON_MODEL_MASS 9999
#define WEAPON_MODEL_SCALE XMFLOAT3(0.3f, 0.3f, 0.3f)
#define WEAPON_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define WEAPON_MODEL_SHADOW true

#define CUBE_MODEL_NAME "Cube"
#define CUBE_MODEL_OBJECTPATH "WorldObject"
#define CUBE_MODEL_TEXPATH ""
#define CUBE_MODEL_EXTENTS XMFLOAT3(0.5f, 0.5f, 0.5f)
#define CUBE_MODEL_TYPE Object_Movable
#define CUBE_MODEL_COLLIDERTYPE Collider_Box
#define CUBE_MODEL_MASS 10
#define CUBE_MODEL_SCALE XMFLOAT3(1,1,1)
#define CUBE_MODEL_RENDERLAYER RenderLayer::Render_Static
#define CUBE_MODEL_SHADOW true

#define JUST_MODEL_NAME "Just"
#define JUST_MODEL_OBJECTPATH "WorldObject"
#define JUST_MODEL_TEXPATH ""
#define JUST_MODEL_EXTENTS XMFLOAT3(1.5f, 0.1f, 5.0f)
#define JUST_MODEL_TYPE Object_World
#define JUST_MODEL_COLLIDERTYPE Collider_Box
#define JUST_MODEL_MASS 9999
#define JUST_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define JUST_MODEL_RENDERLAYER RenderLayer::Render_Static
#define JUST_MODEL_SHADOW false

#define PLATFORM_MODEL_NAME "TmpPlatform"
#define PLATFORM_MODEL_OBJECTPATH ""
#define PLATFORM_MODEL_TEXPATH ""
#define PLATFORM_MODEL_EXTENTS XMFLOAT3(0.5f, 0.5f, 0.5f)
#define PLATFORM_MODEL_TYPE Object_World
#define PLATFORM_MODEL_COLLIDERTYPE Collider_Box
#define PLATFORM_MODEL_MASS 9999
#define PLATFORM_MODEL_SCALE XMFLOAT3(10,10,10)
#define PLATFORM_MODEL_RENDERLAYER RenderLayer::Render_Static
#define PLATFORM_MODEL_SHADOW true

void CreateObjectDefaultData();

#endif // !OBJECTDATA_H
