#include <glm/glm.hpp>
#include <iostream>
#include <GL\glew.h>
#include "objReader.h"
#include <glm\gtx\euler_angles.hpp>

#pragma once

using glm::vec3;

class CGameObject;

class CCollider
{
public:
	CCollider(vec3* pvec3PositionReference);
	~CCollider();
	virtual void DrawShape();
	virtual void GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint);

	unsigned int m_uiVertexArrayObject;
	unsigned int m_uiVertexBufferObject;
	vec3* m_pvec3ObjectPosition;

	void (*OnTriggerEnter)(CGameObject * pgoObject);

	bool m_bIsTrigger = false;

};

class CPlaneCollider : public CCollider {
public:
	CPlaneCollider(vec3* pvec3PositionReference, vec3& rvec3Normal);
	~CPlaneCollider();
	void GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint);

	void DrawShape();

	vec3 m_vec3Normal;
	glm::vec4 m_vec4Vertices[2];

};

class CSphereCollider : public CCollider
{
public:

	CSphereCollider(vec3* pvec3PositionReference);
	~CSphereCollider();

	void GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint);

	void DrawShape();
	float m_fRadius = 1;

private:
	objReader* m_pobjrSphere;
};

class CAxisAlignedBoxCollider :public CCollider
{
public:
	CAxisAlignedBoxCollider(vec3* pvec3PositionReference, vec3& rvec3Extents = CAxisAlignedBoxCollider::sm_vec3DefaultExtents);
	~CAxisAlignedBoxCollider();

	void DrawShape();

	void GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint);

	//Half of the edge used to callculate all four points
	//X - Width
	//Y - Height
	//Z - Depth
	vec3 m_vec3Extents;

	glm::vec4 m_rvec4Vertices[8];

protected:
	 static vec3 sm_vec3DefaultExtents;
};

class COrientatedBox : public CAxisAlignedBoxCollider
{
public:
	COrientatedBox(vec3* pvec3PositionReference, vec3* pvecOrientation, vec3& rvec3Extents = COrientatedBox::sm_vec3DefaultExtents);
	~COrientatedBox();

	void GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint);

	void DrawShape();

	//Gets the closest point within the obb tp a given point
	//Refernce: Book: Real time collision by Christer Ericson
	void GetClosestPoint(vec3& rvec3External, vec3& rvec3Internal);

	vec3* m_pvec3Orientation;
};

struct SCollisionData
{
	bool m_bIsThereCollision;
	glm::vec3 m_vec3CollisionCenter;
	float m_fCollisionDepth = 0;
};