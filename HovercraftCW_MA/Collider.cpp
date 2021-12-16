#include "Collider.h"
vec3 CAxisAlignedBoxCollider::sm_vec3DefaultExtents = { 1,1,1 };

#pragma region Main Collider
CCollider::CCollider(vec3* pvec3PositionReference)
{
	this->m_pvec3ObjectPosition = pvec3PositionReference;

	glGenVertexArrays(1, &m_uiVertexArrayObject);
	glGenBuffers(1, &m_uiVertexBufferObject);

	glBindVertexArray(this->m_uiVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_uiVertexBufferObject);
}

CCollider::~CCollider()
{
}

void CCollider::DrawShape()
{
	std::cout << "This is an abstarct collider and has no dimensions\n";
}

void CCollider::GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint) {
}

#pragma endregion

#pragma region Sphere collider
CSphereCollider::CSphereCollider(vec3* pvec3PositionReference) : CCollider(pvec3PositionReference)
{
	this->m_pobjrSphere = new objReader();
	this->m_pobjrSphere->LoadModel((char*)"../Geometry/Sphere.obj");

	glBufferData(GL_ARRAY_BUFFER, sizeof(fVertex) * this->m_pobjrSphere->numVertices, this->m_pobjrSphere->v, GL_STREAM_DRAW);

	//coorindates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(fVertex), 0);
	glEnableVertexAttribArray(0);
}

CSphereCollider::~CSphereCollider()
{
	delete this->m_pobjrSphere;
}

void CSphereCollider::GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint)
{
	rvec3MinPoint = {this->m_pvec3ObjectPosition->x - this->m_fRadius ,0, this->m_pvec3ObjectPosition->z - this->m_fRadius};
	rvecMaxPoint = { this->m_pvec3ObjectPosition->x + this->m_fRadius ,0, this->m_pvec3ObjectPosition->z + this->m_fRadius };
}

void CSphereCollider::DrawShape()
{
	glBindVertexArray(this->m_uiVertexArrayObject);
	glDrawArrays(GL_LINE_STRIP, 0, this->m_pobjrSphere->numVertices);
}
#pragma endregion

#pragma region AABB

CAxisAlignedBoxCollider::CAxisAlignedBoxCollider(vec3* pvec3PositionReference, vec3& rvec3Extents) : CCollider(pvec3PositionReference)
{
	this->m_vec3Extents = rvec3Extents;

	this->m_rvec4Vertices[0] = glm::vec4(this->m_vec3Extents.x, this->m_vec3Extents.y, this->m_vec3Extents.z, 1);
	this->m_rvec4Vertices[1] = glm::vec4(this->m_vec3Extents.x, this->m_vec3Extents.y, -this->m_vec3Extents.z, 1);
	this->m_rvec4Vertices[2] = glm::vec4(this->m_vec3Extents.x, -this->m_vec3Extents.y, this->m_vec3Extents.z, 1);
	this->m_rvec4Vertices[3] = glm::vec4(this->m_vec3Extents.x, -this->m_vec3Extents.y, -this->m_vec3Extents.z, 1);

	this->m_rvec4Vertices[4] = glm::vec4(-this->m_vec3Extents.x, this->m_vec3Extents.y, this->m_vec3Extents.z, 1);
	this->m_rvec4Vertices[5] = glm::vec4(-this->m_vec3Extents.x, this->m_vec3Extents.y, -this->m_vec3Extents.z, 1);
	this->m_rvec4Vertices[6] = glm::vec4(-this->m_vec3Extents.x, -this->m_vec3Extents.y, +this->m_vec3Extents.z, 1);
	this->m_rvec4Vertices[7] = glm::vec4(-this->m_vec3Extents.x, -this->m_vec3Extents.y, -this->m_vec3Extents.z, 1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 8, this->m_rvec4Vertices, GL_STREAM_DRAW);
	
	//coorindates
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
	glEnableVertexAttribArray(0);
	
}

CAxisAlignedBoxCollider::~CAxisAlignedBoxCollider()
{
}

void CAxisAlignedBoxCollider::DrawShape()
{

	//pass a new model matrix that just moves but doesn't rotate
	glBindVertexArray(this->m_uiVertexArrayObject);
	
	glDrawArrays(GL_LINES, 0, 8);
}

void CAxisAlignedBoxCollider::GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvecMaxPoint)
{
	rvec3MinPoint = {this->m_pvec3ObjectPosition->x - this->m_vec3Extents.x, 0, this->m_pvec3ObjectPosition->z - this->m_vec3Extents.z };
	rvecMaxPoint = { this->m_pvec3ObjectPosition->x + this->m_vec3Extents.x, 0, this->m_pvec3ObjectPosition->z + this->m_vec3Extents.z };
}
#pragma endregion

#pragma region OBB
COrientatedBox::COrientatedBox(vec3* pvec3PositionReference, vec3* pvec3Orientation, vec3& rvec3Extents) : CAxisAlignedBoxCollider(pvec3PositionReference, rvec3Extents)
{
	this->m_pvec3Orientation = pvec3Orientation;
}

COrientatedBox::~COrientatedBox()
{
}

