#ifndef _OGRE_BULLET_CONVERT_H_
#define _OGRE_BULLET_CONVERT_H_

#include "LinearMath/btQuaternion.h"

#include "OgreVector3.h"

inline btVector3 convert(const Ogre::Vector3 &V)
{
    return btVector3(V.x, V.y, V.z);
}
 
inline Ogre::Vector3 convert(const btVector3&V)
{
    return Ogre::Vector3(V.x(), V.y(), V.z());
}
 
inline btQuaternion convert(const Ogre::Quaternion &Q)
{
    return btQuaternion(Q.x, Q.y, Q.z, Q.w);
}
 
inline Ogre::Quaternion convert(const btQuaternion &Q)
{
    return Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());
}

/// Rotation of a vector by a quaternion
inline btVector3 rotate(const btVector3& v, const btQuaternion& q)
{
	// nVidia SDK implementation
	// Found in OgreQuaternion.cpp
	btVector3 uv, uuv;
	btVector3 qvec(q.x(), q.y(), q.z());
	uv = qvec.cross(v);
	uuv = qvec.cross(uv);
	uv *= (2.0f * q.w());
	uuv *= 2.0f;

	return v + uv + uuv;
}

#endif //_OGRE_BULLET_CONVERT_H_