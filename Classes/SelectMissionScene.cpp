#include "SelectMissionScene.h"
#include "MissionInfoItem.h"
#include "GameMainScene.h"
#include "GameMacros.h"
#include "GameManager.h"
#include "NewGameManiScene.h"

#ifdef USE_MENU_ITEM
#include "Discard/MissionMenuItem.h"
#endif // USE_MENU_ITEM


USING_NS_CC;
USING_NS_CC_EXT;


bool SelectMissionScene::init()
{
	if( !Layer::init())
		return false;



	GameManager::getInstance()->loadMissionData();

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size winSize = Director::getInstance()->getWinSize();

	//set background image
// 	Sprite *bg = Sprite::create("choose_bg.png");
// 	bg->setAnchorPoint(Vec2(0.5, 0.5));
// 	bg->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 );	

	Sprite *bgSprite = Sprite::create("choose_bg.png");
	bgSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	bgSprite->setPosition(winSize.width / 2, winSize.height / 2);
	float scale_x = winSize.width / bgSprite->getContentSize().width;
	float scale_y = winSize.height / bgSprite->getContentSize().height;
	bgSprite->setScaleX(scale_x);
	bgSprite->setScaleY(scale_y);
// 	if(scale_x > scale_y)
// 		bgSprite->setScaleX(scale_x);
// 	else
// 		bgSprite->setScale(scale_y);

#ifdef USE_MENU_ITEM
	//set a sub layer to contain menu
	Layer *menuLayer = Layer::create();
	menuLayer->setAnchorPoint(Vec2(menuLayer->getContentSize().width / 2, menuLayer->getContentSize().height / 2));
	menuLayer->setContentSize(Size(visibleSize.width - 120, visibleSize.height - 200));
	menuLayer->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

	//set menu list
	_itemMenu = Menu::create();
	for( int i = 0; i < MISSION_COUNT; ++i )
	{
		MissionMenuItem *menuItem = MissionMenuItem::create("ic_launcher.png", g_mission_items[i]);
		menuItem->setPosition(origin.x + 60, visibleSize.height + origin.y - 100 - (i+1) * 150);
		_itemMenu->addChild(menuItem);
	}

	_itemMenu->setPosition(Vec2::ZERO - menuLayer->getAnchorPoint());
	_itemMenu->setContentSize(Size(visibleSize.width - 120, (MISSION_COUNT + 1) * 150));

	// Register Touch Event
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(SelectMissionScene::onTouchBeganCallback, this);

	listener->onTouchMoved = CC_CALLBACK_2(SelectMissionScene::onTouchMovedCallback, this);

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto mouseListener = EventListenerMouse::create();

	mouseListener->onMouseScroll = [&](Event  *event){

		auto mouseEvent = static_cast<EventMouse*>(event);
		float nMoveY = mouseEvent->getScrollY() * 10;

		auto curPos  = _itemMenu->getPosition();
		auto nextPos = Vec2(curPos.x, curPos.y + nMoveY);

		_itemMenu->setPosition(nextPos);	
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

	menuLayer->addChild(_itemMenu);
	this->addChild(menuLayer, 2);

#endif // !USE_TABLE_VIEW


#ifdef USE_LIST_VIEW
	//set ListView
	cocos2d::ui::ListView *listView = ListView::create();

	for(int i = 0; i < MISSION_COUNT; ++i)
	{
		auto item = MissionInfoItem::create(g_mission_items[i]);
		listView->pushBackCustomItem(item);
	}

	listView->setContentSize(Size(520, 700));
	listView->setItemsMargin(1);
	listView->setPosition(Vec2(60, 130));

	// Register Touch Event
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = [&](Touch *touch, Event *event){
		_beginPos = touch->getLocation();
		return true;
	};

	listener->onTouchEnded = [&](Touch *touch, Event *event){
		auto curPos = touch->getLocation();
		if (curPos == _beginPos)
		{
			int idx = listView->getCurSelectedIndex();
			Director::getInstance()->replaceScene(TransitionProgress::create(1.0f, Scene::create()));
		}

	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, listView);
	this->addChild(listView);

#endif // USE_LIST_VIEW
	//add slider bar
	slider = ControlSlider::create("extensions/sliderBg.png","extensions/sliderBg.png" , "extensions/rect.png");
	slider->setPosition(visibleSize.width - 50, visibleSize.height / 2 );
	//slider->setContentSize(Size(slider->getContentSize().width, 700));
	slider->setScaleX(700 / slider->getContentSize().width);
	slider->setMinimumValue(0.0f);
	slider->setMaximumValue(100.0f);
	slider->setRotation(90);
	slider->setTag(180);
	slider->setEnabled(false);
	slider->addTargetWithActionForControlEvents(this, cccontrol_selector(SelectMissionScene::slideCallback), Control::EventType::VALUE_CHANGED);

	table = TableView::create(this, Size(520, 700));
	table->setDelegate(this);
	table->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
	table->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);  
	//table->reloadData();

	table->setTag(500);
// 	table->setAnchorPoint(Vec2::ZERO);
// 	table->setPosition(origin.x + 60, origin.y + 140);
	table->setAnchorPoint(Vec2(0.5, 0.5));
	table->ignoreAnchorPointForPosition(false);
	table->setPosition(origin.x + visibleSize.width / 2 , origin.y + visibleSize.height / 2);

	MenuItemImage *returnItem = MenuItemImage::create("gohome.png", "gohome.png", [](Ref *pSender){
		Director::getInstance()->popScene();
	});

	returnItem->setPosition(visibleSize.width / 2, origin.y + 50);

	Menu *menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	menu->addChild(returnItem);


	this->addChild(slider, 15);
	this->addChild(table, 10);
	this->addChild(menu, 10);
	this->addChild(bgSprite);
	
	EventListenerKeyboard *keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event)
	{
		log("key Press!");
		if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE )
		{
			Director::getInstance()->popToRootScene();
		}

	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	return true;

}



