#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Nicholas Bazos - ndb9897@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Fill scene vector with MyMesh objects
	for (int i = 0; i < 48; i++)
	{
		MyMesh* m = new MyMesh();
		scene.push_back(m);
	}

	// Generate cubes for all the objects
	for (int i = 0; i < scene.size(); i++)
	{
		scene[i]->GenerateCube(1.0f, C_WHITE);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();


	//Is the arcball active?
	ArcBall();


	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// View, Projection, and Scale
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f, 2.0f, 2.0f));
	static float value = 0.0f;

	// starting cube (from the left side) translations for each row (running top to bottom)
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	matrix4 m4TranslateTwo = glm::translate(IDENTITY_M4, vector3(1.0f + value, 1.0f, 3.0f));
	matrix4 m4TranslateThree = glm::translate(IDENTITY_M4, vector3(value, 0.0f, 3.0f));
	matrix4 m4TranslateFour = glm::translate(IDENTITY_M4, vector3(-1.0f + value, -1.0f, 3.0f));
	matrix4 m4TranslateFive = glm::translate(IDENTITY_M4, vector3(-2.0f + value, -2.0f, 3.0f));
	matrix4 m4TranslateSix = glm::translate(IDENTITY_M4, vector3(-2.0f + value, -3.0f, 3.0f));
	matrix4 m4TranslateSeven = glm::translate(IDENTITY_M4, vector3(-2.0f + value, -4.0f, 3.0f));
	matrix4 m4TranslateEight = glm::translate(IDENTITY_M4, vector3(1.0f + value, -5.0f, 3.0f));

	// movement across screen
	value += 0.01f;

	//matrix4 m4Model = m4Translate * m4Scale;
	//matrix4 m4Model = m4Scale * m4Translate;

	// Render the scene collection 
	for (int i = 0; i < scene.size(); i++)
	{
		// This for loop runs through all the generated cubes within the scene collection and splits them among 
		// rows using conditionals and the corresponding cube indeces. For example, the first row is the top of picture and has two blocks, 
		// therefore, the last index of the first row is the second block of index 1. 
		// Above this for loop, all the starting translations for the first cube in each row starting from the left are made. 
		// Those translations are then modified after the first cube of that row is rendered. 
		// When the modification of the translation includes the addition of a float it signifies how many spaces it exists from x=0.
		// When the modification of the translation includes the (i-#) it indicates the calculation of spaces needed due to the # subtracted from the index. 
		// All nested if statements occur when a row has two or more space between the first block and the last block in the row. 

		// First row
		if (i < 2)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4Translate);
			m4Translate = glm::translate(IDENTITY_M4, vector3(6.0f + value, 2.0f, 3.0f));
		}

		// Second row
		else if (i >= 2 && i < 4)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateTwo);
			m4TranslateTwo = glm::translate(IDENTITY_M4, vector3(5.0f + value, 1.0f, 3.0f));
		}

		// Third row
		else if (i >= 4 && i < 11)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateThree);
			m4TranslateThree = glm::translate(IDENTITY_M4, vector3((i-3) + value, 0.0f, 3.0f));
		}

		// Fourth row
		else if (i >= 11 && i < 19)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateFour);

			if (i == 12)
			{
				m4TranslateFour = glm::translate(IDENTITY_M4, vector3(value, -1.0f, 3.0f));
			}
			else if (i >= 13 && i < 16)
			{
				m4TranslateFour = glm::translate(IDENTITY_M4, vector3((i-11) + value, -1.0f, 3.0f));
			}
			else if(i >= 16 && i < 19)
			{
				m4TranslateFour = glm::translate(IDENTITY_M4, vector3((i-10) + value, -1.0f, 3.0f));
			} 
		}

		// Fifth row
		else if (i >=19 && i < 30)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateFive);
			m4TranslateFive = glm::translate(IDENTITY_M4, vector3((i-20) + value, -2.0f, 3.0f));
		}

		// Sxith row
		else if (i >= 30 && i < 40)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateSix);

			if (i > 30 && i < 38)
			{
				m4TranslateSix = glm::translate(IDENTITY_M4, vector3((i-31) + value, -3.0f, 3.0f));
			}

			if (i == 38)
			{
				m4TranslateSix = glm::translate(IDENTITY_M4, vector3(8.0f + value, -3.0f, 3.0f));
			}
		}

		// Seventh Row
		else if (i >= 40 && i < 44)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateSeven);

			if (i == 40)
			{
				m4TranslateSeven = glm::translate(IDENTITY_M4, vector3(value, -4.0f, 3.0f));
			}
			else if (i == 41)
			{
				m4TranslateSeven = glm::translate(IDENTITY_M4, vector3(6.0f + value, -4.0f, 3.0f));
			}
			else if (i == 42)
			{
				m4TranslateSeven = glm::translate(IDENTITY_M4, vector3(8.0f + value, -4.0f, 3.0f));
			}
		}

		// Eight row
		else if (i >= 44 && i < 48)
		{
			scene[i]->Render(m4Projection, m4View, m4Scale * m4TranslateEight);

			if (i == 44)
			{
				m4TranslateEight = glm::translate(IDENTITY_M4, vector3(2.0f + value, -5.0f, 3.0f));
			}

			if (i >= 45)
			{
				m4TranslateEight = glm::translate(IDENTITY_M4, vector3((i - 41) + value, -5.0f, 3.0f));
			}
		}
	}

	
	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f )));
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	// loops through scene vector to deallocate each element
	for (int i = 0; i < scene.size(); i++)
	{
		if (scene[i] != nullptr)
		{
			delete scene[i];
			scene[i] = nullptr;
		}
		SafeDelete(scene[i]);
	}
	//release GUI
	ShutdownGUI();
}