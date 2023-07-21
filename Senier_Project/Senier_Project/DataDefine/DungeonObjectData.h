#ifndef DUNGEONOBJECTDATA_H

#define DUNGEONOBJECTDATA_H

#include "../Common/Header/D3DUtil.h"
#include "../Common/Header/Struct.h"
#include "../DirectXRendering/Header/Global.h"

using namespace DirectX;

#define BB_1X1X1_MODEL_NAME "BB_1x1x1"
#define BB_1X1X1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BB_1X1X1_MODEL_TEXPATH "WorldObject/Dungeon/DungeonBrickWall"
#define BB_1X1X1_MODEL_EXTENTS XMFLOAT3(0.5f, 0.5f, 0.5f)
#define BB_1X1X1_MODEL_TYPE Object_World
#define BB_1X1X1_MODEL_COLLIDERTYPE Collider_None
#define BB_1X1X1_MODEL_MASS 9999
#define BB_1X1X1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BB_1X1X1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BB_1X1X1_MODEL_SHADOW true

#define BB_1X8X1_MODEL_NAME "BB_1x8x1"
#define BB_1X8X1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BB_1X8X1_MODEL_TEXPATH "WorldObject/Dungeon/DungeonBrickWall"
#define BB_1X8X1_MODEL_EXTENTS XMFLOAT3(0.5f, 0.5f, 4.0f)
#define BB_1X8X1_MODEL_TYPE Object_World
#define BB_1X8X1_MODEL_COLLIDERTYPE Collider_None
#define BB_1X8X1_MODEL_MASS 9999
#define BB_1X8X1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BB_1X8X1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BB_1X8X1_MODEL_SHADOW true

#define BB_4X8X1_MODEL_NAME "BB_4x8x1"
#define BB_4X8X1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BB_4X8X1_MODEL_TEXPATH "WorldObject/Dungeon/DungeonBrickWall"
#define BB_4X8X1_MODEL_EXTENTS XMFLOAT3(2.0f, 0.5f, 4.0f)
#define BB_4X8X1_MODEL_TYPE Object_World
#define BB_4X8X1_MODEL_COLLIDERTYPE Collider_Box
#define BB_4X8X1_MODEL_MASS 9999
#define BB_4X8X1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BB_4X8X1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BB_4X8X1_MODEL_SHADOW true

#define BB_8X16X1_MODEL_NAME "BB_8x16x1"
#define BB_8X16X1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BB_8X16X1_MODEL_TEXPATH "WorldObject/Dungeon/DungeonBrickWall"
#define BB_8X16X1_MODEL_EXTENTS XMFLOAT3(4.0f, 0.5f, 8.0f)
#define BB_8X16X1_MODEL_TYPE Object_World
#define BB_8X16X1_MODEL_COLLIDERTYPE Collider_Box
#define BB_8X16X1_MODEL_MASS 9999
#define BB_8X16X1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BB_8X16X1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BB_8X16X1_MODEL_SHADOW true

#define BB_CORNER_8X8X1_MODEL_NAME "BB_Corner_8x8x1"
#define BB_CORNER_8X8X1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BB_CORNER_8X8X1_MODEL_TEXPATH "WorldObject/Dungeon/DungeonBrickWall"
#define BB_CORNER_8X8X1_MODEL_EXTENTS XMFLOAT3(0.0f,0.0f,0.0f)
#define BB_CORNER_8X8X1_MODEL_TYPE Object_World
#define BB_CORNER_8X8X1_MODEL_COLLIDERTYPE Collider_None
#define BB_CORNER_8X8X1_MODEL_MASS 9999
#define BB_CORNER_8X8X1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BB_CORNER_8X8X1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BB_CORNER_8X8X1_MODEL_SHADOW true

