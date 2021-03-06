#pragma once
#include "CustomComponent.h"
#include "ComponentManager.h"
#include "ComponentBuilder.h"

jeBegin

class Object;
struct Telegram;

class StateMachine : public CustomComponent
{
    jeBaseFriends(StateMachine);

public:

private:

    StateMachine(Object* pObject);
    ~StateMachine() {};

    void Register() override;
    void Load(CR_RJValue _data) override;
    void Init() override;
    void Update(float dt) override;
    void Close() override;
    void Unload() override;
    bool OnMessage(Telegram& msg) override { msg; return false; }

    void EditorUpdate(float dt) override;

};

jeDeclareCustomComponentBuilder(StateMachine);
jeEnd
