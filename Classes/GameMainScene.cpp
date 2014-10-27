#include "GameMainScene.h"
#include "GameManager.h"
#include "SelectMissionScene.h"
#include "Control/PopLayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 

#include "platform/android/jni/JniHelper.h"
#include "jni.h"

#endif


enum ButtonType {
	BT_CHEAT_OK = 1000,
	BT_CHEAT_CANCEL,
	BT_RESET_OK,
	BT_RESET_CANCEL,
	BT_NEXT_MISSION_OK,
	BT_NEXT_MISSION_SHARE,
	BT_SELECT_MISSSION_OK,
	BT_SELECT_MISSSION_CANCEL,
	BT_BUY_OK,
	BT_BUY_CANCEL
};

enum Epay{
	FEE_RESULT_SUCCESS = 101,
	FEE_RESULT_CANCELED,
	FEE_RESULT_FAILED
};

GameMainScene::GameMainScene() :
	_winCell(170, 100, 290, 290), _heros(), _layoutInfo(), _currentDirection(),
	_currentPos(), _beginPos(), _strStep(GET_STR("step"))
{

}

Scene * GameMainScene::createScene(int idx, bool isContinue) {
	auto scene = Scene::create();

	auto layer = GameMainScene::create(idx, isContinue);
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameMainScene::init() {
	if (!Layer::init())
		return false;
	return true;
}

bool GameMainScene::initWithMissionIdx(int idx, bool isContinue) {
	if (!this->init())
		return false;
	//Director::getInstance()->getOpenGLView()->setDesignResolutionSize(640, 960, ResolutionPolicy::FIXED_WIDTH);

	_userData.index = idx + 1;
	_userData.step = 0;
	_userData.usedTime = 0;
	_userData.isDone = false;
	_userData.isLocked = false;

	Size visiableSize = Director::getInstance()->getVisibleSize();

	GameManager::getInstance()->getHeroLayoutInfoByIdx(idx, _layoutInfo);
	if (isContinue)
		GameManager::getInstance()->loadCurrentMission(_layoutInfo);

	auto heros = GameManager::getInstance()->getHeros();

	//register EventListner
	EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch *touch, Event *event) {

		auto sprite = static_cast<Sprite *>(event->getCurrentTarget());

		auto rect = sprite->getBoundingBox();
		auto point = touch->getLocation();

		if(rect.containsPoint(point))
		{
			_beginPos = point;
			_currentPos = sprite->getPosition();
			_currentDirection = checkMoveDirection(sprite);
			return true;
		}
		return false;

	};

	listener->onTouchMoved = [&](Touch *touch, Event *event) {

		auto sprite = static_cast<Sprite *>(event->getCurrentTarget());

		float offsetX = touch->getLocation().x - _beginPos.x;
		float offsetY = touch->getLocation().y - _beginPos.y;

		switch (_currentDirection)
		{
		case CanMovedDirection::UNMOVEABLE:
			return;
		case CanMovedDirection::MOVEABLE_UP:
			if(offsetY <= 0) offsetY = 0;
			if(offsetY >= CELL_HEIGHT) offsetY = CELL_HEIGHT;
			sprite->setPosition(_currentPos.x, _currentPos.y + offsetY );
			break;

		case CanMovedDirection::MOVEABLE_RIGHT:
			if(offsetX <= 0) offsetX = 0;
			if(offsetX >= CELL_WIDTH) offsetX = CELL_WIDTH;
			sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			break;

		case CanMovedDirection::MOVEABLE_LEFT:
			if(offsetX >= 0) offsetX = 0;
			if(offsetX <= 0 - CELL_WIDTH) offsetX = 0 - CELL_WIDTH;
			sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			break;

		case CanMovedDirection::MOVEABLE_DOWN:
			if(offsetY >= 0) offsetY = 0;
			if(offsetY <= 0 - CELL_HEIGHT) offsetY = 0 - CELL_HEIGHT;
			sprite->setPosition(_currentPos.x , _currentPos.y + offsetY);
			break;

		case CanMovedDirection::MOVEABLE_LEFT_RIGHT:
			if(offsetX >= CELL_WIDTH ) offsetX = CELL_WIDTH;
			if(offsetX <= 0 - CELL_WIDTH) offsetX = 0 - CELL_WIDTH;
			sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			break;

		case CanMovedDirection::MOVEABLE_UP_DOWN:
			if(offsetY >= CELL_HEIGHT ) offsetX = CELL_HEIGHT;
			if(offsetY <= 0 - CELL_HEIGHT) offsetX = 0 - CELL_HEIGHT;
			sprite->setPosition(_currentPos.x, _currentPos.y + offsetY);
			break;

		case CanMovedDirection::MOVEABLE_LEFT_DOWN:
			if(ABS(offsetX) >= ABS(offsetY)) //水平方向偏移绝对值 大于 竖直方向偏移绝对值
			{
				if(offsetX >= 0) offsetX = 0;
				if(offsetX <= 0 - CELL_WIDTH) offsetX = 0 - CELL_WIDTH;
				sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			}
			else
			{
				if(offsetY >= 0) offsetY = 0;
				if(offsetY <= 0 - CELL_HEIGHT) offsetY = 0 - CELL_HEIGHT;
				sprite->setPosition(_currentPos.x , _currentPos.y + offsetY);
			}
			break;

		case CanMovedDirection::MOVEABLE_LEFT_UP:
			if(ABS(offsetX) >= ABS(offsetY)) //水平方向偏移绝对值 大于 竖直方向偏移绝对值
			{
				if(offsetX >= 0) offsetX = 0;
				if(offsetX <= 0 - CELL_WIDTH) offsetX = 0 - CELL_WIDTH;
				sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			}
			else
			{
				if(offsetY <= 0) offsetY = 0;
				if(offsetY >= CELL_HEIGHT) offsetY = CELL_HEIGHT;
				sprite->setPosition(_currentPos.x, _currentPos.y + offsetY );
			}
			break;

		case CanMovedDirection::MOVEABLE_RIGHT_DOWN:
			if(ABS(offsetX) >= ABS(offsetY)) //水平方向偏移绝对值 大于 竖直方向偏移绝对值
			{
				if(offsetX <= 0) offsetX = 0;
				if(offsetX >= CELL_WIDTH) offsetX = CELL_WIDTH;
				sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			}
			else
			{
				if(offsetY >= 0) offsetY = 0;
				if(offsetY <= 0 - CELL_HEIGHT) offsetY = 0 - CELL_HEIGHT;
				sprite->setPosition(_currentPos.x , _currentPos.y + offsetY);
			}
			break;

		case CanMovedDirection::MOVEABLE_RIGHT_UP:
			if(ABS(offsetX) >= ABS(offsetY)) //水平方向偏移绝对值 大于 竖直方向偏移绝对值
			{
				if(offsetX <= 0) offsetX = 0;
				if(offsetX >= CELL_WIDTH) offsetX = CELL_WIDTH;
				sprite->setPosition(_currentPos.x + offsetX, _currentPos.y);
			}
			else
			{
				if(offsetY <= 0) offsetY = 0;
				if(offsetY >= CELL_HEIGHT) offsetY = CELL_HEIGHT;
				sprite->setPosition(_currentPos.x, _currentPos.y + offsetY );
			}
			break;

		default:
			break;
		}

	};

	listener->onTouchEnded =
		[&](Touch *touch, Event *event)
	{
		auto sprite = static_cast<Sprite *>(event->getCurrentTarget());

		float offsetX = sprite->getPosition().x - _beginPos.x;
		float offsetY = sprite->getPosition().y - _beginPos.y;

		do
		{
			if(ABS(offsetX)>= CELL_WIDTH / 2 && offsetX > 0)
			{
				sprite->setPosition(_currentPos.x + CELL_WIDTH, _currentPos.y);
				_userData.step++;
				break;
			}
			if(ABS(offsetX)>= CELL_WIDTH / 2 && offsetX < 0)
			{
				sprite->setPosition(_currentPos.x - CELL_WIDTH, _currentPos.y);
				_userData.step++;
				break;
			}
			if(ABS(offsetY) >= CELL_HEIGHT / 2 && offsetY > 0)
			{
				sprite->setPosition(_currentPos.x , _currentPos.y + CELL_HEIGHT);
				_userData.step++;
				break;
			}
			if(ABS(offsetY) >= CELL_HEIGHT / 2 && offsetY < 0)
			{
				sprite->setPosition(_currentPos.x , _currentPos.y - CELL_HEIGHT);
				_userData.step++;
				break;
			}
			sprite->setPosition(_currentPos);
		}while (0);

		refreshLayoutInfo();

		if(this->_winCell.containsPoint(GameManager::getInstance()->getHeros().at(HeroType::CAO_CAO)->getPosition()))
		{
			// 			PopLayer *popLayer = PopLayer::create("message.png");
			// 			popLayer->setTitle("恭喜你， 过关了");
			// 			popLayer->setContentText("");
			// 			popLayer->addButton("b6.png", "b6.png", "下一关", ButtonType::BT_NEXT_MISSION_OK);
			// 			popLayer->addButton("b6.png", "b6.png", "分享给好友", ButtonType::BT_NEXT_MISSION_SHARE);
			// 			popLayer->setCallbackFunc(this, (SEL_CallFuncN)&GameMainScene::popLayerCallFunc);
			// 			this->addChild(popLayer, 50);
			// 
			// 			_userData.isDone = true;
			// 			GameManager::getInstance()->saveMissionData(_userData);
			// 
			// 			MissionInfo nextMision;
			// 			nextMision.index = _userData.index + 1;
			// 			nextMision.step = 0;
			// 			nextMision.usedTime = 0;
			// 			nextMision.isDone = false;
			// 			nextMision.isLocked = false;
			// 			GameManager::getInstance()->saveMissionData(nextMision);	

			this->scheduleOnce(SEL_SCHEDULE(&GameMainScene::winFunc), 0.8f);

		}

	};

	listener->setSwallowTouches(true);

	//create sprites
	std::vector<HeroType> createdType;
	for (int i = 0; i < ROWS; ++i)
		for (int j = 0; j < COLUMS; ++j) {
			if (_layoutInfo.flags[i][j] == HeroType::NO_HERO) {
				Rect rect(j * CELL_WIDTH + 20, i * CELL_HEIGHT + 110,
					CELL_WIDTH, CELL_HEIGHT);
				_emptyCells.push_back(rect);
				continue;
			}

			if (std::find(createdType.begin(), createdType.end(),
				_layoutInfo.flags[i][j]) == createdType.end()) {
					createdType.push_back(_layoutInfo.flags[i][j]);

					Sprite *sprite = heros.at(_layoutInfo.flags[i][j]);

					sprite->setPosition(
						j * CELL_WIDTH + 20
						+ sprite->getContentSize().width / 2,
						i * CELL_HEIGHT + 110
						+ sprite->getContentSize().height / 2);

					_heros.pushBack(sprite);

					this->_eventDispatcher->addEventListenerWithSceneGraphPriority(
						listener->clone(), sprite);

					this->addChild(sprite, 10);
			}

		};


		LabelTTF *labelTime = LabelTTF::create();
		labelTime->setFontSize(30);
		labelTime->setTag(500);
		labelTime->setPosition(visiableSize.width * 5 / 6,visiableSize.height - 45);
		this->addChild(labelTime, 10);

		LabelTTF *labelStep = LabelTTF::create();
		labelStep->setTag(600);
		labelStep->setFontSize(30);
		labelStep->setPosition(visiableSize.width / 6 ,visiableSize.height - 45);
		this->addChild(labelStep, 10);

		String str;
		str.initWithFormat(GET_STR("mission"), _userData.index);
		LabelTTF *labelMissionNo = LabelTTF::create();
		labelMissionNo->setFontSize(30);
		labelMissionNo->setString(str.getCString());;
		labelMissionNo->setPosition(visiableSize.width / 2 ,visiableSize.height - 45);
		this->addChild(labelMissionNo, 10);

		//Add Menus
		MenuItemImage *selectItem =
			MenuItemImage::create("b1.png", "b1.png",
			[&](Ref *pRef) {
				PopLayer *popLayer = PopLayer::create("message.png");
				popLayer->setTitle("title.png");
				popLayer->setContentText(GET_STR("selectMission"), 20, 300);
				popLayer->addButton("ok.png", "ok.png", NULL, ButtonType::BT_SELECT_MISSSION_OK);
				popLayer->addButton("cancel.png", "cancel.png", NULL, ButtonType::BT_SELECT_MISSSION_CANCEL);
				popLayer->setCallbackFunc(this, (SEL_CallFuncN)&GameMainScene::popLayerCallFunc);
				this->addChild(popLayer, 50);
		});
		MenuItemImage *repeatItem =
			MenuItemImage::create("b2.png", "b2.png",
			[&](Ref *ref) {
				PopLayer *popLayer = PopLayer::create("message.png");
				popLayer->setTitle("title.png");
				popLayer->setContentText(GET_STR("resetGame"), 20, 300);
				popLayer->addButton("ok.png", "ok.png", NULL, ButtonType::BT_RESET_OK);
				popLayer->addButton("cancel.png", "cancel.png", NULL, ButtonType::BT_RESET_CANCEL);
				popLayer->setCallbackFunc(this, (SEL_CallFuncN)&GameMainScene::popLayerCallFunc);
				this->addChild(popLayer, 50);
		});

		MenuItemImage *cheatItem =
			MenuItemImage::create("b3.png", "b3.png",
			[&](Ref *ref) {
				if(GameManager::getInstance()->getCheatCount() <= 0)
				{
					PopLayer *popLayer = PopLayer::create("message.png");
					popLayer->setTitle("title.png");
					popLayer->setContentText(GET_STR("recharge"), 20, 300);
					popLayer->addButton("buy.png", "buy.png", NULL, ButtonType::BT_BUY_OK);
					popLayer->addButton("cancel.png", "cancel.png",NULL, ButtonType::BT_BUY_CANCEL);
					popLayer->setCallbackFunc(this, (SEL_CallFuncN)&GameMainScene::popLayerCallFunc);
					this->addChild(popLayer, 50);
				}
				else
				{
					String str;
					str.initWithFormat(GET_STR("cheatInfo"), GameManager::getInstance()->getCheatCount());
					PopLayer *popLayer = PopLayer::create("message.png");
					popLayer->setTitle("title.png");
					popLayer->setContentText(str.getCString());
					popLayer->addButton("used.png", "used.png", NULL, ButtonType::BT_CHEAT_OK);
					popLayer->addButton("no_used.png", "no_used.png",NULL, ButtonType::BT_CHEAT_CANCEL);
					popLayer->setCallbackFunc(this, (SEL_CallFuncN)&GameMainScene::popLayerCallFunc);
					this->addChild(popLayer, 50);

				}
		});

		MenuItemImage *shareItem =
			MenuItemImage::create("b4.png", "b4.png", [](Ref *pRef) {
				//Director::getInstance()->replaceScene(SelectMissionScene::createScene());

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
				JniMethodInfo mInfo;
				jobject jobj;

				bool isHave = JniHelper::getStaticMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "getInstance", "()Ljava/lang/Object;");
				if(!isHave)
				{
					log("jni->%s/getInstance:not Exsit", "org/cocos2dx/cpp/AppActivity");
				}
				else
				{
					log("jni->%s/getInstance: Exsit", "org/cocos2dx/cpp/AppActivity");
					jobj = mInfo.env->CallStaticObjectMethod(mInfo.classID, mInfo.methodID);
				}

				isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "payForCheat", "()V");
				if(!isHave)
				{
					log("jni->%s/payForCheat:not Exsit", "org/cocos2dx/cpp/AppActivity");
				}
				else
				{
					mInfo.env->CallVoidMethod(jobj, mInfo.methodID);
				}

#endif

		});

		Menu *menu = Menu::create(selectItem, repeatItem, cheatItem, shareItem, NULL);
		menu->alignItemsHorizontallyWithPadding(10);
		menu->setPosition(320, 40);
		this->addChild(menu, 10);

		Sprite *bg = Sprite::create("main.png");
		bg->setAnchorPoint(Vec2::ZERO);
		//bg->setPosition(Vec2(0, visiableSize.height - bg->getContentSize().height));
		this->addChild(bg, 0);

		//set Timer
		this->schedule(schedule_selector(GameMainScene::changeTime), 1.0f);

		return true;
}

