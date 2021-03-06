#pragma once
#include "JsonParser.h"

jeBegin

class System {

protected:

	virtual void Load(CR_RJDoc data) = 0;
	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Close() = 0;
	virtual void Unload() = 0;

	System() {};
	virtual ~System() {};

private:

	System(System&&) = delete;
	System(const System&) = delete;
	System& operator=(System&&) = delete;
	System& operator=(const System&) = delete;

};

jeEnd
