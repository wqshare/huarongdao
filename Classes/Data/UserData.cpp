#include "UserData.h"




UserData::UserData():_stepCount(0), _time(0), _currentMission(0)
{
	
}

UserData::~UserData()
{
	Ref::~Ref();
}

