#include "State.h"
#include "StateManager.h"
#include "SystemManager.h"
#include "ObjectContainer.h"

//TODO Remove later
#include "Sprite.h"
#include "Object.h"
#include "Camera.h"
#include "Transform.h"
#include "InputHandler.h"
#include "ObjectFactory.h"
#include "Matrix4x4.h"
#include "GraphicSystem.h"

NS_JE_BEGIN

State::State(const char* _name)
	:m_pLastStage(nullptr)
{
	m_name.assign(_name);
}

void State::Load()
{
	JE_DEBUG_PRINT("Loading %s...\n", m_name.c_str());
	m_objContainer = new ObjectContainer; 
	SystemManager::Load();
}

void State::Init()
{
	JE_DEBUG_PRINT("Initializing %s...\n", m_name.c_str());
	SystemManager::Init();

	/************************* Test Code... **************************/
	ObjectFactory::CreateObject("camera");
	ObjectFactory::GetCreatedObject()->AddComponent<Camera>();
	ObjectFactory::GetCreatedObject()->GetComponent<Camera>()->m_position = vec3(0 ,0, 80);
	SystemManager::GetGraphicSystem()->SetMainCamera(
		ObjectFactory::GetCreatedObject()->GetComponent<Camera>());
	ObjectFactory::AddCreatedObject(m_objContainer);

	ObjectFactory::CreateObject("test");
	ObjectFactory::GetCreatedObject()->AddComponent<Transform>();
	ObjectFactory::GetCreatedObject()->AddComponent<Sprite>();
	ObjectFactory::AddCreatedObject(m_objContainer);

	m_objContainer->GetObject("test")->GetComponent<Transform>()->m_position.Set(10, 10, 1);
	m_objContainer->GetObject("test")->GetComponent<Transform>()->m_rotation = 0;
	m_objContainer->GetObject("test")->GetComponent<Transform>()->m_scale.Set(30.f, 30.f, 0.f);
	m_objContainer->GetObject("test")->GetComponent<Sprite>()->m_color.Set(1.f, 1.f, 0.f, 1.f);
	m_objContainer->GetObject("test")->GetComponent<Sprite>()->AddTexture("testTexture");
	m_objContainer->GetObject("test")->GetComponent<Sprite>()->m_projection = Sprite::ORTHOGONAL;

	ObjectFactory::CreateObject("test1");
	ObjectFactory::GetCreatedObject()->AddComponent<Transform>();
	ObjectFactory::GetCreatedObject()->AddComponent<Sprite>();
	ObjectFactory::AddCreatedObject(m_objContainer);

	m_objContainer->GetObject("test1")->GetComponent<Transform>()->m_position.Set(10, 10, 2);
	m_objContainer->GetObject("test1")->GetComponent<Transform>()->m_rotation = 0;
	m_objContainer->GetObject("test1")->GetComponent<Transform>()->m_scale.Set(30.f, 30.f, 0.f);
	m_objContainer->GetObject("test1")->GetComponent<Sprite>()->m_color.Set(1.f, 1.f, 0.f, 1.f);
	m_objContainer->GetObject("test1")->GetComponent<Sprite>()->AddTexture("testTexture");
	m_objContainer->GetObject("test1")->GetComponent<Sprite>()->m_projection = Sprite::PERSPECTIVE;

	ObjectFactory::CreateObject("test2");
	ObjectFactory::GetCreatedObject()->AddComponent<Transform>();
	ObjectFactory::GetCreatedObject()->AddComponent<Sprite>();
	ObjectFactory::GetCreatedObject()->GetComponent<Transform>()->m_position.Set(-10, -10, 0);
	ObjectFactory::GetCreatedObject()->GetComponent<Transform>()->m_rotation = 0;
	ObjectFactory::GetCreatedObject()->GetComponent<Transform>()->m_scale.Set(30.f, 30.f, 0.f);
	ObjectFactory::GetCreatedObject()->GetComponent<Sprite>()->m_color.Set(1.f, 1.f, 1.f, 1.f);
	ObjectFactory::GetCreatedObject()->GetComponent<Sprite>()->AddTexture("testAnimation");
	ObjectFactory::GetCreatedObject()->GetComponent<Sprite>()->ActiveAnimation(true);
	ObjectFactory::GetCreatedObject()->GetComponent<Sprite>()->SetAnimationFrame(8);
	ObjectFactory::GetCreatedObject()->GetComponent<Sprite>()->SetAnimationSpeed(10.f);
	ObjectFactory::AddCreatedObject(m_objContainer);

	SystemManager::GetGraphicSystem()->SetBackgroundColor(vec4(1,0,0,1));

	mat4 matrix = mat4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16).Inverse();
	std::cout << matrix << std::endl;
}

void State::Update(float _dt)
{
	//JE_DEBUG_PRINT("Updating %s...\n", m_name.c_str());
	SystemManager::Update(_dt);

	/*************************** Temp state test key ******************************/
	if (InputHandler::KeyTriggered(JE_1))
		StateManager::SetNextState("testState1");

	if (InputHandler::KeyTriggered(JE_2))
		StateManager::SetNextState("testState2");

	if (InputHandler::KeyTriggered(JE_3))
		StateManager::SetNextState("testState3");

	if (InputHandler::KeyTriggered(JE_4))
		StateManager::Pause("PauseState");

	if (InputHandler::KeyTriggered(JE_5))
		StateManager::Resume();

	if (InputHandler::KeyTriggered(JE_6))
		StateManager::ResumeAndNext("testState3");

	if (InputHandler::KeyTriggered(JE_MOUSE_LEFT)) {
		JE_DEBUG_PRINT("Left Mouse\n");
		m_objContainer->GetObject("test")->GetComponent<Transform>()->m_position 
			= InputHandler::m_orthoPosition;
		m_objContainer->GetObject("test1")->GetComponent<Transform>()->m_position
			= InputHandler::m_perspPosition;
	}

	if (InputHandler::KeyTriggered(JE_MOUSE_RIGHT))
		JE_DEBUG_PRINT("Right Mouse\n");

	if (InputHandler::KeyTriggered(JE_MOUSE_MIDDLE))
		JE_DEBUG_PRINT("Middle Mouse\n");

	if (InputHandler::KeyPressed(JE_MOUSE_WHEEL_DOWN))
		JE_DEBUG_PRINT("Mouse Wheel Down\n");

	if (InputHandler::KeyPressed(JE_MOUSE_WHEEL_UP))
		JE_DEBUG_PRINT("Mouse Wheel Up\n");

	if (InputHandler::KeyPressed(JE_ESC)) {
		JE_DEBUG_PRINT("Quit\n");
		StateManager::Quit();
	}


	std::cout << InputHandler::m_orthoPosition << std::endl;
}

void State::Close()
{
	JE_DEBUG_PRINT("Closing %s...\n", m_name.c_str());
	SystemManager::Close();
}

void State::Unload()
{
	JE_DEBUG_PRINT("Unloading %s...\n", m_name.c_str());
	ClearObjectContainer();
	SystemManager::Unload();
}

void State::ClearObjectContainer()
{
	if (m_objContainer) {
		delete m_objContainer;
		m_objContainer = nullptr;
	}
}

NS_JE_END

