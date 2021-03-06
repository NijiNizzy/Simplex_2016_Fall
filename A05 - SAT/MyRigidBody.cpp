#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	// Important Notes from the Real Time Collision Detection: 
	// a separating axis is formed by taking the cross product of an edge from each bounding box 
	// -> fix for parallel edges: add a small epsilon value to the absolute values of the matrix elements occuring on the right-hand side of the inequalities 
	// **THE AXES TO TEST ARE THE NORMALS OF EACH SHAPE'S EDGES** 
	// 3 axes from object A (face normals), 3 axes from object B (face normals), 9 axes from the pairs of edges
	// of A and edges of B (cross products of the edges of A and edges of B) = 15 axes

	// radii for Object A and Object B
	float radiusA;
	float radiusB;

	// rotation matrix and absolute rotation matrix
	matrix3 rotation;
	matrix3 absoluteRotation;

	// arrays of model coords for Object A and Object B
	vector3 uA[3];
	vector3 uB[3];

	// calculating the model coords
	uA[0] = vector3(this->m_m4ToWorld * vector4(AXIS_X, 0));
	uA[1] = vector3(this->m_m4ToWorld * vector4(AXIS_Y, 0));
	uA[2] = vector3(this->m_m4ToWorld * vector4(AXIS_Z, 0));

	uB[0] = vector3(a_pOther->m_m4ToWorld * vector4(AXIS_X, 0));
	uB[1] = vector3(a_pOther->m_m4ToWorld * vector4(AXIS_Y, 0));
	uB[2] = vector3(a_pOther->m_m4ToWorld * vector4(AXIS_Z, 0));

	// rotation matrix to put object B in A's coordinate frame
	// looping through the three axes
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			rotation[i][j] = glm::dot(uA[i], uB[j]);
		}
	}

	// vector containing the distance from object B's center to object A's center
	vector3 centerToCenter = a_pOther->GetCenterGlobal() - this->GetCenterGlobal();

	// put centerToCenter vector into A's coordinate frame
	centerToCenter = vector3(glm::dot(centerToCenter, uA[0]), glm::dot(centerToCenter, uA[1]), glm::dot(centerToCenter, uA[2]));

	// compute subexpressions and take into account arithmetic errors when two edges are parallel by adding epsilon
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			absoluteRotation[i][j] = abs(rotation[i][j]) + FLT_EPSILON;
		}
	}

	// test A's x-axis (A0) & test A's y-axis (A1) & test A's z-axis (A2) 
	for (int i = 0; i < 3; i++)
	{
		radiusA = this->m_v3HalfWidth[i];
		radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[i][0] + a_pOther->m_v3HalfWidth[1] * absoluteRotation[i][1] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[i][2];
		if (abs(centerToCenter[i]) > (radiusA + radiusB))
		{
			return 1;
		}
	}

	// test B's x-axis (B0) & test B's y-axis (B1) & test B's z-axis (B2)
	for (int i = 0; i < 3; i++)
	{
		radiusA = this->m_v3HalfWidth[0] * absoluteRotation[0][i] + this->m_v3HalfWidth[1] * absoluteRotation[1][i] + this->m_v3HalfWidth[2] * absoluteRotation[2][i];
		radiusB = a_pOther->m_v3HalfWidth[i];
		if (abs(centerToCenter[0] * rotation[0][i] + centerToCenter[1] * rotation[1][i] + centerToCenter[2] * rotation[2][i]) > (radiusA + radiusB))
		{
			return 1;
		}
	}

	// test cross(A0, B0)
	radiusA = this->m_v3HalfWidth[1] * absoluteRotation[2][0] + this->m_v3HalfWidth[2] * absoluteRotation[1][0];
	radiusB = a_pOther->m_v3HalfWidth[1] * absoluteRotation[0][2] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[0][1];
	if (abs(centerToCenter[2] * rotation[1][0] - centerToCenter[1] * rotation[2][0]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A0, B1)
	radiusA = this->m_v3HalfWidth[1] * absoluteRotation[2][1] + this->m_v3HalfWidth[2] * absoluteRotation[1][1];
	radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[0][2] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[0][0];
	if (abs(centerToCenter[2] * rotation[1][1] - centerToCenter[1] * rotation[2][1]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A0, B2)
	radiusA = this->m_v3HalfWidth[1] * absoluteRotation[2][2] + this->m_v3HalfWidth[2] * absoluteRotation[1][2];
	radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[0][1] + a_pOther->m_v3HalfWidth[1] * absoluteRotation[0][0];
	if (abs(centerToCenter[2] * rotation[1][2] - centerToCenter[1] * rotation[2][2]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A1, B0)
	radiusA = this->m_v3HalfWidth[0] * absoluteRotation[2][0] + this->m_v3HalfWidth[2] * absoluteRotation[0][0];
	radiusB = a_pOther->m_v3HalfWidth[1] * absoluteRotation[1][2] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[1][1];
	if (abs(centerToCenter[0] * rotation[2][0] - centerToCenter[2] * rotation[0][0]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A1, B1)
	radiusA = this->m_v3HalfWidth[0] * absoluteRotation[2][1] + this->m_v3HalfWidth[2] * absoluteRotation[0][1];
	radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[1][2] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[1][0];
	if (abs(centerToCenter[0] * rotation[2][1] - centerToCenter[2] * rotation[0][1]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A1, B2)
	radiusA = this->m_v3HalfWidth[0] * absoluteRotation[2][2] + this->m_v3HalfWidth[2] * absoluteRotation[0][2];
	radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[1][1] + a_pOther->m_v3HalfWidth[1] * absoluteRotation[1][0];
	if (abs(centerToCenter[0] * rotation[2][2] - centerToCenter[2] * rotation[0][2]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A2, B0)
	radiusA = this->m_v3HalfWidth[0] * absoluteRotation[1][0] + this->m_v3HalfWidth[1] * absoluteRotation[0][0];
	radiusB = a_pOther->m_v3HalfWidth[1] * absoluteRotation[2][2] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[2][1];
	if (abs(centerToCenter[1] * rotation[0][0] - centerToCenter[0] * rotation[1][0]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A2, B1)
	radiusA = this->m_v3HalfWidth[0] * absoluteRotation[1][1] + this->m_v3HalfWidth[1] * absoluteRotation[0][1];
	radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[2][2] + a_pOther->m_v3HalfWidth[2] * absoluteRotation[2][0];
	if (abs(centerToCenter[1] * rotation[0][1] - centerToCenter[0] * rotation[1][1]) > (radiusA + radiusB))
	{
		return 1;
	}

	// test cross(A2, B2)
	radiusA = this->m_v3HalfWidth[0] * absoluteRotation[1][2] + this->m_v3HalfWidth[1] * absoluteRotation[0][2];
	radiusB = a_pOther->m_v3HalfWidth[0] * absoluteRotation[2][1] + a_pOther->m_v3HalfWidth[1] * absoluteRotation[2][0];
	if (abs(centerToCenter[1] * rotation[0][2] - centerToCenter[0] * rotation[1][2]) > (radiusA + radiusB))
	{
		return 1;
	}

	// there is no axis that separates the two objects
	return 0;
}