GameMainScene * GameMainScene::create(int idx, bool isContinue) {
	GameMainScene *pRet = new GameMainScene();
	if (pRet && pRet->initWithMissionIdx(idx, isContinue)) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

//判断武将能朝哪个方向移动
CanMovedDirection GameMainScene::checkMoveDirection(Sprite *sprite) {
	//CCASSERT(sprite->getParent() == this, "Error: the sprite must be this layer's child");

	CanMovedDirection flag = CanMovedDirection::UNMOVEABLE;
	Rect emptyRect_1 = _emptyCells.at(0);
	Rect emptyRect_2 = _emptyCells.at(1);
	Rect rect = sprite->getBoundingBox();

	if (!rect.intersectsRect(emptyRect_1) && !rect.intersectsRect(emptyRect_1)) //如果精灵没有和任何一个空块相邻
		return flag;

	float emptyMidX_1 = emptyRect_1.getMidX();
	float emptyMidX_2 = emptyRect_2.getMidX();
	float emptyMidY_1 = emptyRect_1.getMidY();
	float emptyMidY_2 = emptyRect_2.getMidY();

	Vect emptyMidPoint_1 = Vect(emptyMidX_1, emptyMidY_1); //两个空块的中点
	Vect emptyMidPoint_2 = Vect(emptyMidX_2, emptyMidY_2);

	Rect leftZoom = Rect(rect.getMinX() - CELL_WIDTH, rect.getMinY(),
		rect.size.width, rect.size.height);
	Rect rightZoom = Rect(rect.getMinX() + CELL_WIDTH, rect.getMinY(),
		rect.size.width, rect.size.height);
	Rect topZoom = Rect(rect.getMinX(), rect.getMinY() + CELL_HEIGHT,
		rect.size.width, rect.size.height);
	Rect bottomZoom = Rect(rect.getMinX(), rect.getMinY() - CELL_HEIGHT,
		rect.size.width, rect.size.height);

	int tag = sprite->getTag();

	switch (tag) {
	case HeroType::CAO_CAO:
		if (leftZoom.containsPoint(emptyMidPoint_1)
			&& leftZoom.containsPoint(emptyMidPoint_2))
			flag = CanMovedDirection::MOVEABLE_LEFT;

		else if (rightZoom.containsPoint(emptyMidPoint_1)
			&& rightZoom.containsPoint(emptyMidPoint_2))
			flag = CanMovedDirection::MOVEABLE_RIGHT;

		else if (topZoom.containsPoint(emptyMidPoint_1)
			&& topZoom.containsPoint(emptyMidPoint_2))
			flag = CanMovedDirection::MOVEABLE_UP;

		else if (bottomZoom.containsPoint(emptyMidPoint_1)
			&& bottomZoom.containsPoint(emptyMidPoint_2))
			flag = CanMovedDirection::MOVEABLE_UP;
		break;

	case HeroType::GUAN_YU_H: //对于横排武将
	case HeroType::HUANG_ZHONG_H:
	case HeroType::ZHANG_FEI_H:
	case HeroType::ZHAO_YUN_H:
	case HeroType::MA_CHAO_H:
		if ((leftZoom.containsPoint(emptyMidPoint_1)
			|| leftZoom.containsPoint(emptyMidPoint_2)) //如果左边包含其中一个空快且右边一个都不包含
			&& !(rightZoom.containsPoint(emptyMidPoint_1)
			|| rightZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_LEFT;

		else if ((rightZoom.containsPoint(emptyMidPoint_1)
			|| rightZoom.containsPoint(emptyMidPoint_2)) //如果右边包含其中一个空快且左边一个都不包含
			&& !(leftZoom.containsPoint(emptyMidPoint_1)
			|| leftZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_RIGHT;

		else if ((leftZoom.containsPoint(emptyMidPoint_1)
			&& rightZoom.containsPoint(emptyMidPoint_2)) //如果左右各包含一个空快
			|| (leftZoom.containsPoint(emptyMidPoint_2)
			&& rightZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_LEFT_RIGHT;

		else if (topZoom.containsPoint(emptyMidPoint_1)
			&& topZoom.containsPoint(emptyMidPoint_2)) //如果上方包含两个空快
			flag = CanMovedDirection::MOVEABLE_UP;

		else if (bottomZoom.containsPoint(emptyMidPoint_1)
			&& bottomZoom.containsPoint(emptyMidPoint_2)) //如果下方方包含两个空快
			flag = CanMovedDirection::MOVEABLE_DOWN;
		break;

	case HeroType::GUAN_YU_V: //对于横排武将
	case HeroType::HUANG_ZHONG_V:
	case HeroType::ZHANG_FEI_V:
	case HeroType::ZHAO_YUN_V:
	case HeroType::MA_CHAO_V:
		if ((topZoom.containsPoint(emptyMidPoint_1)
			|| topZoom.containsPoint(emptyMidPoint_2)) //如果上方包含其中一个空快且右边一个都不包含
			&& !(bottomZoom.containsPoint(emptyMidPoint_1)
			|| bottomZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_UP;

		else if ((bottomZoom.containsPoint(emptyMidPoint_1)
			|| bottomZoom.containsPoint(emptyMidPoint_2)) //如果下方包含其中一个空快且上边一个都不包含
			&& !(topZoom.containsPoint(emptyMidPoint_1)
			|| topZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_DOWN;

		else if ((topZoom.containsPoint(emptyMidPoint_1)
			&& bottomZoom.containsPoint(emptyMidPoint_2)) //如果上下各包含一个空快
			|| (topZoom.containsPoint(emptyMidPoint_2)
			&& bottomZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_UP_DOWN;

		else if (leftZoom.containsPoint(emptyMidPoint_1)
			&& leftZoom.containsPoint(emptyMidPoint_2)) //如果左方包含两个空快
			flag = CanMovedDirection::MOVEABLE_LEFT;

		else if (rightZoom.containsPoint(emptyMidPoint_1)
			&& rightZoom.containsPoint(emptyMidPoint_2)) //如果右方包含两个空快
			flag = CanMovedDirection::MOVEABLE_RIGHT;
		break;

	case HeroType::XIAO_BING_1: //对于小兵
	case HeroType::XIAO_BING_2:
	case HeroType::XIAO_BING_3:
	case HeroType::XIAO_BING_4:
	case HeroType::XIAO_BING_5:
	case HeroType::XIAO_BING_6:
		if ((leftZoom.containsPoint(emptyMidPoint_1)
			&& rightZoom.containsPoint(emptyMidPoint_2)) || //左右各含一个空块
			(leftZoom.containsPoint(emptyMidPoint_2)
			&& rightZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_LEFT_RIGHT;

		else if ((topZoom.containsPoint(emptyMidPoint_1)
			&& bottomZoom.containsPoint(emptyMidPoint_2)) || //上下各含一个空块
			(topZoom.containsPoint(emptyMidPoint_2)
			&& bottomZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_UP_DOWN;

		else if ((leftZoom.containsPoint(emptyMidPoint_1)
			&& topZoom.containsPoint(emptyMidPoint_2)) || //左上各含一个空块
			(leftZoom.containsPoint(emptyMidPoint_2)
			&& topZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_LEFT_UP;

		else if ((leftZoom.containsPoint(emptyMidPoint_1)
			&& bottomZoom.containsPoint(emptyMidPoint_2)) || //左下各含一个空块
			(leftZoom.containsPoint(emptyMidPoint_2)
			&& bottomZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_LEFT_DOWN;

		else if ((rightZoom.containsPoint(emptyMidPoint_1)
			&& topZoom.containsPoint(emptyMidPoint_2)) || //右上各含一个空块
			(rightZoom.containsPoint(emptyMidPoint_2)
			&& topZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_RIGHT_UP;

		else if ((rightZoom.containsPoint(emptyMidPoint_1)
			&& bottomZoom.containsPoint(emptyMidPoint_2)) || //右下各含一个空块
			(rightZoom.containsPoint(emptyMidPoint_2)
			&& bottomZoom.containsPoint(emptyMidPoint_1)))
			flag = CanMovedDirection::MOVEABLE_RIGHT_DOWN;

		else if ((topZoom.containsPoint(emptyMidPoint_1)
			|| topZoom.containsPoint(emptyMidPoint_2)) //如果上方包含其中一个空快且右边一个都不包含
			&& !(bottomZoom.containsPoint(emptyMidPoint_1)
			|| bottomZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_UP;

		else if ((bottomZoom.containsPoint(emptyMidPoint_1)
			|| bottomZoom.containsPoint(emptyMidPoint_2)) //如果下方包含其中一个空快且上边一个都不包含
			&& !(topZoom.containsPoint(emptyMidPoint_1)
			|| topZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_DOWN;

		else if ((leftZoom.containsPoint(emptyMidPoint_1)
			|| leftZoom.containsPoint(emptyMidPoint_2)) //如果左边包含其中一个空快且右边一个都不包含
			&& !(rightZoom.containsPoint(emptyMidPoint_1)
			|| rightZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_LEFT;

		else if ((rightZoom.containsPoint(emptyMidPoint_1)
			|| rightZoom.containsPoint(emptyMidPoint_2)) //如果右边包含其中一个空快且左边一个都不包含
			&& !(leftZoom.containsPoint(emptyMidPoint_1)
			|| leftZoom.containsPoint(emptyMidPoint_2)))
			flag = CanMovedDirection::MOVEABLE_RIGHT;
		break;

	default:
		break;
	}

	return flag;

}

//刷新每个武将的位置信息
void GameMainScene::refreshLayoutInfo() {
	for (int i = 0; i < ROWS; ++i)
		for (int j = 0; j < COLUMS; ++j)
			_layoutInfo.flags[i][j] = HeroType::NO_HERO;

	//重新获取每个武将的位置信息
	for (auto i = _heros.begin(); i != _heros.end(); ++i) {
		Sprite *sprite = *i;

		HeroType type = static_cast<HeroType>(sprite->getTag());
		Rect rect = sprite->getBoundingBox();
		int x = static_cast<int>((rect.getMinX() - 18) / CELL_WIDTH);
		int y = static_cast<int>((rect.getMinY() - 108) / CELL_HEIGHT);

		_layoutInfo.flags[y][x] = type;

		if (rect.size.width > CELL_WIDTH * 1.5f
			&& rect.size.height > CELL_HEIGHT * 1.5f) {
				_layoutInfo.flags[y + 1][x] = type;
				_layoutInfo.flags[y][x + 1] = type;
				_layoutInfo.flags[y + 1][x + 1] = type;
				continue;
		}
		if (rect.size.width > CELL_WIDTH * 1.5f)
			_layoutInfo.flags[y][x + 1] = type;
		if (rect.size.height > CELL_HEIGHT * 1.5f)
			_layoutInfo.flags[y + 1][x] = type;
	}

	//更新空格子的位置
	_emptyCells.clear();
	for (int i = 0; i < ROWS; ++i)
		for (int j = 0; j < COLUMS; ++j) {
			if (_layoutInfo.flags[i][j] == HeroType::NO_HERO) {
				Rect rect(j * CELL_WIDTH + 20, i * CELL_HEIGHT + 110,
					CELL_WIDTH, CELL_HEIGHT);
				_emptyCells.push_back(rect);
			}
		}

}

//更新时间和步数
void GameMainScene::changeTime(float dt) {
	_userData.usedTime += 1;
	String str;
	int hour = _userData.usedTime / 3600;
	int min = (_userData.usedTime % 3600) / 60;
	int sec = _userData.usedTime - hour * 3600 - min * 60;

	str.initWithFormat("%2d:%2d:%2d", hour, min, sec);
	static_cast<LabelTTF *>(this->getChildByTag(500))->setString(str.getCString());

	str.initWithFormat(_strStep.c_str(), _userData.step);
	static_cast<LabelTTF *>(this->getChildByTag(600))->setString(str.getCString());

}

//使用葵花宝典
void GameMainScene::cheat(Ref *pRef) {

// 	_currentCheatStep = 0;
// 
// 	GameManager::getInstance()->getHeroLayoutInfoByIdx(_userData.index - 1,
// 		_layoutInfo); //恢复原始布局
// 	resetHerosPosition();
// 		
// 	Layer *shadeLayer = Layer::create();
// 	Rect rect = static_cast<Sprite *>(this->getChildByTag(
// 		_layoutInfo.solutions[_currentCheatStep].type))->getBoundingBox();
// 
// 	static_cast<Sprite *>(this->getChildByTag(
// 		_layoutInfo.solutions[_currentCheatStep].type))->runAction(RepeatForever::create(Blink::create(1.0f, 1)));	//闪烁
// 	this->addChild(shadeLayer, 11);

#if 0
	ClippingNode *clip = ClippingNode::create(); //设置裁剪节点
	clip->setInverted(false); //设置底板可见
	clip->setAlphaThreshold(0); //设置透明度Alpha值为0
	clip->setTag(800);
	this->addChild(clip, 10);

	auto shadeLayer = LayerColor::create(Color4B::WHITE);
	clip->addChild(shadeLayer); //在裁剪节点添加一个灰色的透明层
#endif
	

	//设置裁剪模板

	

#if 0
	Sprite * stencil =
		Sprite::createWithSpriteFrame(
		static_cast<Sprite *>(this->getChildByTag(
		_layoutInfo.solutions[_currentCheatStep].type))->getSpriteFrame());
	stencil->setPosition(
		static_cast<Sprite *>(this->getChildByTag(
		_layoutInfo.solutions[_currentCheatStep].type))->getPosition());
	clip->setStencil(stencil);
#endif

	//添加事件监听
// 	EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
// 	listener->onTouchBegan =
// 		[&](Touch *touch, Event *event) {
// 			//auto stencil = static_cast<Sprite *>( static_cast<ClippingNode *>(this->getChildByTag(800))->getStencil() );
// 			auto rect = static_cast<Sprite *>(this->getChildByTag(
// 				_layoutInfo.solutions[_currentCheatStep].type))->getBoundingBox();
// 			if(rect.containsPoint(touch->getLocation()) && _currentCheatStep < _layoutInfo.solutions.size())
// 			{
// 				auto hero = static_cast<Sprite *>(this->getChildByTag(_layoutInfo.solutions[_currentCheatStep].type));
// 				hero->stopAllActions();
// 				switch (_layoutInfo.solutions[_currentCheatStep].action) //判断应该向何处走
// 				{
// 				case CanMovedDirection::MOVEABLE_LEFT: //向左一步
// 					hero->setPosition(hero->getPositionX() - CELL_WIDTH, hero->getPositionY());
// 					break;
// 				case CanMovedDirection::MOVEABLE_RIGHT:
// 					hero->setPosition(hero->getPositionX() + CELL_WIDTH, hero->getPositionY());
// 					break;
// 				case CanMovedDirection::MOVEABLE_UP:
// 					hero->setPosition(hero->getPositionX(), hero->getPositionY() + CELL_HEIGHT);
// 					break;
// 				case CanMovedDirection::MOVEABLE_DOWN:
// 					hero->setPosition(hero->getPositionX(), hero->getPositionY() - CELL_HEIGHT);
// 					break;
// 				default:
// 					break;
// 				}
// 
// 				_currentCheatStep++;
// 				_userData.step++;
// 
// 				//判断是否胜利
// 				if(this->_winCell.containsPoint(this->getChildByTag(HeroType::CAO_CAO)->getPosition()) ||
// 					_currentCheatStep == _layoutInfo.minStep)
// 				{
// 					this->getChildByTag(HeroType::CAO_CAO)->stopAllActions();
// 					this->scheduleOnce(SEL_SCHEDULE(&GameMainScene::winFunc), 1.0f);
// 
// 					return true;
// 				}
// 								
// 				static_cast<Sprite *>(this->getChildByTag(
// 					_layoutInfo.solutions[_currentCheatStep].type))->runAction(RepeatForever::create(Blink::create(1.0f, 1)));	//闪烁
// 
// #if 0
// //重新设置模板
// 				hero = static_cast<Sprite *>(this->getChildByTag(_layoutInfo.solutions[_currentCheatStep].type));
// 				stencil = Sprite::createWithSpriteFrame(hero->getSpriteFrame());
// 				stencil->setPosition(hero->getPosition());
// 				static_cast<ClippingNode *>(this->getChildByTag(800))->setStencil(stencil);			
// 
// #endif
// 				
// 			}
// 			return true;
// 	};
// 	listener->setSwallowTouches(true);
// 	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,
// 		shadeLayer);
}

//重置武将的布局
void GameMainScene::resetHerosPosition() {

	auto heros = GameManager::getInstance()->getHeros();
	std::vector<HeroType> createdType;
	GameManager::getInstance()->getHeroLayoutInfoByIdx(_userData.index - 1,
		_layoutInfo);
	for (int i = 0; i < ROWS; ++i)
		for (int j = 0; j < COLUMS; ++j) {
			if (_layoutInfo.flags[i][j] == HeroType::NO_HERO) {
				Rect rect(j * CELL_WIDTH + 20, i * CELL_HEIGHT + 110,
					CELL_WIDTH, CELL_HEIGHT);
				_emptyCells.push_back(rect);
				continue;
			}

			if (std::find(createdType.begin(), createdType.end(),
				_layoutInfo.flags[i][j]) == createdType.end()) {
					createdType.push_back(_layoutInfo.flags[i][j]);

					Sprite *sprite = heros.at(_layoutInfo.flags[i][j]);

					sprite->setPosition(
						j * CELL_WIDTH + 20
						+ sprite->getContentSize().width / 2,
						i * CELL_HEIGHT + 110
						+ sprite->getContentSize().height / 2);

			}

		};
}

//胜利进行的动作
void GameMainScene::winFunc(float dt) {

	//移除遮罩层
	auto clip = this->getChildByTag(800);
	if (clip)
		clip->removeFromParent();

	//停止计时和步数
	this->unscheduleAllSelectors();

	String str;
	int hour = _userData.usedTime / 3600;
	int min = (_userData.usedTime % 3600) / 60;
	int sec = _userData.usedTime - hour * 3600 - min * 60;

	str.initWithFormat(GET_STR("complate"), _userData.step, hour, min, sec);
	PopLayer *popLayer = PopLayer::create("message.png");
	popLayer->setTitle(GET_STR("prompt"));
	popLayer->setContentText(str.getCString());
	popLayer->addButton("next.png", "next.png",NULL,
		ButtonType::BT_NEXT_MISSION_OK);
	popLayer->addButton("share.png", "share.png", NULL,
		ButtonType::BT_NEXT_MISSION_SHARE);
	popLayer->setCallbackFunc(this,
		(SEL_CallFuncN) &GameMainScene::popLayerCallFunc);
	this->addChild(popLayer, 50);

	//保存关卡完成度
	_userData.isDone = true;
	GameManager::getInstance()->saveMissionData(_userData);

	//改变下一关的解锁状态
	MissionInfo nextMision;
	nextMision.index = _userData.index + 1;
	nextMision.step = 0;
	nextMision.usedTime = 0;
	nextMision.isDone = false;
	nextMision.isLocked = false;
	GameManager::getInstance()->saveMissionData(nextMision);

}

void GameMainScene::popLayerCallFunc(Node *node) {
	MenuItem *item = dynamic_cast<MenuItem *>(node);
	if (item) {
		int tag = item->getTag();
		switch (tag) {
		case BT_CHEAT_OK: //OK
			cheat(NULL);
			GameManager::getInstance()->changeCheatCount(GameManager::getInstance()->getCheatCount() > 0 ? GameManager::getInstance()->getCheatCount() - 1 : 0);
			log("cheat count : %d", GameManager::getInstance()->getCheatCount());
			break;
		case BT_RESET_OK:
			resetHerosPosition();
			break;
		case BT_NEXT_MISSION_OK:
			{
				for(auto &child: _heros)
					child->removeFromParent();
				Director::getInstance()->pushScene(
					GameMainScene::createScene(_userData.index, false));
				break;
			}
		
		case BT_SELECT_MISSSION_OK:
			GameManager::getInstance()->SaveCurrentMission(_userData.index, _layoutInfo);
			Director::getInstance()->popToSceneStackLevel(2);
			break;
		case BT_BUY_OK:
		{
			log("Buy");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 

			JniMethodInfo mInfo;
			jobject jobj;

			bool isHave = JniHelper::getStaticMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "getInstance", "()Ljava/lang/Object;");
			if(!isHave)
			{
				log("jni->%s/getInstance:not Exsit", "org/cocos2dx/cpp/AppActivity");
			}
			else
			{
				log("jni->%s/getInstance: Exsit", "org/cocos2dx/cpp/AppActivity");
				jobj = mInfo.env->CallStaticObjectMethod(mInfo.classID, mInfo.methodID);
			}

			isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "payForCheat", "()V");
			if(!isHave)
			{
				log("jni->%s/payForCheat:not Exsit", "org/cocos2dx/cpp/AppActivity");
			}
			else
			{
				mInfo.env->CallVoidMethod(jobj, mInfo.methodID);
			}



#endif
		}

			break;
		default:
			break;
		}
	}
}


extern "C"{

	void Java_org_cocos2dx_cpp_AppActivity_showPayResult(int resultCode)
	{
		log("enter Java_org_cocos2dx_cpp_AppActivity_showPayResult");
		char str[30];
		sprintf(str, "resultCode: %d", resultCode);
		//MessageBox("str", "test");
		switch (resultCode) {
		case 101:
			MessageBox("Success", "title");
			break;
		case 103:
			MessageBox("cancel", "title");
			break;
		case 102:
			MessageBox("failed", "title");
			break;
		default:
			break;
		}
	}

}

