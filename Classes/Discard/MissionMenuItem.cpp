#include "MissionMenuItem.h"

USING_NS_CC;

bool MissionMenuItem::init()
{
	if(!MenuItem::init())
		return false;

	return true;
}

bool MissionMenuItem::initWithMissionInfo(std::string headImage, MissionInfo info)
{
	if(!this->init())
		return false;
	char indexStr[20];
	char stepStr[20];
	char timeStr[20];
// 	sprintf_s(indexStr, "Mission %d", info.index);
// 	sprintf_s(stepStr, "Total Step: %d", info.step);
// 	sprintf_s(timeStr, "Spend Time: %d", info.usedTime);

	MenuItemImage *head = MenuItemImage::create(headImage, headImage);

	TTFConfig ttfConfig("fonts/Marker Felt.ttf", 12);

	MenuItemLabel *missionNo = MenuItemLabel::create( Label::createWithTTF(ttfConfig, indexStr));
	MenuItemLabel *stepCount = NULL;
	MenuItem *usedTime = NULL;

	if(info.isDone)
	{
		stepCount = MenuItemLabel::create(Label::createWithTTF(ttfConfig, stepStr));
		usedTime = MenuItemLabel::create(Label::createWithTTF(ttfConfig, timeStr));
	}
	else
	{
		stepCount = MenuItemLabel::create(Label::createWithTTF(ttfConfig, "Unfinished" ));
		usedTime = MenuItemImage::create("lock.png", "lock.png");
	}
	
	this->setContentSize(Size(520, 150));
	this->setAnchorPoint(Vec2::ZERO);

	head->setPosition(head->getContentSize().width / 2, this->getContentSize().height / 2);
	missionNo->setPosition(head->getContentSize().width + 100, this->getContentSize().height / 2);
	stepCount->setPosition(head->getContentSize().width + 200, this->getContentSize().height / 2);
	usedTime->setPosition(head->getContentSize().width + 300, this->getContentSize().height / 2);


	this->addChild(head);
	this->addChild(missionNo);
	this->addChild(stepCount);
	this->addChild(usedTime);

	return true;

}

MissionMenuItem * MissionMenuItem::create()
{
	auto p = new MissionMenuItem();
	if( p && p->init())
	{
		p->autorelease();
		return p;
	}
	else
	{		
		delete p;
		p = NULL;
		return NULL;
	}
}

MissionMenuItem * MissionMenuItem::create(std::string headImage, MissionInfo info)
{
	auto p = new MissionMenuItem();
	if( p && p->initWithMissionInfo(headImage, info))
	{
		p->autorelease();
		return p;
	}
	else
	{
		delete p;
		p = NULL;	
		return NULL;
	}
}
