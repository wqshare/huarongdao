#ifndef USER_DATA_H_
#define USER_DATA_H_

#include "cocos2d.h"
#include "GameMacros.h"

USING_NS_CC;
class UserData : public cocos2d::Ref
{
public:
	UserData();
	~UserData();

	int getMissionIdx() {return _currentMission;}
	int getStepCount() {return _stepCount;}
	unsigned getTime() {return _time;}
	bool isDone() {return _isDone;}

	void setMissionIdx(int idx){_currentMission = idx;}
	void setStepCount(unsigned count){_stepCount = count;}
	void setTime(long dt) {_time = dt;}
	void setIsDone(bool is) {_isDone = is; }

private:
	int _currentMission;							//当前关卡索引
	unsigned int _stepCount;						//步数
	long _time;							//时间
	HeroType _layout[ROWS][COLUMS];				//武将布局
	bool _isDone;								//是否通关
};



#endif // !USER_DATA_H_