#define BUILDING_FLOOR_MODEL_NAME "BuildingFloor"
#define BUILDING_FLOOR_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BUILDING_FLOOR_MODEL_TEXPATH "WorldObject/Dungeon/MetalModularStructures"
#define BUILDING_FLOOR_MODEL_EXTENTS XMFLOAT3(1.0f,0,1.0f)
#define BUILDING_FLOOR_MODEL_TYPE Object_World
#define BUILDING_FLOOR_MODEL_COLLIDERTYPE Collider_None
#define BUILDING_FLOOR_MODEL_MASS 9999
#define BUILDING_FLOOR_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BUILDING_FLOOR_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BUILDING_FLOOR_MODEL_SHADOW false

#define BUILDING_FLOOR_SHORT_MODEL_NAME "BuildingFloorShort"
#define BUILDING_FLOOR_SHORT_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define BUILDING_FLOOR_SHORT_MODEL_TEXPATH "WorldObject/Dungeon/MetalModularStructures"
#define BUILDING_FLOOR_SHORT_MODEL_EXTENTS XMFLOAT3(0.5f,0,1.0f)
#define BUILDING_FLOOR_SHORT_MODEL_TYPE Object_World
#define BUILDING_FLOOR_SHORT_MODEL_COLLIDERTYPE Collider_None
#define BUILDING_FLOOR_SHORT_MODEL_MASS 9999
#define BUILDING_FLOOR_SHORT_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define BUILDING_FLOOR_SHORT_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define BUILDING_FLOOR_SHORT_MODEL_SHADOW false

#define CARPET_10_MODEL_NAME "Carpet_10"
#define CARPET_10_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define CARPET_10_MODEL_TEXPATH "WorldObject/Dungeon/Carpet"
#define CARPET_10_MODEL_EXTENTS XMFLOAT3(0,0,0)
#define CARPET_10_MODEL_TYPE Object_World
#define CARPET_10_MODEL_COLLIDERTYPE Collider_None
#define CARPET_10_MODEL_MASS 9999
#define CARPET_10_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define CARPET_10_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define CARPET_10_MODEL_SHADOW false

#define CENTER_PLATFORM_MODEL_NAME "CenterPlatform"
#define CENTER_PLATFORM_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define CENTER_PLATFORM_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define CENTER_PLATFORM_MODEL_EXTENTS XMFLOAT3(2.0f,0,2.0f)
#define CENTER_PLATFORM_MODEL_TYPE Object_World
#define CENTER_PLATFORM_MODEL_COLLIDERTYPE Collider_None
#define CENTER_PLATFORM_MODEL_MASS 9999
#define CENTER_PLATFORM_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define CENTER_PLATFORM_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define CENTER_PLATFORM_MODEL_SHADOW false

#define CUEBE_CHAINS_MODEL_NAME "CuebeChains_1"
#define CUEBE_CHAINS_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define CUEBE_CHAINS_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define CUEBE_CHAINS_MODEL_EXTENTS XMFLOAT3(0.0f, 1.5f, 0.0f)
#define CUEBE_CHAINS_MODEL_TYPE Object_World
#define CUEBE_CHAINS_MODEL_COLLIDERTYPE Collider_None
#define CUEBE_CHAINS_MODEL_MASS 9999
#define CUEBE_CHAINS_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define CUEBE_CHAINS_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define CUEBE_CHAINS_MODEL_SHADOW true

#define DUNGEON_FLOOR_MODEL_NAME "DungeonFloor"
#define DUNGEON_FLOOR_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define DUNGEON_FLOOR_MODEL_TEXPATH "WorldObject/Brick_Wall"
#define DUNGEON_FLOOR_MODEL_EXTENTS XMFLOAT3(0.0f, 0.0f, 0.0f)
#define DUNGEON_FLOOR_MODEL_TYPE Object_World
#define DUNGEON_FLOOR_MODEL_COLLIDERTYPE Collider_None
#define DUNGEON_FLOOR_MODEL_MASS 9999
#define DUNGEON_FLOOR_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define DUNGEON_FLOOR_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define DUNGEON_FLOOR_MODEL_SHADOW false

