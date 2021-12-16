#include "GameObject.h"
#include <vector>
#include "Collider.h"
//https://medium.com/@gx578007/searching-vector-set-and-unordered-set-6649d1aa7752
//but the uset requires its items to be hashed and tuple doesn't have that as far as I understand
#include <set>
#include <tuple>
#include <glm/gtx/euler_angles.hpp>
#include "RotattingBody.h"

using std::vector;
using std::set;
using std::tuple;
using glm::vec4;


#pragma once
class CCollisionCalculator
{
public:
	void RecalculateCollision(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadable);
	vec3 m_vec3UpperLeftPoint;
	void PrintOutBins();

private:
	static const unsigned int sm_kuiGridSize = 10;
	static const float sm_kfLengthOfCell;

	static float sm_InvertedLengthOfCell;

	set<tuple<int, int>> m_setiPopulatedCellIndexes;

	vector<tuple<CGameObject*, CGameObject*>> m_vtpgoCollisionGrid [sm_kuiGridSize][sm_kuiGridSize];

#pragma region Hash codes for collider classes
	//it is cheaper to compare numbers than strings

	static const size_t sm_kstSphereCollider = 2035594727;
	static const size_t sm_kstAxisAllignedBox = 1552493518;
	static const size_t sm_kstOrientatedBox = 2912439608;
	static const size_t sm_kstPlaneCollider = 862373276;
#pragma endregion

	//Partitions everything into spatial grid fro broad phase
	void Partition(vector<CGameObject*>& rvgoWritable, vector<CGameObject*>& rvgoReadablee);

	void CalculatePairs(); //narrow pahase

	void CallCorrectCollisionCallculation(CCollider& colBaseColider1, CCollider& colBaseCollider2, SCollisionData& rcoldtInfo);

	//Switch based helper method for finding the correct collision calculation
	void SwitchThrough(CCollider& colBaseColider1, CCollider& colBaseCollider2, SCollisionData& rcoldtInfo);

	//If statement based helper method for finding the correct collision calculation
	void SweepThrough(CCollider& colBaseColider1, CCollider& colBaseCollider2, SCollisionData& rcoldtInfo);

	void CollisionResolution(CGameObject* pgoWritable1, CGameObject* pgoReadable1, CGameObject* pgoWritable2, CGameObject *pgoReadable2, SCollisionData& rcoldtInfo);

	//A helper method for obb x obb calculation
	void ObbAxisCalculation(float rfAxis, float rfSumOfPoints, bool& rbIsTherePlaneInBetween, float& rfPenetrationDepth);

#pragma region All the unique collision functions
	SCollisionData GetCollisionData(CSphereCollider& rscolShereColider1, CSphereCollider& rscolSphereColider2);

	SCollisionData GetCollisionData(CSphereCollider& rscosSphereColider, CAxisAlignedBoxCollider & raacosAxisAlignedBox);
	SCollisionData GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox, CSphereCollider& rscosSphereColider);

	SCollisionData GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox1, CAxisAlignedBoxCollider& raacosAxisAlignedBox2);

	SCollisionData GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox, COrientatedBox & robcosOrientatedBox);
	SCollisionData GetCollisionData(COrientatedBox& robcosOrientatedBox, CAxisAlignedBoxCollider& raacosAxisAlignedBox);

	SCollisionData GetCollisionData(COrientatedBox& robcosOrientatedBox, CSphereCollider& rscosSphereColider);
	SCollisionData GetCollisionData(CSphereCollider& rscosSphereColider, COrientatedBox& robcosOrientatedBox);

	SCollisionData GetCollisionData(COrientatedBox& robcosOrientatedBox1, COrientatedBox& robcosOrientatedBox2);

	SCollisionData GetCollisionData(CPlaneCollider& rpcosPlane, COrientatedBox& robcosOrientatedBox);
	SCollisionData GetCollisionData(COrientatedBox& robcosOrientatedBox, CPlaneCollider& rpcosPlane);

	SCollisionData GetCollisionData(CPlaneCollider& rpcosPlane, CAxisAlignedBoxCollider& raacosAxisAlignedBox);
	SCollisionData GetCollisionData(CAxisAlignedBoxCollider& raacosAxisAlignedBox, CPlaneCollider& rpcosPlane);

	SCollisionData GetCollisionData(CPlaneCollider& rpcosPlane, CSphereCollider& rscolSphereColider);
	SCollisionData GetCollisionData(CSphereCollider& rscolSphereColider, CPlaneCollider& rpcosPlane);


#pragma endregion
};