cocos2d::Scene * SelectMissionScene::createScene()
{
	auto scene = MyScene::create();

	// 'layer' is an autorelease object
	auto layer = SelectMissionScene::create();

	scene->setSelectLayer(layer);
	// add layer as a child to scene
	//scene->addChild(layer);

	// return the scene
	return scene;
}

#ifdef USE_MENU_ITEM


bool SelectMissionScene::onTouchBeganCallback(Touch *touch, Event *event)
{
	_beginPos = touch->getLocation();
	return true;
}


void SelectMissionScene::onTouchMovedCallback(Touch* touch, Event  *event)
{

	auto touchLocation = touch->getLocation();    
	float nMoveY = (touchLocation.y - _beginPos.y) / 10;

	auto curPos  = _itemMenu->getPosition();
	auto nextPos = Vec2(curPos.x, curPos.y + nMoveY);

	if(nextPos.y < 10.0f)
		return;


	_itemMenu->setPosition(nextPos);		
}


#endif // USE_MENU_ITEM


cocos2d::Size SelectMissionScene::tableCellSizeForIndex(TableView *table, ssize_t idx)
{
	return Size(520, 100);
}

TableViewCell* SelectMissionScene::tableCellAtIndex(TableView *table, ssize_t idx)
{
	log("%d:geted!", idx);
	GameManager *manager = GameManager::getInstance();
	MissionInfoItem *info = MissionInfoItem::create(manager->getMissionStatus()[idx]);

	TableViewCell *cell = table->dequeueCell();
	if(!cell)
	{
		cell = TableViewCell::create();		//or cell = new TTableViewCell(); cell->autorelease()	
	}
	else
	{
		MissionInfoItem *tmp = static_cast<MissionInfoItem *>(cell->getChildByTag(100));
		tmp->removeFromParent();
	}

	info->setAnchorPoint(Vec2::ZERO);
	info->setPosition(Vec2::ZERO);
	info->setTag(100);
	cell->addChild(info);

#ifdef TEST
	/************ Test TableView ******************/
	auto index_text = __String::createWithFormat("%ld",idx + 1);
	if(!cell)
	{
		Label *label = Label::create(index_text->getCString(), "Marker Felt.ttf", 18);
		label->setPosition( 80, 50 );
		label->setTag(150);

		cell = TableViewCell::create();
		Sprite *sprite = Sprite::create("lock.png");
		sprite->setPosition(270, 50);
		sprite->setTag(100);

		cell->addChild(label );
		cell->addChild(sprite);
	}
	else
	{
		Label *label = static_cast<Label *>(cell->getChildByTag(150));
		label->setString(index_text->getCString());
	}

#endif // TEST





	return cell;
}

