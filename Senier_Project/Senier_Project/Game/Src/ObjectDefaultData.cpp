#include "../Header/ObjectDefaultData.h"

void CreateObjectDefaultData()
{
	ObjectDefaultData tmpData;
	tmpData.pstrTexPath = CHARACTER_MODEL_TEXPATH;
	tmpData.xmf3Extents = CHARACTER_MODEL_EXTENTS;
	tmpData.objectType = CHARACTER_MODEL_TYPE;
	tmpData.colliderType = CHARACTER_MODEL_COLLIDERTYPE;
	tmpData.nMass = CHARACTER_MODEL_MASS;
	tmpData.xmf3OffsetScale = CHARACTER_MODEL_SCALE;
	tmpData.renderLayer = CHARACTER_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0,0,0);
	g_DefaultObjectData.insert({ CHARACTER_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CHARACTER_MODEL_NAME);

	tmpData.pstrTexPath = ZOMBIE_MODEL_TEXPATH;
	tmpData.xmf3Extents = ZOMBIE_MODEL_EXTENTS;
	tmpData.objectType = ZOMBIE_MODEL_TYPE;
	tmpData.colliderType = ZOMBIE_MODEL_COLLIDERTYPE;
	tmpData.nMass = ZOMBIE_MODEL_MASS;
	tmpData.xmf3OffsetScale = ZOMBIE_MODEL_SCALE;
	tmpData.renderLayer = ZOMBIE_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ ZOMBIE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(ZOMBIE_MODEL_NAME);

	tmpData.pstrTexPath = WEAPON_MODEL_TEXPATH;
	tmpData.xmf3Extents = WEAPON_MODEL_EXTENTS;
	tmpData.objectType = WEAPON_MODEL_TYPE;
	tmpData.colliderType = WEAPON_MODEL_COLLIDERTYPE;
	tmpData.nMass = WEAPON_MODEL_MASS;
	tmpData.xmf3OffsetScale = WEAPON_MODEL_SCALE;
	tmpData.renderLayer = WEAPON_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ WEAPON_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WEAPON_MODEL_NAME);

	tmpData.pstrTexPath = CUBE_MODEL_TEXPATH;
	tmpData.xmf3Extents = CUBE_MODEL_EXTENTS;
	tmpData.objectType = CUBE_MODEL_TYPE;
	tmpData.colliderType = CUBE_MODEL_COLLIDERTYPE;
	tmpData.nMass = CUBE_MODEL_MASS;
	tmpData.xmf3OffsetScale = CUBE_MODEL_SCALE;
	tmpData.renderLayer = CUBE_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ CUBE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CUBE_MODEL_NAME);

	tmpData.pstrTexPath = GROUND_MODEL_TEXPATH;
	tmpData.xmf3Extents = GROUND_MODEL_EXTENTS;
	tmpData.objectType = GROUND_MODEL_TYPE;
	tmpData.colliderType = GROUND_MODEL_COLLIDERTYPE;
	tmpData.nMass = GROUND_MODEL_MASS;
	tmpData.xmf3OffsetScale = GROUND_MODEL_SCALE;
	tmpData.renderLayer = GROUND_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ GROUND_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(GROUND_MODEL_NAME);

	tmpData.pstrTexPath = CARPET_MODEL_TEXPATH;
	tmpData.xmf3Extents = CARPET_MODEL_EXTENTS;
	tmpData.objectType = CARPET_MODEL_TYPE;
	tmpData.colliderType = CARPET_MODEL_COLLIDERTYPE;
	tmpData.nMass = CARPET_MODEL_MASS;
	tmpData.xmf3OffsetScale = CARPET_MODEL_SCALE;
	tmpData.renderLayer = CARPET_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ CARPET_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CARPET_MODEL_NAME);

	tmpData.pstrTexPath = DRAWER_MODEL_TEXPATH;
	tmpData.xmf3Extents = DRAWER_MODEL_EXTENTS;
	tmpData.objectType = DRAWER_MODEL_TYPE;
	tmpData.colliderType = DRAWER_MODEL_COLLIDERTYPE;
	tmpData.nMass = DRAWER_MODEL_MASS;
	tmpData.xmf3OffsetScale = DRAWER_MODEL_SCALE;
	tmpData.renderLayer = DRAWER_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ DRAWER_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(DRAWER_MODEL_NAME);

	tmpData.pstrTexPath = MECHANICAL_ARM_MODEL_TEXPATH;
	tmpData.xmf3Extents = MECHANICAL_ARM_MODEL_EXTENTS;
	tmpData.objectType = MECHANICAL_ARM_MODEL_TYPE;
	tmpData.colliderType = MECHANICAL_ARM_MODEL_COLLIDERTYPE;
	tmpData.nMass = MECHANICAL_ARM_MODEL_MASS;
	tmpData.xmf3OffsetScale = MECHANICAL_ARM_MODEL_SCALE;
	tmpData.renderLayer = MECHANICAL_ARM_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(-1.0f, tmpData.xmf3Extents.y, 0.8f);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ MECHANICAL_ARM_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(MECHANICAL_ARM_MODEL_NAME);

	tmpData.pstrTexPath = OFFICE_CHAIR_MODEL_TEXPATH;
	tmpData.xmf3Extents = OFFICE_CHAIR_MODEL_EXTENTS;
	tmpData.objectType = OFFICE_CHAIR_MODEL_TYPE;
	tmpData.colliderType = OFFICE_CHAIR_MODEL_COLLIDERTYPE;
	tmpData.nMass = OFFICE_CHAIR_MODEL_MASS;
	tmpData.xmf3OffsetScale = OFFICE_CHAIR_MODEL_SCALE;
	tmpData.renderLayer = OFFICE_CHAIR_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ OFFICE_CHAIR_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(OFFICE_CHAIR_MODEL_NAME);

	tmpData.pstrTexPath = DOOR_MODEL_TEXPATH;
	tmpData.xmf3Extents = DOOR_MODEL_EXTENTS;
	tmpData.objectType = DOOR_MODEL_TYPE;
	tmpData.colliderType = DOOR_MODEL_COLLIDERTYPE;
	tmpData.nMass = DOOR_MODEL_MASS;
	tmpData.xmf3OffsetScale = DOOR_MODEL_SCALE;
	tmpData.renderLayer = DOOR_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ DOOR_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(DOOR_MODEL_NAME);

	tmpData.pstrTexPath = SERVER_RACK_MODEL_TEXPATH;
	tmpData.xmf3Extents = SERVER_RACK_MODEL_EXTENTS;
	tmpData.objectType = SERVER_RACK_MODEL_TYPE;
	tmpData.colliderType = SERVER_RACK_MODEL_COLLIDERTYPE;
	tmpData.nMass = SERVER_RACK_MODEL_MASS;
	tmpData.xmf3OffsetScale = SERVER_RACK_MODEL_SCALE;
	tmpData.renderLayer = SERVER_RACK_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ SERVER_RACK_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SERVER_RACK_MODEL_NAME);

	tmpData.pstrTexPath = SHELF_CRATE_MODEL_TEXPATH;
	tmpData.xmf3Extents = SHELF_CRATE_MODEL_EXTENTS;
	tmpData.objectType = SHELF_CRATE_MODEL_TYPE;
	tmpData.colliderType = SHELF_CRATE_MODEL_COLLIDERTYPE;
	tmpData.nMass = SHELF_CRATE_MODEL_MASS;
	tmpData.xmf3OffsetScale = SHELF_CRATE_MODEL_SCALE;
	tmpData.renderLayer = SHELF_CRATE_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ SHELF_CRATE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SHELF_CRATE_MODEL_NAME);

	tmpData.pstrTexPath = SHELF_MODEL_TEXPATH;
	tmpData.xmf3Extents = SHELF_MODEL_EXTENTS;
	tmpData.objectType = SHELF_MODEL_TYPE;
	tmpData.colliderType = SHELF_MODEL_COLLIDERTYPE;
	tmpData.nMass = SHELF_MODEL_MASS;
	tmpData.xmf3OffsetScale = SHELF_MODEL_SCALE;
	tmpData.renderLayer = SHELF_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ SHELF_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SHELF_MODEL_NAME);

	tmpData.pstrTexPath = STOOL_MODEL_TEXPATH;
	tmpData.xmf3Extents = STOOL_MODEL_EXTENTS;
	tmpData.objectType = STOOL_MODEL_TYPE;
	tmpData.colliderType = STOOL_MODEL_COLLIDERTYPE;
	tmpData.nMass = STOOL_MODEL_MASS;
	tmpData.xmf3OffsetScale = STOOL_MODEL_SCALE;
	tmpData.renderLayer = STOOL_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ STOOL_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(STOOL_MODEL_NAME);

	tmpData.pstrTexPath = TABLE_MODEL_TEXPATH;
	tmpData.xmf3Extents = TABLE_MODEL_EXTENTS;
	tmpData.objectType = TABLE_MODEL_TYPE;
	tmpData.colliderType = TABLE_MODEL_COLLIDERTYPE;
	tmpData.nMass = TABLE_MODEL_MASS;
	tmpData.xmf3OffsetScale = TABLE_MODEL_SCALE;
	tmpData.renderLayer = TABLE_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ TABLE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(TABLE_MODEL_NAME);

	tmpData.pstrTexPath = TABLE_GLASS_MODEL_TEXPATH;
	tmpData.xmf3Extents = TABLE_GLASS_MODEL_EXTENTS;
	tmpData.objectType = TABLE_GLASS_MODEL_TYPE;
	tmpData.colliderType = TABLE_GLASS_MODEL_COLLIDERTYPE;
	tmpData.nMass = TABLE_GLASS_MODEL_MASS;
	tmpData.xmf3OffsetScale = TABLE_GLASS_MODEL_SCALE;
	tmpData.renderLayer = TABLE_GLASS_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ TABLE_GLASS_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(TABLE_GLASS_MODEL_NAME);

	tmpData.pstrTexPath = TABLE_SET_MODEL_TEXPATH;
	tmpData.xmf3Extents = TABLE_SET_MODEL_EXTENTS;
	tmpData.objectType = TABLE_SET_MODEL_TYPE;
	tmpData.colliderType = TABLE_SET_MODEL_COLLIDERTYPE;
	tmpData.nMass = TABLE_SET_MODEL_MASS;
	tmpData.xmf3OffsetScale = TABLE_SET_MODEL_SCALE;
	tmpData.renderLayer = TABLE_SET_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ TABLE_SET_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(TABLE_SET_MODEL_NAME);

	tmpData.pstrTexPath = VASE_MODEL_TEXPATH;
	tmpData.xmf3Extents = VASE_MODEL_EXTENTS;
	tmpData.objectType = VASE_MODEL_TYPE;
	tmpData.colliderType = VASE_MODEL_COLLIDERTYPE;
	tmpData.nMass = VASE_MODEL_MASS;
	tmpData.xmf3OffsetScale = VASE_MODEL_SCALE;
	tmpData.renderLayer = VASE_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ VASE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(VASE_MODEL_NAME);

	tmpData.pstrTexPath = WALL_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_MODEL_EXTENTS;
	tmpData.objectType = WALL_MODEL_TYPE;
	tmpData.colliderType = WALL_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_MODEL_SCALE;
	tmpData.renderLayer = WALL_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(-tmpData.xmf3Extents.x, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ WALL_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_MODEL_NAME);

	tmpData.pstrTexPath = WALL_WITH_WINDOWS_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_WITH_WINDOWS_MODEL_EXTENTS;
	tmpData.objectType = WALL_WITH_WINDOWS_MODEL_TYPE;
	tmpData.colliderType = WALL_WITH_WINDOWS_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_WITH_WINDOWS_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_WITH_WINDOWS_MODEL_SCALE;
	tmpData.renderLayer = WALL_WITH_WINDOWS_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ WALL_WITH_WINDOWS_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_WITH_WINDOWS_MODEL_NAME);

	tmpData.pstrTexPath = WALL_PILLAR_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_PILLAR_MODEL_EXTENTS;
	tmpData.objectType = WALL_PILLAR_MODEL_TYPE;
	tmpData.colliderType = WALL_PILLAR_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_PILLAR_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_PILLAR_MODEL_SCALE;
	tmpData.renderLayer = WALL_PILLAR_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ WALL_PILLAR_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_PILLAR_MODEL_NAME);

	tmpData.pstrTexPath = WALL_PILLAR_3_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_PILLAR_3_MODEL_EXTENTS;
	tmpData.objectType = WALL_PILLAR_3_MODEL_TYPE;
	tmpData.colliderType = WALL_PILLAR_3_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_PILLAR_3_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_PILLAR_3_MODEL_SCALE;
	tmpData.renderLayer = WALL_PILLAR_3_MODEL_RENDERLAYER;
	tmpData.xmf3ColliderOffsetPosition = XMFLOAT3(0.1f, tmpData.xmf3Extents.y, -0.1f);
	tmpData.xmf3ColliderOffsetRotation = XMFLOAT3(0, 0, 0);
	g_DefaultObjectData.insert({ WALL_PILLAR_3_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_PILLAR_3_MODEL_NAME);

}
