#pragma once
#include "Macro.h"

JE_BEGIN

class Object;
class Component;

class ComponentBuilder {

public:

	virtual Component* CreateComponent(Object* _pOwner) const = 0;

protected:

	friend class AssetManager;
	friend class ComponentManager;

	ComponentBuilder() {};
	~ComponentBuilder() {};
	ComponentBuilder(const ComponentBuilder& /*_copy*/) = delete;
	void operator=(const ComponentBuilder& /*_copy*/) = delete;

};

#define JE_DECLARE_COMPONENT_BUILDER(c) class JE_CONCAT(c, Builder) : public ComponentBuilder {	friend class JEngine; JE_CONCAT(c, Builder)();	~JE_CONCAT(c, Builder)() {}; JE_CONCAT(c, Builder)(const JE_CONCAT(c, Builder)& /*_copy*/) = delete; void operator=(const JE_CONCAT(c, Builder)& /*_copy*/) = delete; JE_CONCAT(c, Builder)(JE_CONCAT(c, Builder) && /*_copy*/) = delete; void operator=(JE_CONCAT(c, Builder) && /*_copy*/) = delete; CustomComponent* CreateComponent(Object* _pOwner) const override; };
#define JE_DEFINE_COMPONENT_BUILDER(c)	JE_CONCAT(c, Builder)::JE_CONCAT(c, Builder)() : ComponentBuilder(){} CustomComponent* JE_CONCAT(c, Builder)::CreateComponent(Object* _pOwner) const {	return new (c)(_pOwner);}

JE_END