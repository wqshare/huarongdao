#include "MissionInfoItem.h"

USING_NS_CC;

bool MissionInfoItem::init()
{
	if(!Node::init())
		return false;

	_headImage = Sprite::create("ic_launcher.png");
	this->addChild(_headImage);

	return true;
}

bool MissionInfoItem::initWithMissionInfo(MissionInfo info)
{
	const char * FontPath = "";
	bool ret = false;
	_info = info;

	String indexStr;
	String stepStr;
	String timeStr;

	if(info.index == 1)
		indexStr.initWithFormat(GET_STR("DirectMission"), ' ');
	else
		indexStr.initWithFormat(GET_STR("mission"), info.index);
	stepStr.initWithFormat(GET_STR("totalStep"), info.step);

	_headImage = Sprite::create("ic_launcher.png");
	_misIdx = LabelTTF::create(indexStr.getCString(), FontPath, 20);

	if(!info.isDone && info.isLocked)
	{
		_usedTime = Sprite::create("lock.png");
		_stepCount = LabelTTF::create(GET_STR("noPass"), FontPath, 20);
	}
	else if(!info.isDone && !info.isLocked)	
	{
		_usedTime = Sprite::create();
		_stepCount = LabelTTF::create(GET_STR("noPass"), FontPath, 20);
	}
	else
	{
		int hour = info.usedTime / 3600;
		int min = (info.usedTime % 3600) / 60;
		int sec = info.usedTime - hour * 3600 - min * 60;

		timeStr.initWithFormat(GET_STR("time"), hour, min, sec);

		_stepCount = LabelTTF::create(stepStr.getCString(), FontPath, 20);
		_usedTime = LabelTTF::create(timeStr.getCString(), FontPath, 20);
	}


	this->setAnchorPoint(Vec2::ZERO);
	_headImage->setPosition(75, 50);
	_misIdx->setPosition(200, 50);
	_stepCount->setPosition(300, 50);
	_usedTime->setPosition(450, 50);		

	this->addChild(_headImage);
	this->addChild(_misIdx);
	this->addChild(_stepCount);
	this->addChild(_usedTime);
	this->setContentSize(Size(540, 100));
	ret = true;


	return ret;
}

MissionInfoItem * MissionInfoItem::create()
{
	auto pRet = new MissionInfoItem();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

MissionInfoItem * MissionInfoItem::create(MissionInfo info)
{
	auto pRet = new MissionInfoItem();
	if (pRet && pRet->initWithMissionInfo(info))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

MissionInfoItem::MissionInfoItem():Node(), _headImage(NULL), _misIdx(NULL), _stepCount(NULL), _usedTime(NULL)
{

}

MissionInfoItem::~MissionInfoItem()
{

}

MissionInfo & MissionInfoItem::getInfo()
{
	return _info;
}
