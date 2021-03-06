#pragma once
#include "CustomComponent.h"
#include "ComponentManager.h"
#include "ComponentBuilder.h"
#include "Vector3.h"

jeBegin

class Text;
class Object;
class Transform;
struct Telegram;

/////////////////////////////////////////////////////////////////////////
// Global bully state
/////////////////////////////////////////////////////////////////////////
class BullyState : public CustomComponent
{
    jeBaseFriends(BullyState);

public:

    unsigned	m_hateMiner = 0;
    int		m_minerId = 0;

    const char	*m_content = nullptr;

    Object	*m_bullyTalks = nullptr, *m_pMiner = nullptr;
    Text*	m_talkText = nullptr;
    Transform*	m_pTransform = nullptr, *m_pTalkTransform = nullptr;
    vec3	m_talkOffset;

private:

    BullyState(Object* pObject);
    ~BullyState() {};

    void Register() override;
    void Load(CR_RJValue _data) override;
    void Init() override;
    void Update(float dt) override;
    void Close() override;
    void Unload() override {};
    bool OnMessage(Telegram& msg) override;

    void EditorUpdate(const float /*dt*/) override {};

};

/////////////////////////////////////////////////////////////////////////
// Hating state
/////////////////////////////////////////////////////////////////////////
class JustHatingMiner : public CustomComponent
{
    jeBaseFriends(JustHatingMiner);

public:

    BullyState * m_globalState = nullptr;

private:

    JustHatingMiner(Object* pObject);
    ~JustHatingMiner() {};

    void Register() override;
    void Load(CR_RJValue _data) override;
    void Init() override;
    void Update(float dt) override;
    void Close() override;
    void Unload() override {};
    bool OnMessage(Telegram& msg) override;

    void EditorUpdate(const float /*dt*/) override {};

};

/////////////////////////////////////////////////////////////////////////
// Go fight to miner
/////////////////////////////////////////////////////////////////////////
class GoFight : public CustomComponent
{
    jeBaseFriends(GoFight);

public:

    BullyState * m_globalState = nullptr;
    bool	m_beaten = false;

private:

    GoFight(Object* pObject);
    ~GoFight() {};

    void Register() override;
    void Load(CR_RJValue _data) override;
    void Init() override;
    void Update(float dt) override;
    void Close() override;
    void Unload() override {};
    bool OnMessage(Telegram& msg) override;

    void EditorUpdate(const float /*dt*/) override {};

};

jeDeclareCustomComponentBuilder(BullyState);
jeDeclareCustomComponentBuilder(JustHatingMiner);
jeDeclareCustomComponentBuilder(GoFight);

jeEnd
