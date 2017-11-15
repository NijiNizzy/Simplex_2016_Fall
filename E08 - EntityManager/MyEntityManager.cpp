#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	// set the entity count to zero 
	m_uEntityCount = 0;
	// clear the entity list
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	// loop through the entities list and safe delete 
	for (uint uEntity = 0; uEntity < m_uEntityCount; uEntity++)
	{
		MyEntity* pEntity = m_entityList[uEntity];
		SafeDelete(pEntity);
	}

	// set the entity count to zero 
	m_uEntityCount = 0;
	// clear the entity list
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	// check if nullptr
	if (m_pInstance == nullptr)
	{
		// if so store new entity manager in m_pInstance singleton
		m_pInstance = new MyEntityManager();
	}

	// if not return the instance
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	// if the instance is not a nullptr
	if (m_pInstance != nullptr)
	{
		// delete the pointer and store nullptr
		delete m_pInstance;
		m_pInstance = nullptr; 
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// loop through the entity list and check if each entry's
	// unique ID is equal to the one passed in
	for (uint i = 0; i < m_uEntityCount; i++)
	{
		// if the index in the entity list is equal to the unique ID passed in 
		if (a_sUniqueID == m_entityList[i]->GetUniqueID())
		{
			// return the index of that entity 
			return i;
		}
	}

	// otherwise reutrn -1
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// if there are no entries in the list return the nullptr
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_uEntityCount)
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_uEntityCount - 1;
	}

	// return the model of the entity at the specific index in the list
	return m_entityList[a_uIndex]->GetModel();
	
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	// retrieve the entity and store it a temporary MyEntity pointer
	MyEntity* temp = MyEntity::GetEntity(a_sUniqueID);
	
	// if the entity exists
	if (temp)
	{
		// return the model of the entity
		return temp->GetModel();
	}

	// otherwise return the nullptr
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// if there are no entries in the list return the nullptr
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}
		
	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_uEntityCount)
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_uEntityCount - 1;
	}
		
	// return the rigidbody of the entity at the specific index in the list
	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	// retrieve the entity and store it a temporary MyEntity pointer
	MyEntity* temp = MyEntity::GetEntity(a_sUniqueID);
	
	// if the entity exists
	if (temp)
	{
		// return the rigidbody of the entity
		return temp->GetRigidBody();
	}

	// otherwise return the nullptr
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// if there are no entries in the list return the identity matrix
	if (m_entityList.size() == 0)
	{
		return IDENTITY_M4;
	}
		
	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_uEntityCount)
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_uEntityCount - 1;
	}
		
	// return the model matrix of the entity at the specific index in the list
	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	// retrieve the entity and store it a temporary MyEntity pointer
	MyEntity* temp = MyEntity::GetEntity(a_sUniqueID);

	//if the entity exists
	if (temp)
	{
		// return the model matrix of the entity
		return temp->GetModelMatrix();
	}

	// otherwise return the identity matrix
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	// retrieve the entity and store it a temporary MyEntity pointer
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);

	//if the entity exists
	if (pTemp)
	{
		// set the model matrix to the passed in matrix
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// if there are no entries in the list return nothing
	if (m_entityList.size() == 0)
	{
		return;
	}
	
	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_uEntityCount)
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_uEntityCount - 1;
	}
	
	// set the model matrix of the entity at the specific index in the list
	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	// loop through the entity list
	for (uint i = 0; i < m_uEntityCount - 1; i++)
	{
		// nested loop to go through entity list starting with entity at the index one ahead of original looop
		for (uint j = i + 1; j < m_uEntityCount; j++)
		{
			// check if entity at index i is colliding with all the other entities
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	// create a temporary MyEntity pointer to store the object
	MyEntity* temp = new MyEntity(a_sFileName, a_sUniqueID);

	//if it is initialized
	if (temp->IsInitialized())
	{
		// add it to the entity list
		m_entityList.push_back(temp);
		// update entity count
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	// if there are no entries in the list return nothing
	if (m_entityList.size() == 0)
	{
		return;
	}

	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_uEntityCount)
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_uEntityCount - 1;
	}

	// if the entity is not last entity in the list
	if (a_uIndex != m_uEntityCount - 1)
	{
		// swap it to move it to the end of the list
		std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	}

	// then remove it form the list and 
	MyEntity* temp = m_entityList[m_uEntityCount - 1];
	SafeDelete(temp);
	m_entityList.pop_back();

	// subtract one from the entity count and return
	m_uEntityCount -= 1;
	return;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	// get the index of the entity using the unique ID
	int nIndex = GetEntityIndex(a_sUniqueID);

	// remove it
	RemoveEntity((uint)nIndex);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	// if there are no entries in the list return nothing
	if (m_entityList.size() == 0)
	{
		return "";
	}
	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_entityList.size())
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_entityList.size() - 1;
	}

	// return the the unique ID of the entity at that specific index
	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	// if there are no entries in the list return nothing
	if (m_entityList.size() == 0)
	{
		return nullptr;
	}

	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_entityList.size())
	{
		// store one less than the number of entries in the list in the index
		a_uIndex = m_entityList.size() - 1;
	}

	// return the entity at that index
	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// if the index passed in is greater than the amount of entries in the list
	if (a_uIndex >= m_uEntityCount)
	{
		// for each entity in the list add it to the render list
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; a_uIndex++)
		{
			m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	//otherwise do it for the single entity
	else 
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	// store the entity in a temporary MyEntity pointer
	MyEntity* temp = MyEntity::GetEntity(a_sUniqueID);

	// if the entity exists
	if (temp)
	{
		// add it to the render list
		temp->AddToRenderList(a_bRigidBody);
	}
}