#include <ctime>
#include "Timer.h"

jeBegin

/******************************************************************************/
/*!
\brief - Start to check time
*/
/******************************************************************************/
void Timer::Start(void)
{
	m_time = static_cast<float>(clock());
}

/******************************************************************************/
/*!
\brief - Get spent time from StartTime() moment
*/
/******************************************************************************/
float Timer::GetTime(void)
{
	return (static_cast<float>(clock()) - m_time) / static_cast<float>(CLOCKS_PER_SEC);
}

jeEnd

