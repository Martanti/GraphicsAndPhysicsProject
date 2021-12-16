#include "CollisionCalculator.h"

//https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
//was used as in inspiration for some of the calculations

const float CCollisionCalculator::sm_kfLengthOfCell = 10.f;
float CCollisionCalculator::sm_InvertedLengthOfCell = 1 / CCollisionCalculator::sm_kfLengthOfCell;

void CCollisionCalculator::RecalculateCollision(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadable)
{
	//Create new Tag "Collides" that will have all the objects in the scene with a collider
	this->Partition(rvgoWritable, rvgoReadable);
	this->CalculatePairs();
}

void CCollisionCalculator::Partition(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadable)
{
	for (auto& row : this->m_vtpgoCollisionGrid) {
		for (auto& vpgoCell : row) {
			vpgoCell.clear();
		}
	}

	this->m_setiPopulatedCellIndexes.clear();

	int iIndex = 0;
	for (CGameObject* pgoReadable : rvgoReadable)
	{
		vec3 vec3MinPoint, vec3MaxPoint;
		int iXMin, iXMax, iZMin, iZMax;

		pgoReadable->m_pcolCollider->GetMinAndMaxPoints(vec3MinPoint, vec3MaxPoint);

		vec3MinPoint -= this->m_vec3UpperLeftPoint;
		vec3MaxPoint -= this->m_vec3UpperLeftPoint;

		iXMin = vec3MinPoint.x * CCollisionCalculator::sm_InvertedLengthOfCell;
		iZMin = vec3MinPoint.z * CCollisionCalculator::sm_InvertedLengthOfCell;

		iXMax = vec3MaxPoint.x * CCollisionCalculator::sm_InvertedLengthOfCell;
		iZMax = vec3MaxPoint.z * CCollisionCalculator::sm_InvertedLengthOfCell;

		//also checks not to go above grid size
		int iGridSize = CCollisionCalculator::sm_kuiGridSize;
		for (int iXIndex = iXMin; iXIndex <= iXMax && iXIndex < iGridSize; iXIndex++)
		{
			if (iXIndex < 0)
				continue;

			for (int iZindex = iZMin; iZindex <= iZMax && iZindex < iGridSize; iZindex++)
			{
				if (iZindex < 0)
					continue;

				this->m_vtpgoCollisionGrid[iXIndex][iZindex].emplace_back( std::make_tuple(pgoReadable, rvgoWritable[iIndex]) );

				this->m_setiPopulatedCellIndexes.insert({ iXIndex, iZindex });
			}
		}

		iIndex++;
	}
}

void CCollisionCalculator::CalculatePairs()
{
	//both objects are over the border and are in multiple grid this map will allow inserting both their pointers and checking that they have already collided this will allow to avoid duplicate responses

	map<tuple<CGameObject*, CGameObject*>, bool> mAlreadyCollidedItems;

	for (tuple<int, int> tplIndexes : this->m_setiPopulatedCellIndexes)
	{
		int iX, iZ;
		iX = std::get<0>(tplIndexes);
		iZ = std::get<1>(tplIndexes);

		vector<tuple<CGameObject*, CGameObject*>> vpgoCell = this->m_vtpgoCollisionGrid[iX][iZ];

		if (vpgoCell.size() <= 1)
			continue;

		map<tuple<CGameObject*, CGameObject*>, bool> mAlreadyAddedItems;

		for (auto tpgoPair1 : vpgoCell) {
			for (auto tpgoPair2 : vpgoCell) {

				CGameObject *pgoReadable1, *pgoWritable1, *pgoReadable2, *pgoWritable2;

				pgoReadable1 = std::get<0>(tpgoPair1);
				pgoWritable1 = std::get<1>(tpgoPair1);

				pgoReadable2 = std::get<0>(tpgoPair2);
				pgoWritable2 = std::get<1>(tpgoPair2);

				if (pgoReadable1->m_bIsStatic && pgoReadable2->m_bIsStatic)
					continue;

				if (mAlreadyAddedItems[tpgoPair2])
					continue;

				if (mAlreadyCollidedItems[{pgoReadable1, pgoReadable2}] || mAlreadyCollidedItems[{pgoReadable2, pgoReadable1}])
					continue;

				if (tpgoPair1 != tpgoPair2)
				{
					SCollisionData coldtInfo;

					this->CallCorrectCollisionCallculation(*pgoReadable1->m_pcolCollider, *pgoReadable2->m_pcolCollider, coldtInfo);

					if (coldtInfo.m_bIsThereCollision)
					{
						mAlreadyCollidedItems[{pgoReadable1, pgoReadable2}] = true;

						if (pgoReadable1->m_pcolCollider->m_bIsTrigger || pgoReadable2->m_pcolCollider->m_bIsTrigger)
						{
							if (pgoReadable1->m_pcolCollider->m_bIsTrigger)
								pgoWritable1->m_pcolCollider->OnTriggerEnter(pgoWritable2);

							if (pgoReadable2->m_pcolCollider->m_bIsTrigger)
								pgoWritable2->m_pcolCollider->OnTriggerEnter(pgoWritable1);
						}

						else {

							this->CollisionResolution(pgoWritable1, pgoReadable1, pgoWritable2, pgoReadable2, coldtInfo);
						}

					}
				}
			}

			mAlreadyAddedItems[tpgoPair1] = true;
		}
	}
}

