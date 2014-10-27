#ifndef MISSION_INFO_ITEM_H_
#define MISSION_INFO_ITEM_H_

#include "cocos2d.h"
#include "GameMacros.h"

USING_NS_CC;

class MissionInfoItem : public cocos2d::Node
{	
public:
	MissionInfoItem();
	~MissionInfoItem();
	virtual bool init();
	virtual bool initWithMissionInfo(MissionInfo info);
	MissionInfo & getInfo();

	static MissionInfoItem * create();
	static MissionInfoItem * create(MissionInfo info);

private:
	Sprite *_headImage;
	LabelTTF *_misIdx;
	LabelTTF *_stepCount;	
	Node *_usedTime;
	MissionInfo _info;
};

#endif // !MISSION_INFO_ITEM_H_