#define DUNGEON_ROOF_MODEL_NAME "DungeonRoof"
#define DUNGEON_ROOF_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define DUNGEON_ROOF_MODEL_TEXPATH "WorldObject/Brick_Wall"
#define DUNGEON_ROOF_MODEL_EXTENTS XMFLOAT3(0.0f, 0.0f, 0.0f)
#define DUNGEON_ROOF_MODEL_TYPE Object_World
#define DUNGEON_ROOF_MODEL_COLLIDERTYPE Collider_None
#define DUNGEON_ROOF_MODEL_MASS 9999
#define DUNGEON_ROOF_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define DUNGEON_ROOF_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define DUNGEON_ROOF_MODEL_SHADOW true

#define FLAG1_MODEL_NAME "Flag1"
#define FLAG1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define FLAG1_MODEL_TEXPATH "WorldObject/Dungeon/Flag"
#define FLAG1_MODEL_EXTENTS XMFLOAT3(0.35f, 1.5f, 0.5f)
#define FLAG1_MODEL_TYPE Object_Movable
#define FLAG1_MODEL_COLLIDERTYPE Collider_Box
#define FLAG1_MODEL_MASS 5
#define FLAG1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define FLAG1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define FLAG1_MODEL_SHADOW true

#define FLAG2_MODEL_NAME "Flag2"
#define FLAG2_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define FLAG2_MODEL_TEXPATH "WorldObject/Dungeon/Flag"
#define FLAG2_MODEL_EXTENTS XMFLOAT3(0.35f, 1.5f, 0.5f)
#define FLAG2_MODEL_TYPE Object_Movable
#define FLAG2_MODEL_COLLIDERTYPE Collider_Box
#define FLAG2_MODEL_MASS 5
#define FLAG2_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define FLAG2_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define FLAG2_MODEL_SHADOW true

#define KANDLE1_MODEL_NAME "Kandle1"
#define KANDLE1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define KANDLE1_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define KANDLE1_MODEL_EXTENTS XMFLOAT3(0.4f, 1.4f, 0.4f)
#define KANDLE1_MODEL_TYPE Object_Movable
#define KANDLE1_MODEL_COLLIDERTYPE Collider_Box
#define KANDLE1_MODEL_MASS 10
#define KANDLE1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define KANDLE1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define KANDLE1_MODEL_SHADOW true

#define KANDLE2_MODEL_NAME "Kandle2"
#define KANDLE2_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define KANDLE2_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define KANDLE2_MODEL_EXTENTS XMFLOAT3(0.4f, 0.825f, 0.4f)
#define KANDLE2_MODEL_TYPE Object_Movable
#define KANDLE2_MODEL_COLLIDERTYPE Collider_Box
#define KANDLE2_MODEL_MASS 10
#define KANDLE2_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define KANDLE2_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define KANDLE2_MODEL_SHADOW true

#define KANDLE3_MODEL_NAME "Kandle3"
#define KANDLE3_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define KANDLE3_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define KANDLE3_MODEL_EXTENTS XMFLOAT3(0.4f, 0.45f, 0.4f)
#define KANDLE3_MODEL_TYPE Object_Movable
#define KANDLE3_MODEL_COLLIDERTYPE Collider_Box
#define KANDLE3_MODEL_MASS 10
#define KANDLE3_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define KANDLE3_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define KANDLE3_MODEL_SHADOW true

#define MAGIC_CRYSTAL_MODEL_NAME "MagicCrystal1"
#define MAGIC_CRYSTAL_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define MAGIC_CRYSTAL_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define MAGIC_CRYSTAL_MODEL_EXTENTS XMFLOAT3(1.25f, 1.9f, 1.25f)
#define MAGIC_CRYSTAL_MODEL_TYPE Object_World
#define MAGIC_CRYSTAL_MODEL_COLLIDERTYPE Collider_Box
#define MAGIC_CRYSTAL_MODEL_MASS 9999
#define MAGIC_CRYSTAL_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define MAGIC_CRYSTAL_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define MAGIC_CRYSTAL_MODEL_SHADOW true

