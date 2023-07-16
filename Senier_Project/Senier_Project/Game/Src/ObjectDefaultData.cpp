#include "../Header/ObjectDefaultData.h"

void CreateObjectDefaultData()
{
	ObjectDefaultData tmpData;
	tmpData.pstrObjectPath = CHARACTER_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = CHARACTER_MODEL_TEXPATH;
	tmpData.xmf3Extents = CHARACTER_MODEL_EXTENTS;
	tmpData.objectType = CHARACTER_MODEL_TYPE;
	tmpData.colliderType = CHARACTER_MODEL_COLLIDERTYPE;
	tmpData.nMass = CHARACTER_MODEL_MASS;
	tmpData.xmf3OffsetScale = CHARACTER_MODEL_SCALE;
	tmpData.renderLayer = CHARACTER_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0,0,0);
	tmpData.bShadowed = CHARACTER_MODEL_SHADOW;
	g_DefaultObjectData.insert({ CHARACTER_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CHARACTER_MODEL_NAME);

	tmpData.pstrObjectPath = PLAYER_PROJECTILE1_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = PLAYER_PROJECTILE1_MODEL_TEXPATH;
	tmpData.xmf3Extents = PLAYER_PROJECTILE1_MODEL_EXTENTS;
	tmpData.objectType = PLAYER_PROJECTILE1_MODEL_TYPE;
	tmpData.colliderType = PLAYER_PROJECTILE1_MODEL_COLLIDERTYPE;
	tmpData.nMass = PLAYER_PROJECTILE1_MODEL_MASS;
	tmpData.xmf3OffsetScale = PLAYER_PROJECTILE1_MODEL_SCALE;
	tmpData.renderLayer = PLAYER_PROJECTILE1_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = PLAYER_PROJECTILE1_MODEL_SHADOW;
	g_DefaultObjectData.insert({ PLAYER_PROJECTILE1_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(PLAYER_PROJECTILE1_MODEL_NAME);

	tmpData.pstrObjectPath = PLAYER_PROJECTILE2_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = PLAYER_PROJECTILE2_MODEL_TEXPATH;
	tmpData.xmf3Extents = PLAYER_PROJECTILE2_MODEL_EXTENTS;
	tmpData.objectType = PLAYER_PROJECTILE2_MODEL_TYPE;
	tmpData.colliderType = PLAYER_PROJECTILE2_MODEL_COLLIDERTYPE;
	tmpData.nMass = PLAYER_PROJECTILE2_MODEL_MASS;
	tmpData.xmf3OffsetScale = PLAYER_PROJECTILE2_MODEL_SCALE;
	tmpData.renderLayer = PLAYER_PROJECTILE2_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = PLAYER_PROJECTILE2_MODEL_SHADOW;
	g_DefaultObjectData.insert({ PLAYER_PROJECTILE2_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(PLAYER_PROJECTILE2_MODEL_NAME);

	tmpData.pstrObjectPath = PLAYER_PROJECTILE3_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = PLAYER_PROJECTILE3_MODEL_TEXPATH;
	tmpData.xmf3Extents = PLAYER_PROJECTILE3_MODEL_EXTENTS;
	tmpData.objectType = PLAYER_PROJECTILE3_MODEL_TYPE;
	tmpData.colliderType = PLAYER_PROJECTILE3_MODEL_COLLIDERTYPE;
	tmpData.nMass = PLAYER_PROJECTILE3_MODEL_MASS;
	tmpData.xmf3OffsetScale = PLAYER_PROJECTILE3_MODEL_SCALE;
	tmpData.renderLayer = PLAYER_PROJECTILE3_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = PLAYER_PROJECTILE3_MODEL_SHADOW;
	g_DefaultObjectData.insert({ PLAYER_PROJECTILE3_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(PLAYER_PROJECTILE3_MODEL_NAME);

	tmpData.pstrObjectPath = ENEMY_PROJECTILE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = ENEMY_PROJECTILE_MODEL_TEXPATH;
	tmpData.xmf3Extents = ENEMY_PROJECTILE_MODEL_EXTENTS;
	tmpData.objectType = ENEMY_PROJECTILE_MODEL_TYPE;
	tmpData.colliderType = ENEMY_PROJECTILE_MODEL_COLLIDERTYPE;
	tmpData.nMass = ENEMY_PROJECTILE_MODEL_MASS;
	tmpData.xmf3OffsetScale = ENEMY_PROJECTILE_MODEL_SCALE;
	tmpData.renderLayer = ENEMY_PROJECTILE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = ENEMY_PROJECTILE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ ENEMY_PROJECTILE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(ENEMY_PROJECTILE_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD1_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD1_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD1_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD1_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD1_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD1_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD1_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD1_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD1_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD1_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD1_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD2_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD2_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD2_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD2_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD2_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD2_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD2_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD2_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD2_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD2_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD2_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD3_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD3_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD3_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD3_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD3_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD3_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD3_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD3_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD3_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD3_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD3_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD4_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD4_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD4_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD4_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD4_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD4_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD4_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD4_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD4_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD4_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD4_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD5_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD5_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD5_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD5_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD5_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD5_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD5_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD5_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD5_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD5_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD5_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD6_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD6_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD6_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD6_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD6_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD6_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD6_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD6_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD6_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD6_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD6_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = BLOOD7_EFFECT_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = BLOOD7_EFFECT_MODEL_TEXPATH;
	tmpData.xmf3Extents = BLOOD7_EFFECT_MODEL_EXTENTS;
	tmpData.objectType = BLOOD7_EFFECT_MODEL_TYPE;
	tmpData.colliderType = BLOOD7_EFFECT_MODEL_COLLIDERTYPE;
	tmpData.nMass = BLOOD7_EFFECT_MODEL_MASS;
	tmpData.xmf3OffsetScale = BLOOD7_EFFECT_MODEL_SCALE;
	tmpData.renderLayer = BLOOD7_EFFECT_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, 0, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = BLOOD7_EFFECT_MODEL_SHADOW;
	g_DefaultObjectData.insert({ BLOOD7_EFFECT_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(BLOOD7_EFFECT_MODEL_NAME);

	tmpData.pstrObjectPath = ZOMBIE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = ZOMBIE_MODEL_TEXPATH;
	tmpData.xmf3Extents = ZOMBIE_MODEL_EXTENTS;
	tmpData.objectType = ZOMBIE_MODEL_TYPE;
	tmpData.colliderType = ZOMBIE_MODEL_COLLIDERTYPE;
	tmpData.nMass = ZOMBIE_MODEL_MASS;
	tmpData.xmf3OffsetScale = ZOMBIE_MODEL_SCALE;
	tmpData.renderLayer = ZOMBIE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = ZOMBIE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ ZOMBIE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(ZOMBIE_MODEL_NAME);	
	
	tmpData.pstrObjectPath = HIGHZOMBIE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = HIGHZOMBIE_MODEL_TEXPATH;
	tmpData.xmf3Extents = HIGHZOMBIE_MODEL_EXTENTS;
	tmpData.objectType = HIGHZOMBIE_MODEL_TYPE;
	tmpData.colliderType = HIGHZOMBIE_MODEL_COLLIDERTYPE;
	tmpData.nMass = HIGHZOMBIE_MODEL_MASS;
	tmpData.xmf3OffsetScale = HIGHZOMBIE_MODEL_SCALE;
	tmpData.renderLayer = HIGHZOMBIE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = HIGHZOMBIE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ HIGHZOMBIE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(HIGHZOMBIE_MODEL_NAME);

	tmpData.pstrObjectPath = SCAVENGER_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = SCAVENGER_MODEL_TEXPATH;
	tmpData.xmf3Extents = SCAVENGER_MODEL_EXTENTS;
	tmpData.objectType = SCAVENGER_MODEL_TYPE;
	tmpData.colliderType = SCAVENGER_MODEL_COLLIDERTYPE;
	tmpData.nMass = SCAVENGER_MODEL_MASS;
	tmpData.xmf3OffsetScale = SCAVENGER_MODEL_SCALE;
	tmpData.renderLayer = SCAVENGER_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = SCAVENGER_MODEL_SHADOW;
	g_DefaultObjectData.insert({ SCAVENGER_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SCAVENGER_MODEL_NAME);

	tmpData.pstrObjectPath = CYBER_TWINS_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = CYBER_TWINS_MODEL_TEXPATH;
	tmpData.xmf3Extents = CYBER_TWINS_MODEL_EXTENTS;
	tmpData.objectType = CYBER_TWINS_MODEL_TYPE;
	tmpData.colliderType = CYBER_TWINS_MODEL_COLLIDERTYPE;
	tmpData.nMass = CYBER_TWINS_MODEL_MASS;
	tmpData.xmf3OffsetScale = CYBER_TWINS_MODEL_SCALE;
	tmpData.renderLayer = CYBER_TWINS_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = CYBER_TWINS_MODEL_SHADOW;
	g_DefaultObjectData.insert({ CYBER_TWINS_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CYBER_TWINS_MODEL_NAME);

	tmpData.pstrObjectPath = GHOUL_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = GHOUL_MODEL_TEXPATH;
	tmpData.xmf3Extents = GHOUL_MODEL_EXTENTS;
	tmpData.objectType = GHOUL_MODEL_TYPE;
	tmpData.colliderType = GHOUL_MODEL_COLLIDERTYPE;
	tmpData.nMass = GHOUL_MODEL_MASS;
	tmpData.xmf3OffsetScale = GHOUL_MODEL_SCALE;
	tmpData.renderLayer = GHOUL_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = GHOUL_MODEL_SHADOW;
	g_DefaultObjectData.insert({ GHOUL_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(GHOUL_MODEL_NAME);

	tmpData.pstrObjectPath = NECROMANCER_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = NECROMANCER_MODEL_TEXPATH;
	tmpData.xmf3Extents = NECROMANCER_MODEL_EXTENTS;
	tmpData.objectType = NECROMANCER_MODEL_TYPE;
	tmpData.colliderType = NECROMANCER_MODEL_COLLIDERTYPE;
	tmpData.nMass = NECROMANCER_MODEL_MASS;
	tmpData.xmf3OffsetScale = NECROMANCER_MODEL_SCALE;
	tmpData.renderLayer = NECROMANCER_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = NECROMANCER_MODEL_SHADOW;
	g_DefaultObjectData.insert({ NECROMANCER_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(NECROMANCER_MODEL_NAME);

	tmpData.pstrObjectPath = WEAPON_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = WEAPON_MODEL_TEXPATH;
	tmpData.xmf3Extents = WEAPON_MODEL_EXTENTS;
	tmpData.objectType = WEAPON_MODEL_TYPE;
	tmpData.colliderType = WEAPON_MODEL_COLLIDERTYPE;
	tmpData.nMass = WEAPON_MODEL_MASS;
	tmpData.xmf3OffsetScale = WEAPON_MODEL_SCALE;
	tmpData.renderLayer = WEAPON_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y - 1.3f, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = WEAPON_MODEL_SHADOW;
	g_DefaultObjectData.insert({ WEAPON_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WEAPON_MODEL_NAME);

	tmpData.pstrObjectPath = CUBE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = CUBE_MODEL_TEXPATH;
	tmpData.xmf3Extents = CUBE_MODEL_EXTENTS;
	tmpData.objectType = CUBE_MODEL_TYPE;
	tmpData.colliderType = CUBE_MODEL_COLLIDERTYPE;
	tmpData.nMass = CUBE_MODEL_MASS;
	tmpData.xmf3OffsetScale = CUBE_MODEL_SCALE;
	tmpData.renderLayer = CUBE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = CUBE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ CUBE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CUBE_MODEL_NAME);

	tmpData.pstrObjectPath = GROUND_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = GROUND_MODEL_TEXPATH;
	tmpData.xmf3Extents = GROUND_MODEL_EXTENTS;
	tmpData.objectType = GROUND_MODEL_TYPE;
	tmpData.colliderType = GROUND_MODEL_COLLIDERTYPE;
	tmpData.nMass = GROUND_MODEL_MASS;
	tmpData.xmf3OffsetScale = GROUND_MODEL_SCALE;
	tmpData.renderLayer = GROUND_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = GROUND_MODEL_SHADOW;
	g_DefaultObjectData.insert({ GROUND_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(GROUND_MODEL_NAME);

	tmpData.pstrObjectPath = CARPET_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = CARPET_MODEL_TEXPATH;
	tmpData.xmf3Extents = CARPET_MODEL_EXTENTS;
	tmpData.objectType = CARPET_MODEL_TYPE;
	tmpData.colliderType = CARPET_MODEL_COLLIDERTYPE;
	tmpData.nMass = CARPET_MODEL_MASS;
	tmpData.xmf3OffsetScale = CARPET_MODEL_SCALE;
	tmpData.renderLayer = CARPET_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = CARPET_MODEL_SHADOW;
	g_DefaultObjectData.insert({ CARPET_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(CARPET_MODEL_NAME);

	tmpData.pstrObjectPath = DRAWER_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = DRAWER_MODEL_TEXPATH;
	tmpData.xmf3Extents = DRAWER_MODEL_EXTENTS;
	tmpData.objectType = DRAWER_MODEL_TYPE;
	tmpData.colliderType = DRAWER_MODEL_COLLIDERTYPE;
	tmpData.nMass = DRAWER_MODEL_MASS;
	tmpData.xmf3OffsetScale = DRAWER_MODEL_SCALE;
	tmpData.renderLayer = DRAWER_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = DRAWER_MODEL_SHADOW;
	g_DefaultObjectData.insert({ DRAWER_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(DRAWER_MODEL_NAME);

	tmpData.pstrObjectPath = MECHANICAL_ARM_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = MECHANICAL_ARM_MODEL_TEXPATH;
	tmpData.xmf3Extents = MECHANICAL_ARM_MODEL_EXTENTS;
	tmpData.objectType = MECHANICAL_ARM_MODEL_TYPE;
	tmpData.colliderType = MECHANICAL_ARM_MODEL_COLLIDERTYPE;
	tmpData.nMass = MECHANICAL_ARM_MODEL_MASS;
	tmpData.xmf3OffsetScale = MECHANICAL_ARM_MODEL_SCALE;
	tmpData.renderLayer = MECHANICAL_ARM_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(-1.0f, tmpData.xmf3Extents.y, 0.8f);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = MECHANICAL_ARM_MODEL_SHADOW;
	g_DefaultObjectData.insert({ MECHANICAL_ARM_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(MECHANICAL_ARM_MODEL_NAME);

	tmpData.pstrObjectPath = OFFICE_CHAIR_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = OFFICE_CHAIR_MODEL_TEXPATH;
	tmpData.xmf3Extents = OFFICE_CHAIR_MODEL_EXTENTS;
	tmpData.objectType = OFFICE_CHAIR_MODEL_TYPE;
	tmpData.colliderType = OFFICE_CHAIR_MODEL_COLLIDERTYPE;
	tmpData.nMass = OFFICE_CHAIR_MODEL_MASS;
	tmpData.xmf3OffsetScale = OFFICE_CHAIR_MODEL_SCALE;
	tmpData.renderLayer = OFFICE_CHAIR_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = OFFICE_CHAIR_MODEL_SHADOW;
	g_DefaultObjectData.insert({ OFFICE_CHAIR_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(OFFICE_CHAIR_MODEL_NAME);

	tmpData.pstrObjectPath = DOOR_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = DOOR_MODEL_TEXPATH;
	tmpData.xmf3Extents = DOOR_MODEL_EXTENTS;
	tmpData.objectType = DOOR_MODEL_TYPE;
	tmpData.colliderType = DOOR_MODEL_COLLIDERTYPE;
	tmpData.nMass = DOOR_MODEL_MASS;
	tmpData.xmf3OffsetScale = DOOR_MODEL_SCALE;
	tmpData.renderLayer = DOOR_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = DOOR_MODEL_SHADOW;
	g_DefaultObjectData.insert({ DOOR_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(DOOR_MODEL_NAME);

	tmpData.pstrObjectPath = SERVER_RACK_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = SERVER_RACK_MODEL_TEXPATH;
	tmpData.xmf3Extents = SERVER_RACK_MODEL_EXTENTS;
	tmpData.objectType = SERVER_RACK_MODEL_TYPE;
	tmpData.colliderType = SERVER_RACK_MODEL_COLLIDERTYPE;
	tmpData.nMass = SERVER_RACK_MODEL_MASS;
	tmpData.xmf3OffsetScale = SERVER_RACK_MODEL_SCALE;
	tmpData.renderLayer = SERVER_RACK_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = SERVER_RACK_MODEL_SHADOW;
	g_DefaultObjectData.insert({ SERVER_RACK_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SERVER_RACK_MODEL_NAME);

	tmpData.pstrObjectPath = SHELF_CRATE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = SHELF_CRATE_MODEL_TEXPATH;
	tmpData.xmf3Extents = SHELF_CRATE_MODEL_EXTENTS;
	tmpData.objectType = SHELF_CRATE_MODEL_TYPE;
	tmpData.colliderType = SHELF_CRATE_MODEL_COLLIDERTYPE;
	tmpData.nMass = SHELF_CRATE_MODEL_MASS;
	tmpData.xmf3OffsetScale = SHELF_CRATE_MODEL_SCALE;
	tmpData.renderLayer = SHELF_CRATE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = SHELF_CRATE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ SHELF_CRATE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SHELF_CRATE_MODEL_NAME);

	tmpData.pstrObjectPath = SHELF_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = SHELF_MODEL_TEXPATH;
	tmpData.xmf3Extents = SHELF_MODEL_EXTENTS;
	tmpData.objectType = SHELF_MODEL_TYPE;
	tmpData.colliderType = SHELF_MODEL_COLLIDERTYPE;
	tmpData.nMass = SHELF_MODEL_MASS;
	tmpData.xmf3OffsetScale = SHELF_MODEL_SCALE;
	tmpData.renderLayer = SHELF_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = SHELF_MODEL_SHADOW;
	g_DefaultObjectData.insert({ SHELF_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(SHELF_MODEL_NAME);

	tmpData.pstrObjectPath = STOOL_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = STOOL_MODEL_TEXPATH;
	tmpData.xmf3Extents = STOOL_MODEL_EXTENTS;
	tmpData.objectType = STOOL_MODEL_TYPE;
	tmpData.colliderType = STOOL_MODEL_COLLIDERTYPE;
	tmpData.nMass = STOOL_MODEL_MASS;
	tmpData.xmf3OffsetScale = STOOL_MODEL_SCALE;
	tmpData.renderLayer = STOOL_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = STOOL_MODEL_SHADOW;
	g_DefaultObjectData.insert({ STOOL_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(STOOL_MODEL_NAME);

	tmpData.pstrObjectPath = TABLE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = TABLE_MODEL_TEXPATH;
	tmpData.xmf3Extents = TABLE_MODEL_EXTENTS;
	tmpData.objectType = TABLE_MODEL_TYPE;
	tmpData.colliderType = TABLE_MODEL_COLLIDERTYPE;
	tmpData.nMass = TABLE_MODEL_MASS;
	tmpData.xmf3OffsetScale = TABLE_MODEL_SCALE;
	tmpData.renderLayer = TABLE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = TABLE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ TABLE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(TABLE_MODEL_NAME);

	tmpData.pstrObjectPath = TABLE_GLASS_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = TABLE_GLASS_MODEL_TEXPATH;
	tmpData.xmf3Extents = TABLE_GLASS_MODEL_EXTENTS;
	tmpData.objectType = TABLE_GLASS_MODEL_TYPE;
	tmpData.colliderType = TABLE_GLASS_MODEL_COLLIDERTYPE;
	tmpData.nMass = TABLE_GLASS_MODEL_MASS;
	tmpData.xmf3OffsetScale = TABLE_GLASS_MODEL_SCALE;
	tmpData.renderLayer = TABLE_GLASS_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = TABLE_GLASS_MODEL_SHADOW;
	g_DefaultObjectData.insert({ TABLE_GLASS_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(TABLE_GLASS_MODEL_NAME);

	tmpData.pstrObjectPath = TABLE_SET_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = TABLE_SET_MODEL_TEXPATH;
	tmpData.xmf3Extents = TABLE_SET_MODEL_EXTENTS;
	tmpData.objectType = TABLE_SET_MODEL_TYPE;
	tmpData.colliderType = TABLE_SET_MODEL_COLLIDERTYPE;
	tmpData.nMass = TABLE_SET_MODEL_MASS;
	tmpData.xmf3OffsetScale = TABLE_SET_MODEL_SCALE;
	tmpData.renderLayer = TABLE_SET_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = TABLE_SET_MODEL_SHADOW;
	g_DefaultObjectData.insert({ TABLE_SET_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(TABLE_SET_MODEL_NAME);

	tmpData.pstrObjectPath = VASE_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = VASE_MODEL_TEXPATH;
	tmpData.xmf3Extents = VASE_MODEL_EXTENTS;
	tmpData.objectType = VASE_MODEL_TYPE;
	tmpData.colliderType = VASE_MODEL_COLLIDERTYPE;
	tmpData.nMass = VASE_MODEL_MASS;
	tmpData.xmf3OffsetScale = VASE_MODEL_SCALE;
	tmpData.renderLayer = VASE_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = VASE_MODEL_SHADOW;
	g_DefaultObjectData.insert({ VASE_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(VASE_MODEL_NAME);

	tmpData.pstrObjectPath = WALL_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = WALL_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_MODEL_EXTENTS;
	tmpData.objectType = WALL_MODEL_TYPE;
	tmpData.colliderType = WALL_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_MODEL_SCALE;
	tmpData.renderLayer = WALL_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(-tmpData.xmf3Extents.x, tmpData.xmf3Extents.y, 0.08f);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = WALL_MODEL_SHADOW;
	g_DefaultObjectData.insert({ WALL_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_MODEL_NAME);

	tmpData.pstrObjectPath = WALL_WITH_WINDOWS_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = WALL_WITH_WINDOWS_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_WITH_WINDOWS_MODEL_EXTENTS;
	tmpData.objectType = WALL_WITH_WINDOWS_MODEL_TYPE;
	tmpData.colliderType = WALL_WITH_WINDOWS_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_WITH_WINDOWS_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_WITH_WINDOWS_MODEL_SCALE;
	tmpData.renderLayer = WALL_WITH_WINDOWS_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = WALL_WITH_WINDOWS_MODEL_SHADOW;
	g_DefaultObjectData.insert({ WALL_WITH_WINDOWS_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_WITH_WINDOWS_MODEL_NAME);

	tmpData.pstrObjectPath = WALL_PILLAR_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = WALL_PILLAR_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_PILLAR_MODEL_EXTENTS;
	tmpData.objectType = WALL_PILLAR_MODEL_TYPE;
	tmpData.colliderType = WALL_PILLAR_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_PILLAR_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_PILLAR_MODEL_SCALE;
	tmpData.renderLayer = WALL_PILLAR_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = WALL_PILLAR_MODEL_SHADOW;
	g_DefaultObjectData.insert({ WALL_PILLAR_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_PILLAR_MODEL_NAME);

	tmpData.pstrObjectPath = WALL_PILLAR_3_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = WALL_PILLAR_3_MODEL_TEXPATH;
	tmpData.xmf3Extents = WALL_PILLAR_3_MODEL_EXTENTS;
	tmpData.objectType = WALL_PILLAR_3_MODEL_TYPE;
	tmpData.colliderType = WALL_PILLAR_3_MODEL_COLLIDERTYPE;
	tmpData.nMass = WALL_PILLAR_3_MODEL_MASS;
	tmpData.xmf3OffsetScale = WALL_PILLAR_3_MODEL_SCALE;
	tmpData.renderLayer = WALL_PILLAR_3_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0.1f, tmpData.xmf3Extents.y, -0.1f);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = WALL_PILLAR_3_MODEL_SHADOW;
	g_DefaultObjectData.insert({ WALL_PILLAR_3_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(WALL_PILLAR_3_MODEL_NAME);

	tmpData.pstrObjectPath = PLATFORM_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = PLATFORM_MODEL_TEXPATH;
	tmpData.xmf3Extents = PLATFORM_MODEL_EXTENTS;
	tmpData.objectType = PLATFORM_MODEL_TYPE;
	tmpData.colliderType = PLATFORM_MODEL_COLLIDERTYPE;
	tmpData.nMass = PLATFORM_MODEL_MASS;
	tmpData.xmf3OffsetScale = PLATFORM_MODEL_SCALE;
	tmpData.renderLayer = PLATFORM_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0.0f, tmpData.xmf3Extents.y, 0.0f);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = PLATFORM_MODEL_SHADOW;
	g_DefaultObjectData.insert({ PLATFORM_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(PLATFORM_MODEL_NAME);
	
	tmpData.pstrObjectPath = ITEM_MODEL_OBJECTPATH;
	tmpData.pstrTexPath = ITEM_MODEL_TEXPATH;
	tmpData.xmf3Extents = ITEM_MODEL_EXTENTS;
	tmpData.objectType = ITEM_MODEL_TYPE;
	tmpData.colliderType = ITEM_MODEL_COLLIDERTYPE;
	tmpData.nMass = ITEM_MODEL_MASS;
	tmpData.xmf3OffsetScale = ITEM_MODEL_SCALE;
	tmpData.renderLayer = ITEM_MODEL_RENDERLAYER;
	tmpData.xmf3MeshOffsetPosition = XMFLOAT3(0, tmpData.xmf3Extents.y, 0);
	tmpData.xmf3MeshOffsetRotation = XMFLOAT3(0, 0, 0);
	tmpData.bShadowed = ITEM_MODEL_SHADOW;
	g_DefaultObjectData.insert({ ITEM_MODEL_NAME, tmpData });
	g_DefaultObjectNames.emplace_back(ITEM_MODEL_NAME);
}
