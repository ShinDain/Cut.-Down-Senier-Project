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
//
//#define GROUND_MODEL_NAME "20m_Epoxy_Ground"
//#define GROUND_MODEL_OBJECTPATH "WorldObject"
//#define GROUND_MODEL_TEXPATH "Epoxy_Ground"
//#define GROUND_MODEL_EXTENTS XMFLOAT3(0, 0, 0)
//#define GROUND_MODEL_TYPE Object_World
//#define GROUND_MODEL_COLLIDERTYPE Collider_None
//#define GROUND_MODEL_MASS 9999
//#define GROUND_MODEL_SCALE XMFLOAT3(10,10,10)
//#define GROUND_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define GROUND_MODEL_SHADOW true
//
//#define CARPET_MODEL_NAME "Carpet_5"
//#define CARPET_MODEL_OBJECTPATH "WorldObject"
//#define CARPET_MODEL_TEXPATH "Carpet"
//#define CARPET_MODEL_EXTENTS XMFLOAT3(0, 0, 0)
//#define CARPET_MODEL_TYPE Object_World
//#define CARPET_MODEL_COLLIDERTYPE Collider_None
//#define CARPET_MODEL_MASS 9999
//#define CARPET_MODEL_SCALE XMFLOAT3(10,10,10)
//#define CARPET_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define CARPET_MODEL_SHADOW true
//
//#define DRAWER_MODEL_NAME "Drawer_2"
//#define DRAWER_MODEL_OBJECTPATH "WorldObject"
//#define DRAWER_MODEL_TEXPATH "DrawerAndTable"
//#define DRAWER_MODEL_EXTENTS XMFLOAT3(0.4f, 0.45f, 0.45f)
//#define DRAWER_MODEL_TYPE Object_Movable
//#define DRAWER_MODEL_COLLIDERTYPE Collider_Box
//#define DRAWER_MODEL_MASS 10
//#define DRAWER_MODEL_SCALE XMFLOAT3(10,10,10)
//#define DRAWER_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define DRAWER_MODEL_SHADOW true
//
//#define MECHANICAL_ARM_MODEL_NAME "Mechanical_Arm_1"
//#define MECHANICAL_ARM_MODEL_OBJECTPATH "WorldObject"
//#define MECHANICAL_ARM_MODEL_TEXPATH "Mechanical_Arm"
//#define MECHANICAL_ARM_MODEL_EXTENTS XMFLOAT3(1.15f, 1.f, 1.f)
//#define MECHANICAL_ARM_MODEL_TYPE Object_World
//#define MECHANICAL_ARM_MODEL_COLLIDERTYPE Collider_Box
//#define MECHANICAL_ARM_MODEL_MASS 9999
//#define MECHANICAL_ARM_MODEL_SCALE XMFLOAT3(10,10,10)
//#define MECHANICAL_ARM_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define MECHANICAL_ARM_MODEL_SHADOW true
//
//#define OFFICE_CHAIR_MODEL_NAME "office_chair"
//#define OFFICE_CHAIR_MODEL_OBJECTPATH "WorldObject"
//#define OFFICE_CHAIR_MODEL_TEXPATH "Chair"
//#define OFFICE_CHAIR_MODEL_EXTENTS XMFLOAT3(0.4f, 0.65f, 0.4f)
//#define OFFICE_CHAIR_MODEL_TYPE Object_Movable
//#define OFFICE_CHAIR_MODEL_COLLIDERTYPE Collider_Box
//#define OFFICE_CHAIR_MODEL_MASS 3
//#define OFFICE_CHAIR_MODEL_SCALE XMFLOAT3(10,10,10)
//#define OFFICE_CHAIR_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define OFFICE_CHAIR_MODEL_SHADOW true
//
//#define DOOR_MODEL_NAME "SciFiDoor"
//#define DOOR_MODEL_OBJECTPATH "WorldObject"
//#define DOOR_MODEL_TEXPATH "SciFiDoor"
//#define DOOR_MODEL_EXTENTS XMFLOAT3(1.0f, 1.1f, 0.125f)
//#define DOOR_MODEL_TYPE Object_World
//#define DOOR_MODEL_COLLIDERTYPE Collider_Box
//#define DOOR_MODEL_MASS 9999
//#define DOOR_MODEL_SCALE XMFLOAT3(10,10,10)
//#define DOOR_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define DOOR_MODEL_SHADOW true
//
//#define SERVER_RACK_MODEL_NAME "Server_Rack"
//#define SERVER_RACK_MODEL_OBJECTPATH "WorldObject"
//#define SERVER_RACK_MODEL_TEXPATH "Server_Rack"
//#define SERVER_RACK_MODEL_EXTENTS XMFLOAT3(0.35f, 1.1f, 0.3f)
//#define SERVER_RACK_MODEL_TYPE Object_Movable
//#define SERVER_RACK_MODEL_COLLIDERTYPE Collider_Box
//#define SERVER_RACK_MODEL_MASS 40
//#define SERVER_RACK_MODEL_SCALE XMFLOAT3(10,10,10)
//#define SERVER_RACK_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define SERVER_RACK_MODEL_SHADOW true
//
//#define SHELF_CRATE_MODEL_NAME "Shelf_with_Crates"
//#define SHELF_CRATE_MODEL_OBJECTPATH "WorldObject"
//#define SHELF_CRATE_MODEL_TEXPATH "Shelf"
//#define SHELF_CRATE_MODEL_EXTENTS XMFLOAT3(0.4f, 1.1f, 1.4f)
//#define SHELF_CRATE_MODEL_TYPE Object_World
//#define SHELF_CRATE_MODEL_COLLIDERTYPE Collider_Box
//#define SHELF_CRATE_MODEL_MASS 9999
//#define SHELF_CRATE_MODEL_SCALE XMFLOAT3(10,10,10)
//#define SHELF_CRATE_MODEL_RENDERLAYER RenderLayer::Render_Static
//#define SHELF_CRATE_MODEL_SHADOW true
//
//#define SHELF_MODEL_NAME "Shelf_Without_Crates"
//#define SHELF_MODEL_OBJECTPATH "WorldObject"
//#define SHELF_MODEL_TEXPATH "Shelf"
//#define SHELF_MODEL_EXTENTS XMFLOAT3(0.4f, 1.1f, 1.4f)
//#define SHELF_MODEL_TYPE Object_World
//#define SHELF_MODEL_COLLIDERTYPE Collider_Box
//#define SHELF_MODEL_MASS 9999
//#define SHELF_MODEL_SCALE XMFLOAT3(10,10,10)
//#define SHELF_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define SHELF_MODEL_SHADOW true
//
//#define STOOL_MODEL_NAME "Stool"
//#define STOOL_MODEL_OBJECTPATH "WorldObject"
//#define STOOL_MODEL_TEXPATH "Chair"
//#define STOOL_MODEL_EXTENTS XMFLOAT3(0.2f, 0.5f, 0.2f)
//#define STOOL_MODEL_TYPE Object_Movable
//#define STOOL_MODEL_COLLIDERTYPE Collider_Box
//#define STOOL_MODEL_MASS 5
//#define STOOL_MODEL_SCALE XMFLOAT3(10,10,10)
//#define STOOL_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define STOOL_MODEL_SHADOW true
//
//#define TABLE_MODEL_NAME "Table_1"
//#define TABLE_MODEL_OBJECTPATH "WorldObject"
//#define TABLE_MODEL_TEXPATH "DrawerAndTable"
//#define TABLE_MODEL_EXTENTS XMFLOAT3(1.0f, 0.5f, 0.5f)
//#define TABLE_MODEL_TYPE Object_World
//#define TABLE_MODEL_COLLIDERTYPE Collider_Box
//#define TABLE_MODEL_MASS 9999
//#define TABLE_MODEL_SCALE XMFLOAT3(10,10,10)
//#define TABLE_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define TABLE_MODEL_SHADOW true
//
//#define TABLE_GLASS_MODEL_NAME "Table_6"
//#define TABLE_GLASS_MODEL_OBJECTPATH "WorldObject"
//#define TABLE_GLASS_MODEL_TEXPATH "DrawerAndTable"
//#define TABLE_GLASS_MODEL_EXTENTS XMFLOAT3(1.5f, 0.5f, 0.5f)
//#define TABLE_GLASS_MODEL_TYPE Object_World
//#define TABLE_GLASS_MODEL_COLLIDERTYPE Collider_Box
//#define TABLE_GLASS_MODEL_MASS 9999
//#define TABLE_GLASS_MODEL_SCALE XMFLOAT3(10,10,10)
//#define TABLE_GLASS_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define TABLE_GLASS_MODEL_SHADOW true
//
//#define TABLE_SET_MODEL_NAME "Table_Set_1"
//#define TABLE_SET_MODEL_OBJECTPATH "WorldObject"
//#define TABLE_SET_MODEL_TEXPATH "DrawerAndTable"
//#define TABLE_SET_MODEL_EXTENTS XMFLOAT3(1.5f, 0.5f, 0.5f)
//#define TABLE_SET_MODEL_TYPE Object_World
//#define TABLE_SET_MODEL_COLLIDERTYPE Collider_Box
//#define TABLE_SET_MODEL_MASS 9999
//#define TABLE_SET_MODEL_SCALE XMFLOAT3(10,10,10)
//#define TABLE_SET_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define TABLE_SET_MODEL_SHADOW true
//
//#define VASE_MODEL_NAME "Vase"
//#define VASE_MODEL_OBJECTPATH "WorldObject"
//#define VASE_MODEL_TEXPATH "Vase"
//#define VASE_MODEL_EXTENTS XMFLOAT3(0.2f, 0.5f, 0.2f)
//#define VASE_MODEL_TYPE Object_Movable
//#define VASE_MODEL_COLLIDERTYPE Collider_Box
//#define VASE_MODEL_MASS 5
//#define VASE_MODEL_SCALE XMFLOAT3(10,10,10)
//#define VASE_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define VASE_MODEL_SHADOW true
//
//#define WALL_MODEL_NAME "Wall_2"
//#define WALL_MODEL_OBJECTPATH "WorldObject"
//#define WALL_MODEL_TEXPATH "Wall"
//#define WALL_MODEL_EXTENTS XMFLOAT3(2.0f, 2.0f, 0.08f)
//#define WALL_MODEL_TYPE Object_World
//#define WALL_MODEL_COLLIDERTYPE Collider_Box
//#define WALL_MODEL_MASS 9999
//#define WALL_MODEL_SCALE XMFLOAT3(10,10,10)
//#define WALL_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define WALL_MODEL_SHADOW false
//
//#define WALL_WITH_WINDOWS_MODEL_NAME "Wall_With_Windows_Set_10"
//#define WALL_WITH_WINDOWS_MODEL_OBJECTPATH "WorldObject"
//#define WALL_WITH_WINDOWS_MODEL_TEXPATH "Wall"
//#define WALL_WITH_WINDOWS_MODEL_EXTENTS XMFLOAT3(1.9f, 1.2f, 0.125f)
//#define WALL_WITH_WINDOWS_MODEL_TYPE Object_World
//#define WALL_WITH_WINDOWS_MODEL_COLLIDERTYPE Collider_Box
//#define WALL_WITH_WINDOWS_MODEL_MASS 9999
//#define WALL_WITH_WINDOWS_MODEL_SCALE XMFLOAT3(10,10,10)
//#define WALL_WITH_WINDOWS_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define WALL_WITH_WINDOWS_MODEL_SHADOW false
//
//#define WALL_PILLAR_MODEL_NAME "Wall_Pillar_1"
//#define WALL_PILLAR_MODEL_OBJECTPATH "WorldObject"
//#define WALL_PILLAR_MODEL_TEXPATH "Wall"
//#define WALL_PILLAR_MODEL_EXTENTS XMFLOAT3(0.2f, 2.0f, 0.15f)
//#define WALL_PILLAR_MODEL_TYPE Object_World
//#define WALL_PILLAR_MODEL_COLLIDERTYPE Collider_None
//#define WALL_PILLAR_MODEL_MASS 9999
//#define WALL_PILLAR_MODEL_SCALE XMFLOAT3(10,10,10)
//#define WALL_PILLAR_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define WALL_PILLAR_MODEL_SHADOW false
//
//#define WALL_PILLAR_3_MODEL_NAME "Wall_Pillar_3"
//#define WALL_PILLAR_3_MODEL_OBJECTPATH "WorldObject"
//#define WALL_PILLAR_3_MODEL_TEXPATH "Wall"
//#define WALL_PILLAR_3_MODEL_EXTENTS XMFLOAT3(0.25f, 2.0f, 0.25f)
//#define WALL_PILLAR_3_MODEL_TYPE Object_World
//#define WALL_PILLAR_3_MODEL_COLLIDERTYPE Collider_None
//#define WALL_PILLAR_3_MODEL_MASS 9999
//#define WALL_PILLAR_3_MODEL_SCALE XMFLOAT3(10,10,10)
//#define WALL_PILLAR_3_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
//#define WALL_PILLAR_3_MODEL_SHADOW false

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
