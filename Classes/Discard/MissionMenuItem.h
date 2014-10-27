#ifndef MISSION_MENU_ITEM_H_
#define MISSION_MENU_ITEM_H_

#include "cocos2d.h"
#include "GameMacros.h"

class MissionMenuItem : public cocos2d::MenuItem
{
public:
	virtual bool init();
	virtual bool initWithMissionInfo(std::string headImage, MissionInfo info);

	static MissionMenuItem *create();
	static MissionMenuItem *create(std::string headImage, MissionInfo info);

};

#endif // !MISSION_MENU_ITEM_H_