#define METAL_BRIDGELEG_MODEL_NAME "MetalBridgeLeg"
#define METAL_BRIDGELEG_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define METAL_BRIDGELEG_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define METAL_BRIDGELEG_MODEL_EXTENTS XMFLOAT3(0.0f, 5.0f, 0.0f)
#define METAL_BRIDGELEG_MODEL_TYPE Object_World
#define METAL_BRIDGELEG_MODEL_COLLIDERTYPE Collider_None
#define METAL_BRIDGELEG_MODEL_MASS 9999
#define METAL_BRIDGELEG_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define METAL_BRIDGELEG_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define METAL_BRIDGELEG_MODEL_SHADOW false

#define METAL_BRIDGELEG_SIDE_MODEL_NAME "MetalBridgeSide"
#define METAL_BRIDGELEG_SIDE_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define METAL_BRIDGELEG_SIDE_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define METAL_BRIDGELEG_SIDE_MODEL_EXTENTS XMFLOAT3(0.45f, 5.0f, 1.25f)
#define METAL_BRIDGELEG_SIDE_MODEL_TYPE Object_World
#define METAL_BRIDGELEG_SIDE_MODEL_COLLIDERTYPE Collider_None
#define METAL_BRIDGELEG_SIDE_MODEL_MASS 9999
#define METAL_BRIDGELEG_SIDE_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define METAL_BRIDGELEG_SIDE_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define METAL_BRIDGELEG_SIDE_MODEL_SHADOW false

#define ROCK_FORMATION1_MODEL_NAME "RockFormation1"
#define ROCK_FORMATION1_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define ROCK_FORMATION1_MODEL_TEXPATH "WorldObject/Dungeon/Rocks"
#define ROCK_FORMATION1_MODEL_EXTENTS XMFLOAT3(1.0f, 1.85f, 1.0f)
#define ROCK_FORMATION1_MODEL_TYPE Object_World
#define ROCK_FORMATION1_MODEL_COLLIDERTYPE Collider_Box
#define ROCK_FORMATION1_MODEL_MASS 9999
#define ROCK_FORMATION1_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define ROCK_FORMATION1_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define ROCK_FORMATION1_MODEL_SHADOW true

#define ROCK_FORMATION2_MODEL_NAME "RockFormation2"
#define ROCK_FORMATION2_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define ROCK_FORMATION2_MODEL_TEXPATH "WorldObject/Dungeon/Rocks"
#define ROCK_FORMATION2_MODEL_EXTENTS XMFLOAT3(1.1f, 1.85f, 1.1f)
#define ROCK_FORMATION2_MODEL_TYPE Object_World
#define ROCK_FORMATION2_MODEL_COLLIDERTYPE Collider_Box
#define ROCK_FORMATION2_MODEL_MASS 9999
#define ROCK_FORMATION2_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define ROCK_FORMATION2_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define ROCK_FORMATION2_MODEL_SHADOW true

#define ROCK_FORMATION5_MODEL_NAME "RockFormation5"
#define ROCK_FORMATION5_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define ROCK_FORMATION5_MODEL_TEXPATH "WorldObject/Dungeon/Rocks"
#define ROCK_FORMATION5_MODEL_EXTENTS XMFLOAT3(1.1f, 1.3f, 1.1f)
#define ROCK_FORMATION5_MODEL_TYPE Object_World
#define ROCK_FORMATION5_MODEL_COLLIDERTYPE Collider_Box
#define ROCK_FORMATION5_MODEL_MASS 9999
#define ROCK_FORMATION5_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define ROCK_FORMATION5_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define ROCK_FORMATION5_MODEL_SHADOW true

#define SANCTUARY_CUBE_MODEL_NAME "SanctuaryCube"
#define SANCTUARY_CUBE_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define SANCTUARY_CUBE_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define SANCTUARY_CUBE_MODEL_EXTENTS XMFLOAT3(1.0f, 1.1f, 1.0f)
#define SANCTUARY_CUBE_MODEL_TYPE Object_World
#define SANCTUARY_CUBE_MODEL_COLLIDERTYPE Collider_Box
#define SANCTUARY_CUBE_MODEL_MASS 9999
#define SANCTUARY_CUBE_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define SANCTUARY_CUBE_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define SANCTUARY_CUBE_MODEL_SHADOW true