void CCollisionCalculator::CallCorrectCollisionCallculation(CCollider& colBaseCollider1, CCollider& colBaseCollider2, SCollisionData& rcoldtInfo)
{
	//just two alternatives
	//this->SweepThrough(colBaseCollider1, colBaseCollider2, rcoldtInfo);
	this->SwitchThrough(colBaseCollider1, colBaseCollider2, rcoldtInfo);
}

void CCollisionCalculator::SwitchThrough(CCollider& colBaseCollider1, CCollider& colBaseCollider2, SCollisionData& rcoldtInfo)
{
	size_t stClassHash1 = typeid(colBaseCollider1).hash_code();
	size_t stClassHash2 = typeid(colBaseCollider2).hash_code();

	switch (stClassHash1)
	{
	case(CCollisionCalculator::sm_kstPlaneCollider):
		switch (stClassHash2)
		{
		case(CCollisionCalculator::sm_kstOrientatedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<CPlaneCollider*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstAxisAllignedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<CPlaneCollider*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstSphereCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<CPlaneCollider*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
			break;

		default:
			break;
		}

		break;

	case(CCollisionCalculator::sm_kstOrientatedBox):
		switch (stClassHash2)
		{
		case(CCollisionCalculator::sm_kstPlaneCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<CPlaneCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstOrientatedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstAxisAllignedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstSphereCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
			break;

		default:
			break;
		}
		break;

	case(CCollisionCalculator::sm_kstAxisAllignedBox):
		switch (stClassHash2)
		{
		case(CCollisionCalculator::sm_kstPlaneCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<CPlaneCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstOrientatedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstAxisAllignedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstSphereCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
			break;

		default:
			break;
		}
		break;

	case(CCollisionCalculator::sm_kstSphereCollider):
		switch (stClassHash2)
		{
		case(CCollisionCalculator::sm_kstPlaneCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<CPlaneCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstOrientatedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstAxisAllignedBox):
			rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
			break;

		case(CCollisionCalculator::sm_kstSphereCollider):
			rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

}

void CCollisionCalculator::SweepThrough(CCollider& colBaseCollider1, CCollider& colBaseCollider2, SCollisionData& rcoldtInfo)
{
	size_t stClassHash1 = typeid(colBaseCollider1).hash_code();
	size_t stClassHash2 = typeid(colBaseCollider2).hash_code();

	//plane vs plane is skipped as planes will be static and have no reason to pass 

	if (stClassHash1 == CCollisionCalculator::sm_kstPlaneCollider && stClassHash2 == CCollisionCalculator::sm_kstOrientatedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CPlaneCollider*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstOrientatedBox && stClassHash2 == CCollisionCalculator::sm_kstPlaneCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<CPlaneCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstOrientatedBox && stClassHash2 == CCollisionCalculator::sm_kstOrientatedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstPlaneCollider && stClassHash2 == CCollisionCalculator::sm_kstAxisAllignedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CPlaneCollider*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstAxisAllignedBox && stClassHash2 == CCollisionCalculator::sm_kstPlaneCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<CPlaneCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstOrientatedBox && stClassHash2 == CCollisionCalculator::sm_kstAxisAllignedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstAxisAllignedBox && stClassHash2 == CCollisionCalculator::sm_kstOrientatedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstOrientatedBox && stClassHash2 == CCollisionCalculator::sm_kstSphereCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<COrientatedBox*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstSphereCollider && stClassHash2 == CCollisionCalculator::sm_kstOrientatedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<COrientatedBox*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstPlaneCollider && stClassHash2 == CCollisionCalculator::sm_kstSphereCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CPlaneCollider*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstSphereCollider && stClassHash2 == CCollisionCalculator::sm_kstPlaneCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<CPlaneCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstSphereCollider && stClassHash2 == CCollisionCalculator::sm_kstSphereCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstSphereCollider && stClassHash2 == CCollisionCalculator::sm_kstAxisAllignedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CSphereCollider*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstAxisAllignedBox && stClassHash2 == CCollisionCalculator::sm_kstSphereCollider) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<CSphereCollider*>(&colBaseCollider2));
		return;
	}

	else if (stClassHash1 == CCollisionCalculator::sm_kstAxisAllignedBox && stClassHash2 == CCollisionCalculator::sm_kstAxisAllignedBox) {
		rcoldtInfo = this->GetCollisionData(*static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider1), *static_cast<CAxisAlignedBoxCollider*>(&colBaseCollider2));
		return;
	}
}

void CCollisionCalculator::CollisionResolution(CGameObject* pgoWritable1, CGameObject* pgoReadable1, CGameObject* pgoWritable2, CGameObject* pgoReadable2, SCollisionData& rcoldtInfo)
{
	vec3 vec3Object1Velocity = pgoReadable1->m_pbdPhysicalBody != nullptr ? pgoReadable1->m_pbdPhysicalBody->m_vec3Velocity : vec3(0, 0, 0);
	vec3 vec3Object2Velocity = pgoReadable2->m_pbdPhysicalBody != nullptr ? pgoReadable2->m_pbdPhysicalBody->m_vec3Velocity : vec3(0, 0, 0);

	vec3 vec3VelocityDifference = vec3Object1Velocity - vec3Object2Velocity;

	vec3 vec3CollisionNormal = glm::normalize(pgoReadable1->m_vec3Position - rcoldtInfo.m_vec3CollisionCenter);

	if (isnan(vec3CollisionNormal.x) && isnan(vec3CollisionNormal.y) && isnan(vec3CollisionNormal.z))
	{
		//sometimes center and collision normal become same and everything craps out
		//so to prevent division by 0 set it to very small vector
		vec3CollisionNormal = { 0.001,0,0 };
	}

	//Co-efficient of restitution                                                                                                                                                          
	float fCollisionCoeficient = 0.5f;

	float fObject1InvertedMass = pgoReadable1->m_pbdPhysicalBody != nullptr ? pgoReadable1->m_pbdPhysicalBody->m_fInvertedMass : 0;
	float fObject2InvertedMass = pgoReadable2->m_pbdPhysicalBody != nullptr ? pgoReadable2->m_pbdPhysicalBody->m_fInvertedMass : 0;
	
	CRotattingBody* prbReadable1 = static_cast<CRotattingBody*>(pgoReadable1->m_pbdPhysicalBody);
	CRotattingBody* prbReadable2 = static_cast<CRotattingBody*>(pgoReadable2->m_pbdPhysicalBody);

	mat3 mat3InvertedInertia1 = prbReadable1!= nullptr ? prbReadable1->m_mat3InvertedInertia : mat3(1) ;
	mat3 mat3InvertedInertia2 = prbReadable2 != nullptr ? prbReadable2->m_mat3InvertedInertia : mat3(1);;

	vec3 vec3Radius1 = rcoldtInfo.m_vec3CollisionCenter - pgoReadable1->m_vec3Position;
	
	vec3 vec3Radius2 = rcoldtInfo.m_vec3CollisionCenter - pgoReadable2->m_vec3Position;

	float fCollisionResponse = (glm::dot(-vec3VelocityDifference, vec3CollisionNormal) * (1.f + fCollisionCoeficient))
		/ (glm::dot(vec3CollisionNormal, vec3CollisionNormal) * (fObject1InvertedMass + fObject2InvertedMass) +
		glm::dot( glm::cross(mat3InvertedInertia1 * glm::cross(vec3Radius1, vec3CollisionNormal), vec3Radius1  ) +  glm::cross(mat3InvertedInertia1 * glm::cross(vec3Radius2, vec3CollisionNormal), vec3Radius2  ) , vec3CollisionNormal));

	if (!pgoReadable1->m_bIsStatic && pgoReadable1->m_pbdPhysicalBody != nullptr) {

		pgoWritable1->m_vec3Position = pgoReadable1->m_vec3Position +  vec3CollisionNormal * rcoldtInfo.m_fCollisionDepth;
		pgoWritable1->m_pbdPhysicalBody->m_vec3Velocity = fCollisionResponse * pgoReadable1->m_pbdPhysicalBody->m_fInvertedMass * vec3CollisionNormal;

		if (prbReadable1 != nullptr){
			CRotattingBody* prbWritable = static_cast<CRotattingBody*>(pgoWritable1->m_pbdPhysicalBody);
			prbWritable->m_vec3AngularVelocity = prbReadable1->m_vec3AngularVelocity +  prbReadable1->m_mat3InvertedInertia * glm::cross({0.5f,0.f, 0.5f}, fCollisionResponse * -vec3CollisionNormal);
		}
	}

	if (!pgoReadable2->m_bIsStatic && pgoReadable2->m_pbdPhysicalBody != nullptr) {
		pgoWritable2->m_vec3Position = pgoReadable2->m_vec3Position - vec3CollisionNormal * rcoldtInfo.m_fCollisionDepth;
		pgoWritable2->m_pbdPhysicalBody->m_vec3Velocity = -fCollisionResponse * pgoReadable2->m_pbdPhysicalBody->m_fInvertedMass * vec3CollisionNormal;

		if (prbReadable2 != nullptr) {
			CRotattingBody* prbWritable = static_cast<CRotattingBody*>(pgoWritable2->m_pbdPhysicalBody);
			prbWritable->m_vec3AngularVelocity = prbReadable2->m_vec3AngularVelocity + prbReadable2->m_mat3InvertedInertia * glm::cross({ 0.5f,0.f, 0.5f }, -fCollisionResponse * vec3CollisionNormal);
		}
	}
}

void CCollisionCalculator::ObbAxisCalculation(float fAxis, float fSumOfPoints, bool& rbIsTherePlaneInBetween, float& rfPenetrationDepth)
{
	float fDifference = fSumOfPoints - fAxis;

	if (0 > fDifference) {
		rbIsTherePlaneInBetween = true;
		return;
	}

	if (fDifference < rfPenetrationDepth && fDifference > 0)
	{
		rfPenetrationDepth = fDifference;
	}
}

#pragma region Collision data calculations

SCollisionData CCollisionCalculator::GetCollisionData(CSphereCollider& rscolShereColider1, CSphereCollider& rscolSphereColider2)
{
	SCollisionData coldtInfo;

	float fDistance = glm::distance(*rscolShereColider1.m_pvec3ObjectPosition, *rscolSphereColider2.m_pvec3ObjectPosition);

	float radiiSum = (rscolShereColider1.m_fRadius + rscolSphereColider2.m_fRadius);
	if (fDistance <= radiiSum)
	{
		coldtInfo.m_bIsThereCollision = true;
		coldtInfo.m_fCollisionDepth = radiiSum - fDistance;
		//gets the vector between center points, normalizes it, times by the radious of the first center, the move it by the first center to get the offset and the offset the half of depth as it would just point to the first collision point
		coldtInfo.m_vec3CollisionCenter = glm::normalize((*rscolSphereColider2.m_pvec3ObjectPosition - *rscolShereColider1.m_pvec3ObjectPosition)) * coldtInfo.m_fCollisionDepth + *rscolShereColider1.m_pvec3ObjectPosition;
	}
	else {
		return { false };
	}

	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(CSphereCollider& rscosSphereColider, CAxisAlignedBoxCollider& raacosAxisAlignedBox)
{
	return this->GetCollisionData(raacosAxisAlignedBox, rscosSphereColider);
}

SCollisionData CCollisionCalculator::GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox, CSphereCollider& rscosSphereColider)
{
	vec3 vec3AABBMin, vec3AABBMax;
	raacosAxisAlignedBox.GetMinAndMaxPoints(vec3AABBMin, vec3AABBMax);

	//this would brake if the sphere is inside of the box

	vec3 vec3ClosestPointToSphere;
	vec3ClosestPointToSphere.x = glm::max(vec3AABBMin.x, glm::min(rscosSphereColider.m_pvec3ObjectPosition->x, vec3AABBMax.x));
	vec3ClosestPointToSphere.y = glm::max(vec3AABBMin.y, glm::min(rscosSphereColider.m_pvec3ObjectPosition->y, vec3AABBMax.y));
	vec3ClosestPointToSphere.z = glm::max(vec3AABBMin.z, glm::min(rscosSphereColider.m_pvec3ObjectPosition->z, vec3AABBMax.z));

	float fDistance = glm::length(vec3ClosestPointToSphere - *rscosSphereColider.m_pvec3ObjectPosition);

	float fDistanceDifference = rscosSphereColider.m_fRadius - fDistance;

	if (fDistance > rscosSphereColider.m_fRadius)
		return { false };

	SCollisionData coldtInfo;

	coldtInfo.m_bIsThereCollision = true;

	//this would be wrong if the center of the sphere is inside of the box
	coldtInfo.m_fCollisionDepth = glm::abs(fDistanceDifference);

	//similar to sphere x sphere
	coldtInfo.m_vec3CollisionCenter = glm::normalize(*raacosAxisAlignedBox.m_pvec3ObjectPosition - *rscosSphereColider.m_pvec3ObjectPosition) * coldtInfo.m_fCollisionDepth + *rscosSphereColider.m_pvec3ObjectPosition;
	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox1, CAxisAlignedBoxCollider& raacosAxisAlignedBox2)
{
	vec3 vec3MinFirst, vec3MaxFirst, vec3MinSecond, vec3MaxSecond;

	raacosAxisAlignedBox1.GetMinAndMaxPoints(vec3MinFirst, vec3MaxFirst);
	raacosAxisAlignedBox2.GetMinAndMaxPoints(vec3MinSecond, vec3MaxSecond);

	//separate everything into separete calculation parts which will allow to see which is highed than another and thus will allow to callculate the mid point of intersection as it will allow to calculate it's min and max point

	vec3 vec3IntersectionMaxPoint, vec3IntersectionMinPoint;

	//x axis check
	if (vec3MinFirst.x <= vec3MaxSecond.x && vec3MinSecond.x <= vec3MaxFirst.x) {
		vec3IntersectionMinPoint.x = vec3MinFirst.x;
		vec3IntersectionMaxPoint.x = vec3MaxSecond.x;
	}

	else if (vec3MaxFirst.x >= vec3MinSecond.x && vec3MinFirst.x <= vec3MaxSecond.x) {
		vec3IntersectionMinPoint.x = vec3MinSecond.x;
		vec3IntersectionMaxPoint.x = vec3MaxFirst.x;
	}

	else
		return { false };

	//z axis check

	if (vec3MinFirst.z <= vec3MaxSecond.z && vec3MinSecond.z <= vec3MaxFirst.z) {
		vec3IntersectionMinPoint.z = vec3MinFirst.z;
		vec3IntersectionMaxPoint.z = vec3MaxSecond.z;
	}

	else if (vec3MaxFirst.z >= vec3MinSecond.z && vec3MinFirst.z <= vec3MaxSecond.z) {
		vec3IntersectionMinPoint.z = vec3MinSecond.z;
		vec3IntersectionMaxPoint.z = vec3MaxFirst.z;
	}

	else
		return { false };

	//y axis check
	if (vec3MinFirst.y <= vec3MaxSecond.y && vec3MinSecond.y <= vec3MaxFirst.y) {
		vec3IntersectionMinPoint.y = vec3MinFirst.y;
		vec3IntersectionMaxPoint.y = vec3MaxSecond.y;
	}

	else if (vec3MaxFirst.y >= vec3MinSecond.y && vec3MinFirst.y <= vec3MaxSecond.y) {
		vec3IntersectionMinPoint.y = vec3MinSecond.y;
		vec3IntersectionMaxPoint.y = vec3MaxFirst.y;
	}

	else
		return { false };

	SCollisionData coldtInfo;
	coldtInfo.m_bIsThereCollision = true;


	//getting the middle between min and max
	//using double cast as it vs would give me warning about arithmetic overflow
	//but casting might be slower and the chances of it going above the 4byte limits are slim
	coldtInfo.m_vec3CollisionCenter = vec3(
	((double)vec3IntersectionMinPoint.x + (double)vec3IntersectionMaxPoint.x) * 0.5,
		((double)vec3IntersectionMinPoint.y + (double)vec3IntersectionMaxPoint.y) * 0.5,
		((double)vec3IntersectionMinPoint.z + (double)vec3IntersectionMaxPoint.z) * 0.5);

	//just get one of the cross lines
	//this basically could be done by calculating a collision normal, procject it on both walls of the penetration box and then get the length between those two projected points
	//but that would be computationaly demanding
	coldtInfo.m_fCollisionDepth = glm::length((vec3IntersectionMaxPoint - vec3IntersectionMinPoint)) * 0.2f;

	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox, COrientatedBox& robcosOrientatedBox)
{
	return this->GetCollisionData(robcosOrientatedBox, raacosAxisAlignedBox);
}

SCollisionData CCollisionCalculator::GetCollisionData(COrientatedBox& robcosOrientatedBox, CAxisAlignedBoxCollider& raacosAxisAlignedBox)
{
	vec3 vec3TempOrientation = { 0,0,0 };
	COrientatedBox obTemp = COrientatedBox(raacosAxisAlignedBox.m_pvec3ObjectPosition, &vec3TempOrientation, raacosAxisAlignedBox.m_vec3Extents);
	return this->GetCollisionData(robcosOrientatedBox, obTemp);
}

SCollisionData CCollisionCalculator::GetCollisionData(COrientatedBox& robcosOrientatedBox, CSphereCollider& rscosSphereColider)
{
	return this->GetCollisionData(rscosSphereColider, robcosOrientatedBox);
}

SCollisionData CCollisionCalculator::GetCollisionData(CSphereCollider& rscosSphereColider, COrientatedBox& robcosOrientatedBox)
{
	vec3 vec3Internal;
	robcosOrientatedBox.GetClosestPoint(*rscosSphereColider.m_pvec3ObjectPosition, vec3Internal);
	
	vec3 vec3ToNearestPoint = vec3Internal - *rscosSphereColider.m_pvec3ObjectPosition;

	if (glm::dot(vec3ToNearestPoint, vec3ToNearestPoint) > rscosSphereColider.m_fRadius * rscosSphereColider.m_fRadius)
		return {false};

	SCollisionData coldtInfo;
	coldtInfo.m_bIsThereCollision = true;
	coldtInfo.m_fCollisionDepth = rscosSphereColider.m_fRadius - glm::length(vec3ToNearestPoint);

	coldtInfo.m_vec3CollisionCenter = glm::normalize(*robcosOrientatedBox.m_pvec3ObjectPosition - *rscosSphereColider.m_pvec3ObjectPosition) * coldtInfo.m_fCollisionDepth + *rscosSphereColider.m_pvec3ObjectPosition;

	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(COrientatedBox& robcosOrientatedBox1, COrientatedBox& robcosOrientatedBox2)
{
	//bind object rotation to this
	mat4 mat4Rotations1 = glm::yawPitchRoll(robcosOrientatedBox1.m_pvec3Orientation->y, robcosOrientatedBox1.m_pvec3Orientation->x, robcosOrientatedBox1.m_pvec3Orientation->z);
	mat4 mat4Rotations2 = glm::yawPitchRoll(robcosOrientatedBox2.m_pvec3Orientation->y, robcosOrientatedBox2.m_pvec3Orientation->x, robcosOrientatedBox2.m_pvec3Orientation->z);

	//unit vectors

	vec4 ax = vec4(1, 0, 0, 0) * mat4Rotations1;
	vec4 bx = vec4(1, 0, 0, 0) * mat4Rotations2;

	vec4 ay = vec4(0, 1, 0, 0) * mat4Rotations1;
	vec4 by = vec4(0, 1, 0, 0) * mat4Rotations2;

	vec4 az = vec4(0, 0, 1, 0) * mat4Rotations1;
	vec4 bz = vec4(0, 0, 1, 0) * mat4Rotations2;

	vec4 t = vec4(*robcosOrientatedBox2.m_pvec3ObjectPosition - *robcosOrientatedBox1.m_pvec3ObjectPosition, 0);

	vec3* pvec3Extents1 = &robcosOrientatedBox1.m_vec3Extents;
	vec3* pvec3Extents2 = &robcosOrientatedBox2.m_vec3Extents;
	float penDepth = FLT_MAX;

	bool bIsTherePlane = false;

	ObbAxisCalculation(glm::abs(glm::dot(t, ax)), pvec3Extents1->x + glm::abs(pvec3Extents2->x * glm::dot(ax, bx)) + glm::abs(pvec3Extents2->y * glm::dot(ax, by)) + glm::abs(pvec3Extents2->z * glm::dot(ax, bz)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, ay)), pvec3Extents1->y + glm::abs(pvec3Extents2->x * glm::dot(ay, bx)) + glm::abs(pvec3Extents2->y * glm::dot(ay, by)) + glm::abs(pvec3Extents2->z * glm::dot(ay, bz)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, az)), pvec3Extents1->z + glm::abs(pvec3Extents2->x * glm::dot(az, bx)) + glm::abs(pvec3Extents2->y * glm::dot(az, by)) + glm::abs(pvec3Extents2->z * glm::dot(az, bz)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, bx)), glm::abs(pvec3Extents1->x * glm::dot(ax, bx)) + glm::abs(pvec3Extents1->y * glm::dot(ay, bx)) + glm::abs(pvec3Extents1->z * glm::dot(az, bx)) + pvec3Extents2->x, bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, by)), glm::abs(pvec3Extents1->x * glm::dot(ax, by)) + glm::abs(pvec3Extents1->y * glm::dot(ay, by)) + glm::abs(pvec3Extents1->z * glm::dot(az, by)) + pvec3Extents2->y, bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	//6
	ObbAxisCalculation(glm::abs(glm::dot(t, bz)), glm::abs(pvec3Extents1->x * glm::dot(ax, bz)) + glm::abs(pvec3Extents1->y * glm::dot(ay, bz)) + glm::abs(pvec3Extents1->z * glm::dot(az, bz)) + pvec3Extents2->z, bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, az) * glm::dot(ay, bx) - glm::dot(t, ay) * glm::dot(az, bx)), glm::abs(pvec3Extents1->y * glm::dot(az, bx)) + glm::abs(pvec3Extents1->z * glm::dot(ay, bx)) + glm::abs(pvec3Extents2->y * glm::dot(ax, bz)) + glm::abs(pvec3Extents2->z * glm::dot(ax, by)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, az) * glm::dot(ay, by) - glm::dot(t, ay) * glm::dot(az, by)), glm::abs(pvec3Extents1->y * glm::dot(az, by)) + glm::abs(pvec3Extents1->z * glm::dot(ay, by)) + glm::abs(pvec3Extents2->x * glm::dot(ax, bz)) + glm::abs(pvec3Extents2->z * glm::dot(ax, bx)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, az) * glm::dot(ay, bz) - glm::dot(t, ay) * glm::dot(az, bz)), glm::abs(pvec3Extents1->y * glm::dot(az, bz)) + glm::abs(pvec3Extents1->z * glm::dot(ay, bz)) + glm::abs(pvec3Extents2->x * glm::dot(ax, by)) + glm::abs(pvec3Extents2->y * glm::dot(ax, bx)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, ax) * glm::dot(az, bx) - glm::dot(t, az) * glm::dot(ax, bx)), glm::abs(pvec3Extents1->x * glm::dot(az, bx)) + glm::abs(pvec3Extents1->z * glm::dot(ax, bx)) + glm::abs(pvec3Extents2->y * glm::dot(ay, bz)) + glm::abs(pvec3Extents2->z * glm::dot(ay, by)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, ax) * glm::dot(az, by) - glm::dot(t, az) * glm::dot(ax, by)), glm::abs(pvec3Extents1->x * glm::dot(az, by)) + glm::abs(pvec3Extents1->z * glm::dot(ax, by)) + glm::abs(pvec3Extents2->x * glm::dot(ay, bz)) + glm::abs(pvec3Extents2->z * glm::dot(ay, bx)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	//12
	ObbAxisCalculation(glm::abs(glm::dot(t, ax) * glm::dot(az, bz) - glm::dot(t, az) * glm::dot(ax, bz)), glm::abs(pvec3Extents1->x * glm::dot(az, bz)) + glm::abs(pvec3Extents1->z * glm::dot(ax, bz)) + glm::abs(pvec3Extents2->x * glm::dot(ay, by)) + glm::abs(pvec3Extents2->y * glm::dot(ay, bx)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, ay) * glm::dot(ax, bx) - glm::dot(t, ax) * glm::dot(ay, bx)), glm::abs(pvec3Extents1->x * glm::dot(ay, bx)) + glm::abs(pvec3Extents1->y * glm::dot(ax, bx)) + glm::abs(pvec3Extents2->y * glm::dot(az, bz)) + glm::abs(pvec3Extents2->z * glm::dot(az, by)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, ay) * glm::dot(ax, by) - glm::dot(t, ax) * glm::dot(ay, by)), glm::abs(pvec3Extents1->x * glm::dot(ay, by)) + glm::abs(pvec3Extents1->y * glm::dot(ax, by)) + glm::abs(pvec3Extents2->x * glm::dot(az, bz)) + glm::abs(pvec3Extents2->z * glm::dot(az, bx)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	ObbAxisCalculation(glm::abs(glm::dot(t, ay) * glm::dot(ax, bz) - glm::dot(t, ax) * glm::dot(ay, bz)), glm::abs(pvec3Extents1->x * glm::dot(ay, bz)) + glm::abs(pvec3Extents1->y * glm::dot(ax, bz)) + glm::abs(pvec3Extents2->x * glm::dot(az, by)) + glm::abs(pvec3Extents2->y * glm::dot(az, bx)), bIsTherePlane, penDepth);
	if (bIsTherePlane)
		return { false };

	SCollisionData coldtInfo;
	coldtInfo.m_bIsThereCollision = true;
	coldtInfo.m_fCollisionDepth = penDepth;

	vec3 colNormal = *robcosOrientatedBox2.m_pvec3ObjectPosition - *robcosOrientatedBox1.m_pvec3ObjectPosition;

	coldtInfo.m_vec3CollisionCenter = *robcosOrientatedBox1.m_pvec3ObjectPosition + colNormal * penDepth;
	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(CPlaneCollider& rpcosPlane, COrientatedBox& robcosOrientatedBox)
{
	//do similar to sphere but check with every vertex

	float fProjectedPointT = rpcosPlane.m_vec3Normal.x * (rpcosPlane.m_pvec3ObjectPosition->x - robcosOrientatedBox.m_pvec3ObjectPosition->x) + rpcosPlane.m_vec3Normal.y * (rpcosPlane.m_pvec3ObjectPosition->y - robcosOrientatedBox.m_pvec3ObjectPosition->y) + rpcosPlane.m_vec3Normal.z * (rpcosPlane.m_pvec3ObjectPosition->z - robcosOrientatedBox.m_pvec3ObjectPosition->z) / (rpcosPlane.m_vec3Normal.x * rpcosPlane.m_vec3Normal.x + rpcosPlane.m_vec3Normal.y * rpcosPlane.m_vec3Normal.y + rpcosPlane.m_vec3Normal.z * rpcosPlane.m_vec3Normal.z);
	vec3 vec3ProjectionPoint = { robcosOrientatedBox.m_pvec3ObjectPosition->x + fProjectedPointT * rpcosPlane.m_vec3Normal.x, robcosOrientatedBox.m_pvec3ObjectPosition->y + fProjectedPointT * rpcosPlane.m_vec3Normal.y, robcosOrientatedBox.m_pvec3ObjectPosition->z + fProjectedPointT * rpcosPlane.m_vec3Normal.z };

	float fCenterAndProjection = glm::length(vec3ProjectionPoint - *robcosOrientatedBox.m_pvec3ObjectPosition);

	vec3 vec3Internal;
	robcosOrientatedBox.GetClosestPoint(vec3ProjectionPoint, vec3Internal);
	
	float fCenterAndInternal = glm::length(*robcosOrientatedBox.m_pvec3ObjectPosition - vec3Internal);

	if (fCenterAndProjection > fCenterAndInternal)
		return { false };

	SCollisionData coldtInfo;
	coldtInfo.m_bIsThereCollision = true;

	coldtInfo.m_vec3CollisionCenter = (vec3ProjectionPoint - vec3Internal) * 0.5f + vec3Internal;
	coldtInfo.m_fCollisionDepth = glm::length( coldtInfo.m_vec3CollisionCenter - *robcosOrientatedBox.m_pvec3ObjectPosition);

	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(COrientatedBox& robcosOrientatedBox, CPlaneCollider& rpcosPlane)
{
	return this->GetCollisionData(rpcosPlane, robcosOrientatedBox);
}

SCollisionData CCollisionCalculator::GetCollisionData(CPlaneCollider& rpcosPlane, CAxisAlignedBoxCollider& raacosAxisAlignedBox)
{
	vec3 vec3TempOrientation = { 0,0,0 };
	COrientatedBox obTemp = COrientatedBox(raacosAxisAlignedBox.m_pvec3ObjectPosition, &vec3TempOrientation, raacosAxisAlignedBox.m_vec3Extents);

	return this->GetCollisionData(rpcosPlane, obTemp);
}

SCollisionData CCollisionCalculator::GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox, CPlaneCollider& rpcosPlane)
{
	return this->GetCollisionData(rpcosPlane, raacosAxisAlignedBox);
}

SCollisionData CCollisionCalculator::GetCollisionData(CPlaneCollider& rpcosPlane, CSphereCollider& rscolSphereColider)
{

	vec3 vec3FromPlaneToObject = *rscolSphereColider.m_pvec3ObjectPosition - *rpcosPlane.m_pvec3ObjectPosition;

	float fDistance = glm::dot(rpcosPlane.m_vec3Normal, vec3FromPlaneToObject) / glm::length(rpcosPlane.m_vec3Normal);

	float fDistanceFromFurtherstPoint = fDistance - rscolSphereColider.m_fRadius;

	if (fDistanceFromFurtherstPoint > 0)
		return { false };

	SCollisionData coldtInfo;

	coldtInfo.m_bIsThereCollision = true;

	if (fDistance <= 0) {
		coldtInfo.m_fCollisionDepth = abs(fDistanceFromFurtherstPoint);
	}

	else {
		coldtInfo.m_fCollisionDepth = abs(fDistanceFromFurtherstPoint - fDistance);
	}

	//add fDistance + half of the depth and add all that to the center point of the sphere towards the plane to get the center point of the collision

	vec3 vec3PlaneNormals = rpcosPlane.m_vec3Normal;

	//https://math.stackexchange.com/questions/100761/how-do-i-find-the-projection-of-a-point-onto-a-plane
	float fProjectedPointT = vec3PlaneNormals.x * (rpcosPlane.m_pvec3ObjectPosition->x - rscolSphereColider.m_pvec3ObjectPosition->x) + vec3PlaneNormals.y * (rpcosPlane.m_pvec3ObjectPosition->y - rscolSphereColider.m_pvec3ObjectPosition->y) + vec3PlaneNormals.z * (rpcosPlane.m_pvec3ObjectPosition->z - rscolSphereColider.m_pvec3ObjectPosition->z) / (vec3PlaneNormals.x * vec3PlaneNormals.x + vec3PlaneNormals.y * vec3PlaneNormals.y + vec3PlaneNormals.z * vec3PlaneNormals.z);
	fProjectedPointT *= -1;
	vec3 vec3ProjectionPoint = { rscolSphereColider.m_pvec3ObjectPosition->x + fProjectedPointT * vec3PlaneNormals.x, rscolSphereColider.m_pvec3ObjectPosition->y + fProjectedPointT * vec3PlaneNormals.y, rscolSphereColider.m_pvec3ObjectPosition->z + fProjectedPointT * vec3PlaneNormals.z };

	//get vector between sphere pos and projection point, normalize it and multiply by the depth then add it the point
	coldtInfo.m_vec3CollisionCenter = vec3ProjectionPoint - glm::normalize(vec3ProjectionPoint - *rscolSphereColider.m_pvec3ObjectPosition) * coldtInfo.m_fCollisionDepth;
	return coldtInfo;
}

SCollisionData CCollisionCalculator::GetCollisionData(CSphereCollider& rscolSphereColider, CPlaneCollider& rpcosPlane)
{
	return this->GetCollisionData(rpcosPlane, rscolSphereColider);
}

#pragma endregion

void CCollisionCalculator::PrintOutBins()
{
	for (tuple<int, int> tplIndexes : this->m_setiPopulatedCellIndexes)
	{
		int iX, iZ;
		iX = std::get<0>(tplIndexes);
		iZ = std::get<1>(tplIndexes);

		std::cout << "\nBIN (" << iX << ";" << iZ << ") Contains:   ";
		vector<tuple< CGameObject*, CGameObject*>> vtpgoBin = this->m_vtpgoCollisionGrid[iX][iZ];

		for (auto pgoBinContents : vtpgoBin)
		{
			CGameObject* pgoReadable = std::get<0>(pgoBinContents);
			CGameObject* pgoWritable = std::get<1>(pgoBinContents);
			std::cout << pgoReadable->GetName() << " ( Read: " << pgoReadable << " , Write: " << pgoWritable <<" )  |  ";
		}
	}
}