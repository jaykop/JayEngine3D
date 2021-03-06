#pragma once
#include "CustomComponent.h"
#include "ComponentBuilder.h"
#include "ComponentManager.h"
#include "Timer.h"

jeBegin

class Object;
class Camera;
struct Telegram;
class Transform;

class TestLogic : public CustomComponent
{
    template <class T>
    friend class MemoryAllocator;

    friend class ComponentManager;
    friend class TestLogicBuilder;

public:

	Object * m_ortho, *m_pers;
	Camera *camera;
	Transform* pointer;
private:

    std::vector <Object*> m_list;

    TestLogic(Object* pObject);
    ~TestLogic() {};

    static int a;

    void Register() override;
    void Load(CR_RJValue _data) override;
    void Init() override;
    void Update(float dt) override;
    void Close() override;
    void Unload() override;
    bool OnMessage(Telegram& msg) override { msg; return false; }

    void EditorUpdate(float dt) override;

    Timer t;
};

jeDeclareCustomComponentBuilder(TestLogic);
jeEnd