#define STATUE_MODEL_NAME "Statue"
#define STATUE_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define STATUE_MODEL_TEXPATH "WorldObject/Dungeon/Statue"
#define STATUE_MODEL_EXTENTS XMFLOAT3(1.0f, 1.65f, 0.8f)
#define STATUE_MODEL_TYPE Object_Movable
#define STATUE_MODEL_COLLIDERTYPE Collider_Box
#define STATUE_MODEL_MASS 20
#define STATUE_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define STATUE_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define STATUE_MODEL_SHADOW true


#define STATUE2_MODEL_NAME "Statue2"
#define STATUE2_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define STATUE2_MODEL_TEXPATH "WorldObject/Dungeon/Statue"
#define STATUE2_MODEL_EXTENTS XMFLOAT3(1.0f, 1.65f, 0.8f)
#define STATUE2_MODEL_TYPE Object_Movable
#define STATUE2_MODEL_COLLIDERTYPE Collider_Box
#define STATUE2_MODEL_MASS 20
#define STATUE2_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define STATUE2_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define STATUE2_MODEL_SHADOW true

#define TELEPORT_MODEL_NAME "Teleport"
#define TELEPORT_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define TELEPORT_MODEL_TEXPATH "WorldObject/Dungeon/Sanctuary"
#define TELEPORT_MODEL_EXTENTS XMFLOAT3(0.8f, 1.4f, 0.8f)
#define TELEPORT_MODEL_TYPE Object_Movable
#define TELEPORT_MODEL_COLLIDERTYPE Collider_Box
#define TELEPORT_MODEL_MASS 5
#define TELEPORT_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define TELEPORT_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define TELEPORT_MODEL_SHADOW true

#define TOWER_EXTENTION_MODEL_NAME "tower_extension"
#define TOWER_EXTENTION_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define TOWER_EXTENTION_MODEL_TEXPATH "WorldObject/Brick_Wall"
#define TOWER_EXTENTION_MODEL_EXTENTS XMFLOAT3(2.25f, 2.25f, 4.75f)
#define TOWER_EXTENTION_MODEL_TYPE Object_World
#define TOWER_EXTENTION_MODEL_COLLIDERTYPE Collider_Box
#define TOWER_EXTENTION_MODEL_MASS 9999
#define TOWER_EXTENTION_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define TOWER_EXTENTION_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define TOWER_EXTENTION_MODEL_SHADOW true

#define TUBECHANNEL_MODEL_NAME "TubeChannel"
#define TUBECHANNEL_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define TUBECHANNEL_MODEL_TEXPATH "WorldObject/Dungeon/MetalModularStructures"
#define TUBECHANNEL_MODEL_EXTENTS XMFLOAT3(0.35f, 0.0f, 0.0f)
#define TUBECHANNEL_MODEL_TYPE Object_World
#define TUBECHANNEL_MODEL_COLLIDERTYPE Collider_None
#define TUBECHANNEL_MODEL_MASS 9999
#define TUBECHANNEL_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define TUBECHANNEL_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define TUBECHANNEL_MODEL_SHADOW true

#define VINTILATIONBOX_MODEL_NAME "VintilationBox"
#define VINTILATIONBOX_MODEL_OBJECTPATH "WorldObject/Dungeon"
#define VINTILATIONBOX_MODEL_TEXPATH "WorldObject/Dungeon/MetalModularStructures"
#define VINTILATIONBOX_MODEL_EXTENTS XMFLOAT3(0.75f, 0.4f, 0.43f)
#define VINTILATIONBOX_MODEL_TYPE Object_Movable
#define VINTILATIONBOX_MODEL_COLLIDERTYPE Collider_Box
#define VINTILATIONBOX_MODEL_MASS 9999
#define VINTILATIONBOX_MODEL_SCALE XMFLOAT3(10.0f, 10.0f, 10.0f)
#define VINTILATIONBOX_MODEL_RENDERLAYER RenderLayer::Render_TextureMesh
#define VINTILATIONBOX_MODEL_SHADOW true


#endif