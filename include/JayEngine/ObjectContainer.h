#pragma once
#include "Macro.h"
#include <unordered_map>

NS_JE_BEGIN

class Object;
typedef std::unordered_map<std::string, Object*> ObjectMap;

class ObjectContainer {

	friend class State;
	friend class Object;
	friend class ObjectFactory;

public:

	void		RemoveObject(const char* _name);
	Object*		GetObject(const char* _name);
	ObjectMap*	GetObjectMap();
	bool		HasObject(const char* _name);

	template <typename ComponentType>
	inline ObjectMap GetObjects();

private:

	~ObjectContainer();
	ObjectContainer() {};
	ObjectContainer(const ObjectContainer& /*_copy*/) {};
	void operator=(const ObjectContainer& /*_copy*/) {};

	void		ClearObjectMap();

	ObjectMap	m_objectMap;
};

NS_JE_END

#include "ObjectContainer.inl"