void COrientatedBox::GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvec3MaxPoint)
{
	//rotate every vertex
	//go trhough every one of them and get the biggest and smallest x and y

	float fSinY = glm::sin(glm::radians(this->m_pvec3Orientation->y));
	float fCosY = glm::cos(glm::radians(this->m_pvec3Orientation->y));
	glm::mat4 mat4AroundY(
		fCosY,	0.f,	fSinY,	0.f,
		0.f,	1.f,	0.f,	0.f,
		-fSinY,	0.f,	fCosY,	0.f,
		0.f,	0.f,	0.f,	1.f);


	glm::vec4 rvec4RotatedVertices[8];

	for (size_t i = 0; i < 8; i++)
	{
		rvec4RotatedVertices[i] =  this->m_rvec4Vertices[i] * mat4AroundY;
	}

	float fMinX, fMaxX, fMinZ, fMaxZ;
	fMinX = fMinZ = FLT_MAX;
	fMaxX = fMaxZ = -FLT_MAX;

	for (auto vertex: rvec4RotatedVertices)
	{
		if (vertex.x > fMaxX)
			fMaxX = vertex.x;

		else if (vertex.x < fMinX)
			fMinX = vertex.x;

		if (vertex.z > fMaxZ)
			fMaxZ = vertex.z;

		else if (vertex.z < fMinZ)
			fMinZ = vertex.z;
	}

	rvec3MinPoint = { this->m_pvec3ObjectPosition->x + fMinX, 0, this->m_pvec3ObjectPosition->z + fMinZ };
	rvec3MaxPoint = { this->m_pvec3ObjectPosition->x + fMaxX, 0, this->m_pvec3ObjectPosition->z + fMaxZ };

}

void COrientatedBox::DrawShape()
{
	glBindVertexArray(this->m_uiVertexArrayObject);

	glDrawArrays(GL_LINES, 0, 8);
}

void COrientatedBox::GetClosestPoint(vec3& rvec3External, vec3& rvec3Internal)
{
	vec3 vec3Normal = rvec3External - *this->m_pvec3ObjectPosition;

	rvec3Internal = *this->m_pvec3ObjectPosition;

	glm::mat4 mat4Rotations1 = glm::yawPitchRoll(this->m_pvec3Orientation->y, this->m_pvec3Orientation->x, this->m_pvec3Orientation->z);
	glm::vec3 vec3AxisX = vec3(glm::vec4(1, 0, 0, 1) * mat4Rotations1);
	glm::vec3 vec3AxisY = vec3(glm::vec4(0, 1, 0, 1) * mat4Rotations1);
	glm::vec3 vec3AxisZ = vec3(glm::vec4(0, 0, 1, 1) * mat4Rotations1);

	float fDistanceOnX, fDistanceOnY, fDistanceOnZ;

	fDistanceOnX = glm::dot(vec3Normal, vec3AxisX);
	if (fDistanceOnX > this->m_vec3Extents.x)
		fDistanceOnX = this->m_vec3Extents.x;

	if (fDistanceOnX < -this->m_vec3Extents.x)
		fDistanceOnX = -this->m_vec3Extents.x;

	rvec3Internal += fDistanceOnX * vec3AxisX;

	fDistanceOnY = glm::dot(vec3Normal, vec3AxisY);
	if (fDistanceOnY > this->m_vec3Extents.y)
		fDistanceOnY = this->m_vec3Extents.y;

	if (fDistanceOnY < -this->m_vec3Extents.y)
		fDistanceOnY = -this->m_vec3Extents.y;

	rvec3Internal += fDistanceOnY * vec3AxisY;

	fDistanceOnZ = glm::dot(vec3Normal, vec3AxisZ);
	if (fDistanceOnZ > this->m_vec3Extents.z)
		fDistanceOnZ = this->m_vec3Extents.z;

	if (fDistanceOnZ < -this->m_vec3Extents.z)
		fDistanceOnZ = -this->m_vec3Extents.z;

	rvec3Internal += fDistanceOnZ * vec3AxisZ;
}
#pragma endregion

#pragma region Plane
CPlaneCollider::CPlaneCollider(vec3* pvec3PositionReference, vec3& rvec3Normal) : CCollider(pvec3PositionReference)
{
	this->m_vec3Normal = rvec3Normal;

	this->m_vec4Vertices[0] = glm::vec4(*this->m_pvec3ObjectPosition, 1);
	this->m_vec4Vertices[1] = glm::vec4(*this->m_pvec3ObjectPosition+3.f*this->m_vec3Normal, 1);

	this->m_vec4Vertices[0] = glm::vec4(*(this->m_pvec3ObjectPosition), 1);
	this->m_vec4Vertices[1] = glm::vec4(*(this->m_pvec3ObjectPosition) + this->m_vec3Normal*3.f, 1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 2, this->m_vec4Vertices, GL_STREAM_DRAW);

	//coorindates
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
	glEnableVertexAttribArray(0);
}

CPlaneCollider::~CPlaneCollider()
{
}

void CPlaneCollider::GetMinAndMaxPoints(vec3& rvec3MinPoint, vec3& rvec3MaxPoint)
{
	glm::mat3 mat3RotationAroundY(
		0, 0, 1,
		0, 1, 0,
		-1, 0, 0
	);

	vec3 one, two;
	one = two = this->m_vec3Normal * mat3RotationAroundY * 100.f;
	//to invert it
	one *= -1;

	one += *this->m_pvec3ObjectPosition;
	two += *this->m_pvec3ObjectPosition;

	rvec3MaxPoint.x = one.x > two.x ? one.x : two.x;
	rvec3MaxPoint.y = one.y > two.y ? one.y : two.y;
	rvec3MaxPoint.z = one.z > two.z ? one.z : two.z;
	
	rvec3MinPoint.x = one.x < two.x ? one.x : two.x;
	rvec3MinPoint.y = one.y < two.y ? one.y : two.y;
	rvec3MinPoint.z = one.z < two.z ? one.z : two.z;
}

void CPlaneCollider::DrawShape()
{
	glBindVertexArray(this->m_uiVertexArrayObject);

	glDrawArrays(GL_LINES, 0, 2);

}
#pragma endregion