#ifndef SELECT_MISSION_SCENE_H_
#define  SELECT_MISSION_SCENE_H_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;
USING_NS_CC;

#define MISSION_COUNT 40

class SelectMissionScene : public cocos2d::Layer, public TableViewDataSource, public TableViewDelegate
{
public:
	static cocos2d::Scene *createScene();

	virtual bool init();

	CREATE_FUNC(SelectMissionScene);

#ifdef USE_LIST_VIEW

	virtual bool onTouchBeganCallback(Touch *touch, Event *event);
	virtual void onTouchMovedCallback(Touch* touch, Event  *event);
#endif // USE_LIST_VIEW

	virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);

	virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);

	virtual ssize_t numberOfCellsInTableView(TableView *table);

	virtual void tableCellTouched(TableView* table, TableViewCell* cell);

	virtual void tableCellHighlight(TableView* table, TableViewCell* cell);

	virtual void tableCellUnhighlight(TableView* table, TableViewCell* cell);

	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view);

	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view){};


	virtual void onEnter();
	virtual void onExit();

	void refresh();

	void hiedSlider(float dt);

private:
	cocos2d::Vec2 _beginPos;
	ControlSlider *slider;
	TableView *table;

	void slideCallback(Object *sender, Control::EventType controlEvent); 

#ifdef USE_MENU_ITEM
	cocos2d::Menu *_itemMenu;

#endif // USE_MENU_ITEM

};





class MyScene : public cocos2d::Scene
{
public:
	CREATE_FUNC(MyScene);
	
	virtual bool init();

	MyScene();
	virtual ~MyScene();

private:
	virtual void onEnter();

	CC_SYNTHESIZE_RETAIN(SelectMissionScene *, _layer, SelectLayer);
};
#endif // !SELECT_MISSION_SCENE_H_
