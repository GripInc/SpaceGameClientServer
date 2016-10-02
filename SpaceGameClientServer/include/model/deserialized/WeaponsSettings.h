#ifndef _WEAPONS_SETTINGS_H_
#define _WEAPONS_SETTINGS_H_

#include "model/deserialized/SystemSettings.h"
#include "model/deserialized/CollisionShapeSettings.h"
#include "model/deserialized/HardPointSettings.h"

#include "LinearMath/btAlignedObjectArray.h"

class WeaponSettings : public SystemSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		bool success = true;

		success &= SystemSettings::deserialize(_node);

		XMLHelper::getStringFrom(_node, "mesh", mMesh);

		XMLHelper::getStringFrom(_node, "shotType", mShotType);
		XMLHelper::getFloatFrom(_node, "fireRate", mFireRate);

		float tempNoslePosX, tempNoslePosY, tempNoslePosZ;
		XMLHelper::getFloatFrom(_node, "noslePosX", tempNoslePosX);
		XMLHelper::getFloatFrom(_node, "noslePosY", tempNoslePosY);
		XMLHelper::getFloatFrom(_node, "noslePosZ", tempNoslePosZ);
		mNoslePosition = btVector3(tempNoslePosX, tempNoslePosY, tempNoslePosZ);

		XMLHelper::getUnsignedIntFrom(_node, "hitPoints", mHitPoints);
		XMLHelper::getFloatFrom(_node, "consumption", mConsumption);

		//HardPoint
		tinyxml2::XMLElement* hardPointNode = _node->FirstChildElement("HardPoint");
		if(hardPointNode)
			mHardPointSettings.deserialize(hardPointNode);
		else
			success = false;

		//CollisionShapes
		tinyxml2::XMLElement* collisionShape = _node->FirstChildElement("CollisionShape");
		while(collisionShape != NULL)
		{
			mCollisionShapes.push_back(CollisionShapeSettings());
			if(mCollisionShapes[mCollisionShapes.size() - 1].deserialize(collisionShape))
				collisionShape = collisionShape->NextSiblingElement("CollisionShape");
			else
				success = false;
		}

		return success;
	}

	const std::string& getShotType() const { return mShotType; }
	const std::string& getMesh() const { return mMesh; }
	float getFireRate() const { return mFireRate; }
	const btVector3& getNoslePosition() const { return mNoslePosition; }
	const HardPointSettings& getHardPointSettings() const { return mHardPointSettings; }
	unsigned int getHitPoints() const { return mHitPoints; }
	float getConsumption() const { return mConsumption; }
	const btAlignedObjectArray<CollisionShapeSettings>& getCollisionShapes() const { return mCollisionShapes; }

protected:
	std::string mShotType;
	std::string mMesh;
	float mFireRate = 0.f; //in sec
	btVector3 mNoslePosition;
	HardPointSettings mHardPointSettings;
	unsigned int mHitPoints = 0U;
	float mConsumption = 0.f;

	btAlignedObjectArray<CollisionShapeSettings> mCollisionShapes;
};

class WeaponsSettings
{
public:
	virtual bool deserialize(tinyxml2::XMLElement* _node)
	{
		tinyxml2::XMLElement* weaponNode = _node->FirstChildElement("Weapon");
		while(weaponNode != NULL)
		{
			mWeapons.push_back(WeaponSettings());
			if(mWeapons[mWeapons.size() - 1].deserialize(weaponNode))
				weaponNode = weaponNode->NextSiblingElement("Weapon");
			else
				return false;
		}

		return true;
	}

	const WeaponSettings* getWeaponSettings(const std::string& _weapon) const
	{
		for(int i = 0; i < mWeapons.size(); ++i)
			if(mWeapons[i].getName().compare(_weapon) == 0)
				return &mWeapons[i];

		return NULL;
	}

	btAlignedObjectArray<WeaponSettings> mWeapons;
};

#endif //_WEAPONS_SETTINGS_H_