ssize_t SelectMissionScene::numberOfCellsInTableView(TableView *table)
{
	return MISSION_COUNT;
}

void SelectMissionScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
	auto sprite = static_cast<MissionInfoItem *>(cell->getChildByTag(100));
	if(sprite->getInfo().isLocked)
	{
		return;
	}

	//Director::getInstance()->pushScene(TransitionSlideInR::create(0.5f, GameMainScene::createScene(cell->getIdx(), false)));
	Director::getInstance()->pushScene(TransitionSlideInR::create(0.2f, NewGameMainScene::createScene(cell->getIdx(), false)));

}

void SelectMissionScene::tableCellHighlight(TableView* table, TableViewCell* cell)
{
	LayerColor *shadeLayer = LayerColor::create(Color4B(0, 0, 170, 100));
	shadeLayer->setContentSize(Size(540, 100));
	shadeLayer->setTag(5000);
	cell->addChild(shadeLayer);
}

void SelectMissionScene::tableCellUnhighlight(TableView* table, TableViewCell* cell)
{
	LayerColor *shadeLayer= static_cast<LayerColor *>(cell->getChildByTag(5000));
	shadeLayer->removeFromParent();
}

void SelectMissionScene::onEnter()
{
 	Layer::onEnter();
// 
// 	static_cast<TableView *>(this->getChildByTag(500))->reloadData();
}

void SelectMissionScene::onExit()
{
	Layer::onExit();
}

void SelectMissionScene::refresh()
{
	static_cast<TableView *>(this->getChildByTag(500))->reloadData();
}

void SelectMissionScene::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
	auto offset = view->getContentOffset();
	auto contentSize = view->getContentSize();
	auto viewSize = view->getViewSize();

	auto value = (1 + (offset.y / (contentSize.height - viewSize.height))) * 100.0f;
	slider->setValue(value);

	log("offset: %f, %f", offset.x, offset.y);
	log("contentSize: %f, %f", contentSize.width, contentSize.height);
	log("viewSize: %f, %f", viewSize.width, viewSize.height);
	log("viewSize: %f, %f", viewSize.width, viewSize.height);
	//log("value: %f", value);
}

void SelectMissionScene::slideCallback(Object *sender, Control::EventType controlEvent)
{
// 	auto slide_control = (ControlSlider*)sender;//通过回调参数sender 获得ControlSlider  
// 	int current_value = slide_control->getValue();//获取slide当前的值  
// 		
// 	Size realSize = table->getContentSize() - table->getViewSize();
// 	float precent = 1 - (current_value / slide_control->getMaximumValue());
// 	table->setContentOffset(Vec2(0.0f, 0 - realSize.height * precent));
	//table->setContentOffset(table->getViewSize() * current_value);
}



/************************************************************************/
/* My Scene Implementation                                                                     */
/************************************************************************/

bool MyScene::init()
{
	if(!Scene::init())
		return false;
	
	return true;
}

void MyScene::onEnter()
{	
	Scene::onEnter();

	if(getSelectLayer()->getParent() != NULL)
	{
		getSelectLayer()->removeFromParent();
	}

	auto layer = SelectMissionScene::create();

	this->setSelectLayer(layer);
	this->addChild(getSelectLayer());
}

MyScene::MyScene():_layer(NULL)
{

}

MyScene::~MyScene()
{
	CC_SAFE_RELEASE(_layer);
}
