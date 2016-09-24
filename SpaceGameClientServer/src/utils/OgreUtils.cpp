#include "utils/OgreUtils.h"

#include "OgreSceneNode.h"
#include "OgreSubMesh.h"
#include "OgreMesh.h"
#include "OgreManualObject.h"

void OgreUtils::destroyAllAttachedMovableObjects(Ogre::SceneNode* node )
{
   if(!node)
	   return;

   // Destroy all the attached objects
   Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

   while ( itObject.hasMoreElements() )
      node->getCreator()->destroyMovableObject(itObject.getNext());

   // Recurse to child SceneNodes
   Ogre::SceneNode::ChildNodeIterator itChild = node->getChildIterator();

   while ( itChild.hasMoreElements() )
   {
      Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
      destroyAllAttachedMovableObjects( pChildNode );
   }
}

void OgreUtils::destroySceneNode( Ogre::SceneNode* node )
{
   if(!node) return;
   destroyAllAttachedMovableObjects(node);
   node->removeAndDestroyAllChildren();
   node->getCreator()->destroySceneNode(node);
}

enum Ogre::SceneManager::PrefabType OgreUtils::primitiveNameToPrefabEnum(const std::string& _name)
{
	if(_name.compare("cube") == 0)
		return Ogre::SceneManager::PT_CUBE;
	else if(_name.compare("sphere") == 0)
		return Ogre::SceneManager::PT_SPHERE;
	else if(_name.compare("plane") == 0)
		return Ogre::SceneManager::PT_PLANE;

	return Ogre::SceneManager::PT_CUBE;
}

enum Ogre::Light::LightTypes OgreUtils::lightTypeNameToTypeEnum(const std::string& _name)
{
	if(_name.compare("point") == 0)
		return Ogre::Light::LT_POINT;
	else if(_name.compare("directional") == 0)
		return Ogre::Light::LT_DIRECTIONAL;
	else if(_name.compare("spotlight") == 0)
		return Ogre::Light::LT_SPOTLIGHT;

	return Ogre::Light::LT_POINT;
}

//Code from http://www.ogre3d.org/tikiwiki/RetrieveVertexData
void OgreUtils::getMeshInformation(const Ogre::Mesh* const mesh, size_t &vertex_count, Ogre::Vector3* &vertices, size_t &index_count, unsigned long* &indices, const Ogre::Vector3 &position, const Ogre::Quaternion &orient, const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;
 
    vertex_count = index_count = 0;
 
    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }
        // Add the indices
        index_count += submesh->indexData->indexCount;
    }
 
    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];
 
    added_shared = false;
 
    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
 
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
 
        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }
 
            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
 
            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
 
            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
 
            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //Ogre::Real* pReal;
            float* pReal;
 
            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }
 
            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }
 
        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
 
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
 
        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);
 
        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
 
        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }
 
        ibuf->unlock();
        current_offset = next_offset;
    }
}

Ogre::Vector3 OgreUtils::multiply(const Ogre::Quaternion& quat, const Ogre::Vector3& vec)
{
    float num = quat.x * 2.f;
    float num2 = quat.y * 2.f;
    float num3 = quat.z * 2.f;
    float num4 = quat.x * num;
    float num5 = quat.y * num2;
    float num6 = quat.z * num3;
    float num7 = quat.x * num2;
    float num8 = quat.x * num3;
    float num9 = quat.y * num3;
    float num10 = quat.w * num;
    float num11 = quat.w * num2;
    float num12 = quat.w * num3;
    Ogre::Vector3 result;
    result.x = (1.f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z;
    result.y = (num7 + num12) * vec.x + (1.f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z;
    result.z = (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.f - (num4 + num5)) * vec.z;
    return result;
}

void OgreUtils::createSphere(Ogre::SceneManager& sceneManager, const std::string& strName, const float r, const int nRings, const int nSegments)
{
	Ogre::ManualObject * manual = sceneManager.createManualObject(strName);
	manual->begin("", Ogre::RenderOperation::OT_TRIANGLE_LIST);
 
	float fDeltaRingAngle = (Ogre::Math::PI / nRings);
	float fDeltaSegAngle = (2 * Ogre::Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;
 
	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);
 
		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);
 
			// Add one vertex to the strip which makes up the sphere
			manual->position( x0, y0, z0);
			manual->normal(Ogre::Vector3(x0, y0, z0).normalisedCopy());
			manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);
 
			if (ring != nRings) {
				// each vertex (except the last) has six indicies pointing to it
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex);               
				manual->index(wVerticeIndex + nSegments);
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex + 1);
				manual->index(wVerticeIndex);
				wVerticeIndex ++;
				}
		}; // end for seg
	} // end for ring
	manual->end();
	Ogre::MeshPtr mesh = manual->convertToMesh(strName);
	mesh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-r, -r, -r), Ogre::Vector3(r, r, r)), false );
 
	mesh->_setBoundingSphereRadius(r);
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
	{
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
	}
}