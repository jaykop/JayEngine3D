#pragma once
#include "Macro.h"

JE_BEGIN

class Timer {

public:

	Timer():m_time(0.f) {};
	~Timer() {};

	void	Start(void);
	float	GetTime(void);

private:

	Timer(const Timer& /*_cpoy*/) = delete;
	void operator=(const Timer& /*_cpoy*/) = delete;

	float m_time;

};

JE_END