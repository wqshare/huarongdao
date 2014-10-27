#include "NewGameManiScene.h"
#include "GameManager.h"
#include "Control/PopLayer.h"
#include "SelectMissionScene.h"
#include "Control/PopLayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 

#include "platform/android/jni/JniHelper.h"
#include "jni.h"

#endif


enum ButtonType {
	BT_CHEAT_OK = 1000,
	BT_CHEAT_CANCEL,
	BT_CHECK_CHEAT_OK,
	BT_CHECK_CHEAT_CANCEL,
	BT_RESET_OK,
	BT_RESET_CANCEL,
	BT_NEXT_MISSION_OK,
	BT_NEXT_MISSION_SHARE,
	BT_SELECT_MISSSION_OK,
	BT_SELECT_MISSSION_CANCEL,
	BT_BUY_OK,
	BT_BUY_CANCEL,
	BT_UNNOTICE,
	BT_INSTALL_OK
};

enum Epay{
	FEE_RESULT_SUCCESS = 101,
	FEE_RESULT_CANCELED,
	FEE_RESULT_FAILED
};



NewGameMainScene::NewGameMainScene():m__heros(0), m__layoutInfo(), m__strStep(GET_STR("step")),
	winRect(170, 100, 290, 290),_currentDirectStep(STEP1),isDirected(false)
{

}


NewGameMainScene::~NewGameMainScene()
{
	m__heros.clear();
}

bool NewGameMainScene::init()
{
	if(!Layer::init())
		return false;
	return true;
}

bool NewGameMainScene::initWithMissionIdx(int idx, bool isContinue)
{
	if(!Layer::init())
		return false;



	if(idx == MISSION_COUNT - 5)
	{
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

		isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "updateMission", "()V");
		if(!isHave)
		{
			log("jni->%s/updateMission:not Exsit", "org/cocos2dx/cpp/AppActivity");
		}
		else
		{
			mInfo.env->CallVoidMethod(jobj, mInfo.methodID);
		}
		GameManager::getInstance()->loadLayoutData();
#endif
	}

	isNotice = true;
	m__isContinue = isContinue;
	if (isContinue)
		GameManager::getInstance()->loadCurrentMission(m__layoutInfo);
	else
		GameManager::getInstance()->getHeroLayoutInfoByIdx(idx, m__layoutInfo);

	m__userData.index = idx + 1;
	m__userData.step = 0;
	m__userData.usedTime = 0;
	m__userData.isDone = false;
	m__userData.isLocked = false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();	

	Size size(640, 960);
	Size frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
	float scale_x = frameSize.width / size.width;
	float scale_y = frameSize.height / frameSize.height;


	this->setHerosPosition(false);
	this->addListener();

	LabelTTF *labelTime = LabelTTF::create();
	labelTime->setFontSize(30);
	labelTime->setTag(50);
	labelTime->setPosition(visibleSize.width * 5 / 6,visibleSize.height - 40);
	this->addChild(labelTime, 10);

	std::string tmp = StringUtils::format(m__strStep.c_str(), 0);
	LabelTTF *labelStep = LabelTTF::create();
	labelStep->setString(tmp);
	labelStep->setTag(60);
	labelStep->setFontSize(30);
	labelStep->setPosition(visibleSize.width / 6 ,visibleSize.height - 40);
	this->addChild(labelStep, 10);

	String str;
	str.initWithFormat(GET_STR("mission"), idx + 1);
	LabelTTF *labelMissionNo = LabelTTF::create();
	labelMissionNo->setFontSize(30);
	labelMissionNo->setString(str.getCString());;
	labelMissionNo->setPosition(visibleSize.width / 2 ,visibleSize.height - 40);
	this->addChild(labelMissionNo, 10);


	//Add Menus
	MenuItemImage *selectItem =
		MenuItemImage::create("b1.png", "b1.png",
		[&](Ref *pRef) {

			PopLayer *popLayer = PopLayer::create("message.png");
			popLayer->setTitle("title.png");
			popLayer->setContentText(GET_STR("selectMission"), 30,50, 250);
			popLayer->addButton("ok.png", "ok.png", NULL, ButtonType::BT_SELECT_MISSSION_OK);
			popLayer->addButton("cancel.png", "cancel.png", NULL, ButtonType::BT_SELECT_MISSSION_CANCEL);
			popLayer->setCallbackFunc(this, (SEL_CallFuncN)&NewGameMainScene::popLayerCallFunc);
			this->addChild(popLayer, 50);
	});
	MenuItemImage *repeatItem =
		MenuItemImage::create("b2.png", "b2.png",
		[&](Ref *ref) {

			PopLayer *popLayer = PopLayer::create("message.png");
			popLayer->setTitle("title.png");
			popLayer->setContentText(GET_STR("resetGame"), 30,150, 300);
			popLayer->addButton("ok.png", "ok.png", NULL, ButtonType::BT_RESET_OK);
			popLayer->addButton("cancel.png", "cancel.png", NULL, ButtonType::BT_RESET_CANCEL);
			popLayer->setCallbackFunc(this, (SEL_CallFuncN)&NewGameMainScene::popLayerCallFunc);
			this->addChild(popLayer, 50);
	});

	MenuItemImage *cheatItem =
		MenuItemImage::create("b3.png", "b3.png", CC_CALLBACK_1(NewGameMainScene::checkCheat, this));

	MenuItemImage *shareItem =
		MenuItemImage::create("b4.png", "b4.png", [&](Ref *pRef) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			JniMethodInfo mInfo;

			bool isHave = JniHelper::getStaticMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "share", "()V");
			if(!isHave)
			{
				log("jni->%s/share:not Exsit", "org/cocos2dx/cpp/AppActivity");
			}
			else
			{
				log("jni->%s/share: Exsit", "org/cocos2dx/cpp/AppActivity");
				mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
			}

#else
			MessageBox(GET_STR("share_msg"), GET_STR("prompt"));
#endif	

#if 0
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
#endif
	});


	//add background

	Sprite *bg = Sprite::create("main.png");
	bg->setAnchorPoint(Vec2(1,1));
	bg->setPosition(visibleSize.width, visibleSize.height);
	//bg->setPosition(Vec2(0, visiableSize.height - bg->getContentSize().height));
	bg->setTag(888);
	this->addChild(bg, 0);

	Sprite *bottomBg = Sprite::create("banner.png");
	bottomBg->setAnchorPoint(Vec2(1, 1));
	bottomBg->setPosition(visibleSize.width, visibleSize.height - bg->getContentSize().height);
	bottomBg->setScaleY((visibleSize.height - bg->getContentSize().height) / bottomBg->getContentSize().height);
	this->addChild(bottomBg, 0);

	Menu *menu = Menu::create(selectItem, repeatItem, cheatItem, shareItem, NULL);
	menu->alignItemsHorizontallyWithPadding(10);
	//menu->setPosition(320, 40);
	menu->setPosition(visibleSize.width / 2 , visibleSize.height - bg->getContentSize().height + 40);
	this->addChild(menu, 10);

	//add cheat count
	std::string strCheat = StringUtils::format("%d", GameManager::getInstance()->getCheatCount());
	Label *cheatLabel = Label::create(strCheat, "Consolas", 22);
	cheatLabel->setPosition(menu->getPosition().x + cheatItem->getContentSize().width , menu->getPosition().y + cheatItem->getContentSize().height / 2);
	cheatLabel->setColor(Color3B::WHITE);
	cheatLabel->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
	cheatLabel->enableShadow();
	cheatLabel->setTag(150);
	this->addChild(cheatLabel, 20);

	DrawNode *dn = DrawNode::create();
	dn->drawDot(cheatLabel->getPosition(), 18, Color4F(1, 0, 0, 0.5));
	this->addChild(dn, 15);

	//set Timer
	this->schedule(schedule_selector(NewGameMainScene::changeTime), 1.0f);

	if(idx == 0 /*&& GameManager::getInstance()->isFirstPlay()*/)
	{
		//GameManager::getInstance()->setPlayed();
		NewPlayerDirector(STEP1);
	}
		


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	postInfo();
	addAdBanner();
#endif

	return true;
}

void NewGameMainScene::checkCheat(Ref *ref)
{	

	if(GameManager::getInstance()->getCheatCount() <= 0)
	{
		PopLayer *popLayer = PopLayer::create("message.png");
		popLayer->setTitle("title.png");		
#if     (CONFIG_NO_EPAY == 0)
		popLayer->setContentText(GET_STR("recharge"),25,50, 170);
		popLayer->addButton("buy.png", "buy.png", NULL, ButtonType::BT_BUY_OK);
#else
		popLayer->setContentText(GET_STR("recharge_noEpay"),25,50, 200);
#endif
		popLayer->addButton("free_get.png", "free_get.png",NULL, ButtonType::BT_INSTALL_OK);
		popLayer->addButton("cancel.png", "cancel.png",NULL, ButtonType::BT_BUY_CANCEL);
		popLayer->setCallbackFunc(this, (SEL_CallFuncN)&NewGameMainScene::popLayerCallFunc);
		this->addChild(popLayer, 50);
	}
	else
	{
		String str;
		str.initWithFormat(GET_STR("cheatInfo"), GameManager::getInstance()->getCheatCount());
		PopLayer *popLayer = PopLayer::create("message.png");
		popLayer->setTitle("title.png");
		popLayer->setContentText(str.getCString(), 30,50, 250);
		popLayer->addButton("used.png", "used.png", NULL, ButtonType::BT_CHEAT_OK);
		popLayer->addButton("no_used.png", "no_used.png",NULL, ButtonType::BT_CHEAT_CANCEL);
		popLayer->setCallbackFunc(this, (SEL_CallFuncN)&NewGameMainScene::popLayerCallFunc);
		this->addChild(popLayer, 50);

	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void NewGameMainScene::postInfo()
{
	JniMethodInfo info;

	bool isHave = JniHelper::getStaticMethodInfo(info, "org/cocos2dx/cpp/AppActivity", "post", "()V");
	if(!isHave)
	{
		log("jni->%s/post:not Exsit", "org/cocos2dx/cpp/AppActivity");
	}
	else
	{
		log("jni->%s/post: Exsit", "org/cocos2dx/cpp/AppActivity");
		info.env->CallStaticVoidMethod(info.classID, info.methodID);
	}
}

void NewGameMainScene::addAdBanner()
{
	JniMethodInfo mInfo;
	jobject jobj;

	bool isHave = JniHelper::getStaticMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "getInstance", "()Ljava/lang/Object;");
	if(!isHave)
	{
		log("addBanner");
		log("jni->%s/getInstance:not Exsit", "org/cocos2dx/cpp/AppActivity");
	}
	else
	{
		log("jni->%s/getInstance: Exsit", "org/cocos2dx/cpp/AppActivity");
		jobj = mInfo.env->CallStaticObjectMethod(mInfo.classID, mInfo.methodID);
	}

	isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "addBanner", "()V");
	if(!isHave)
	{
		log("jni->%s/addBanner:not Exsit", "org/cocos2dx/cpp/AppActivity");
	}
	else
	{
		log("jni->%s/addBanner:Exsit", "org/cocos2dx/cpp/AppActivity");
		mInfo.env->CallVoidMethod(jobj, mInfo.methodID);
	}
}

#endif




NewGameMainScene * NewGameMainScene::create(int idx, bool isContinue)
{
	NewGameMainScene *pRet = new NewGameMainScene();
	if (pRet && pRet->initWithMissionIdx(idx, isContinue)) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = NULL;
		return pRet;
	}
}

void NewGameMainScene::addHero(const char *fileName, const char *name, int tag, int x, int y)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();	
	Size bgSize(640, 960);

	Sprite *sprite = Sprite::create(fileName);
	sprite->setAnchorPoint(Vec2::ZERO);
	sprite->setPosition((visibleSize.width - bgSize.width) + x * CELL_WIDTH + 20, visibleSize.height - bgSize.height + y * CELL_HEIGHT + 110);	

	sprite->setName(name);
	this->addChild(sprite, 10, tag);
	m__heros.pushBack(sprite);
}




Scene * NewGameMainScene::createScene(int idx, bool isContinue)
{
	auto scene = Scene::create();

	auto layer = NewGameMainScene::create(idx, isContinue);
	context = NULL;
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void NewGameMainScene::popLayerCallFunc(Node *node)
{
	MenuItem *item = dynamic_cast<MenuItem *>(node);
	if (item) {
		int tag = item->getTag();
		switch (tag) {
		case BT_CHEAT_OK: 
			GameManager::getInstance()->changeCheatCount(GameManager::getInstance()->getCheatCount() > 0 ? GameManager::getInstance()->getCheatCount() - 1 : 0);
			log("cheat count : %d", GameManager::getInstance()->getCheatCount());
			cheat();
			break;
		case BT_RESET_OK:			
			resetHerosPosition();
			break;

		case BT_CHECK_CHEAT_OK:
			checkCheat(NULL);
			break;
		case BT_NEXT_MISSION_SHARE:
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			JniMethodInfo mInfo;

			bool isHave = JniHelper::getStaticMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "share", "()V");
			if(!isHave)
			{
				log("jni->%s/share:not Exsit", "org/cocos2dx/cpp/AppActivity");
			}
			else
			{
				log("jni->%s/share: Exsit", "org/cocos2dx/cpp/AppActivity");
				mInfo.env->CallStaticVoidMethod(mInfo.classID, mInfo.methodID);
			}

#else
			MessageBox(GET_STR("share_msg"), GET_STR("prompt"));
#endif	
		}
			//no break
		case BT_NEXT_MISSION_OK:
			{
				for(auto &child: m__heros)
					child->removeFromParent();
				Director::getInstance()->replaceScene(
					NewGameMainScene::createScene(m__userData.index, false));
				break;
			}

		case BT_SELECT_MISSSION_OK:
			GameManager::getInstance()->SaveCurrentMission(m__userData.index, m__layoutInfo);

			if(!m__isContinue)
				Director::getInstance()->popToSceneStackLevel(2);
			else
				Director::getInstance()->replaceScene(SelectMissionScene::createScene());
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
		case BT_UNNOTICE:
			isNotice = false;
			break;
		case BT_INSTALL_OK:
			{
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

				isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "enterOffWall", "()V");
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
		default:
			break;

		}
	}

}





void NewGameMainScene::setHerosPosition(bool isReset)
{

	if(isReset)
	{
		for (auto &item: m__heros)
		{
			item->removeFromParent();
		}
	}

	if(!m__heros.empty())
		m__heros.clear();


	for(int i = 0; i < ROWS; i++)
	{
		for(int j = 0; j < COLUMS; j++)
		{
			switch (m__layoutInfo.flags[i][j])
			{
			case 0:
				break;
			case 1:
				if(this->getChildByName("caocao") == NULL)
				{
					addHero("caocao0.png", "caocao", HeroType::CAO_CAO, j, i);
				}		
				break;
			case 2:
				if(this->getChildByName("zhaoyun") == NULL)
				{
					if(j == COLUMS - 1 || m__layoutInfo.flags[i][j + 1] != 2)
					{
						addHero("zhaoyun1.png","zhaoyun", HeroType::ZHAO_YUN_V, j, i);
					}
					else
					{
						addHero("zhaoyun2.png","zhaoyun", HeroType::ZHAO_YUN_H, j, i);
					}
				}
				break;
			case 3:
				if(this->getChildByName("huangzhong") == NULL)
				{
					if(j == COLUMS - 1 || m__layoutInfo.flags[i][j + 1] != 3)
					{
						addHero("huangzhong1.png","huangzhong", HeroType::HUANG_ZHONG_V, j, i);
					}
					else
					{
						addHero("huangzhong2.png", "huangzhong", HeroType::HUANG_ZHONG_H, j, i);
					}
				}
				break;
			case 4:
				if(this->getChildByName("machao") == NULL)
				{
					if(j == COLUMS - 1 || m__layoutInfo.flags[i][j + 1] != 4)
					{
						addHero("machao1.png","machao", HeroType::MA_CHAO_V, j, i);
					}
					else
					{
						addHero("machao2.png","machao", HeroType::MA_CHAO_H, j, i);
					}
				}
				break;
			case 5:
				if(this->getChildByName("zhangfei") == NULL)
				{
					if(j == COLUMS - 1 || m__layoutInfo.flags[i][j + 1] != 5)
					{
						addHero("zhangfei1.png","zhangfei", HeroType::ZHANG_FEI_V, j, i);
					}
					else
					{
						addHero("zhangfei2.png","zhangfei", HeroType::ZHANG_FEI_H, j, i);
					}
				}
				break;
			case 6:
				if(this->getChildByName("guanyu") == NULL)
				{
					if(j == COLUMS - 1 || m__layoutInfo.flags[i][j + 1] != 6)
					{
						addHero("guanyu1.png","guanyu", HeroType::GUAN_YU_V, j, i);
					}
					else
					{
						addHero("guanyu2.png", "guanyu",HeroType::GUAN_YU_H, j, i);
					}
				}
				break;
			case 7:
				addHero("xiaobing0.png", "xiaobing", HeroType::XIAO_BING_1, j, i);
				break;
			case 8:
				addHero("xiaobing0.png", "xiaobing", HeroType::XIAO_BING_2, j, i);
				break;
			case 9:
				addHero("xiaobing0.png", "xiaobing", HeroType::XIAO_BING_3, j, i);
				break;
			case 10:
				addHero("xiaobing0.png", "xiaobing", HeroType::XIAO_BING_4, j, i);
				break;
			case 11:
				addHero("xiaobing0.png", "xiaobing", HeroType::XIAO_BING_5, j, i);
				break;
			case 12:
				addHero("xiaobing0.png", "xiaobing", HeroType::XIAO_BING_6, j, i);
				break;
			default:
				break;
			}
		}
	}
}


static Vec2 startPos;
static Vec2 originPos;
static int x, y;
static bool isTouched = false;
void NewGameMainScene::addListener()
{
	EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();


	listener->onTouchBegan = [&](Touch *touch, Event *event)
	{
		if(isTouched)
			return false;
		auto sprite = static_cast<Sprite *>(event->getCurrentTarget());

		auto rect = sprite->getBoundingBox();
		auto point = touch->getLocation();

		if(!rect.containsPoint(point))
		{
			return false;
		}
		isTouched = true;
		Size bgSize(640, 960);
		Size visibleSize = Director::getInstance()->getVisibleSize();

		startPos = sprite->getPosition();
		originPos = touch->getLocation();
		x = int((sprite->getBoundingBox().getMinX() - 18 - visibleSize.width + bgSize.width) / CELL_WIDTH); 
		y = int((sprite->getBoundingBox().getMinY() - 108 - visibleSize.height + bgSize.height) / CELL_HEIGHT); 
		return true;
	};

	listener->onTouchMoved = [&, this](Touch *touch, Event *event)
	{
		auto sprite = static_cast<Sprite *>(event->getCurrentTarget());

		HeroType tag = (HeroType)sprite->getTag();


		//log("originPoint: %f, %f ", originPos.x, originPos.y);
		auto offsetX = touch->getLocation().x - originPos.x;
		auto offsetY = touch->getLocation().y - originPos.y;
		//log("offsetX, Y: %f, %f ", offsetX, offsetY);

		// 		if( offsetX > CELL_WIDTH && offsetY > 0 )	//右上
		// 		{
		// 			switch (tag)
		// 			{			
		// 			case HeroType::XIAO_BING_1:
		// 				if(x >= COLUMS - 1)
		// 					break;
		// 				if(x <= 2 && y <= 3 && m__layoutInfo.flags[y][x+1] == HeroType::NO_HERO && m__layoutInfo.flags[y + 1][x + 1] == HeroType::NO_HERO)
		// 				{
		// 					sprite->setPosition(startPos.x + (ABS(offsetX) >  CELL_WIDTH ?  CELL_WIDTH : offsetX), startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
		// 				}		
		// 				break;
		// 			default:
		// 				break;
		// 			}
		// 		}
		// 
		// 		if( offsetX > CELL_WIDTH && offsetY < 0 )	//右下
		// 		{
		// 			switch (tag)
		// 			{			
		// 			case HeroType::XIAO_BING_1:
		// 				if(x >= COLUMS - 1)
		// 					break;
		// 				if(x <= 2 && y >= 1 && m__layoutInfo.flags[y][x+1] == HeroType::NO_HERO && m__layoutInfo.flags[y - 1][x + 1] == HeroType::NO_HERO)
		// 				{
		// 					sprite->setPosition(startPos.x + (ABS(offsetX) >  CELL_WIDTH ?  CELL_WIDTH : offsetX), startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
		// 				}		
		// 				break;
		// 			default:
		// 				break;
		// 			}
		// 		}
		// 
		// 		if( offsetX < 0 && offsetY > 0 )	//左上
		// 		{
		// 			switch (tag)
		// 			{			
		// 			case HeroType::XIAO_BING_1:
		// 				if(x >= COLUMS - 1)
		// 					break;
		// 				if(x >= 1 &&  y <= 3 && m__layoutInfo.flags[y][x-1] == HeroType::NO_HERO && m__layoutInfo.flags[y + 1][x - 1] == HeroType::NO_HERO)
		// 				{
		// 					sprite->setPosition(startPos.x - (ABS(offsetX) >  CELL_WIDTH ?  CELL_WIDTH : offsetX), startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
		// 				}		
		// 				break;
		// 			default:
		// 				break;
		// 			}
		// 		}
		// 
		// 		if( offsetX > 0 && offsetY > 0 )	//左下
		// 		{
		// 			switch (tag)
		// 			{			
		// 			case HeroType::XIAO_BING_1:
		// 				if(x >= COLUMS - 1)
		// 					break;
		// 				if(x >= 1 && y>= 1 && m__layoutInfo.flags[y][x - 1] == HeroType::NO_HERO && m__layoutInfo.flags[y - 1][x - 1] == HeroType::NO_HERO)
		// 				{
		// 					sprite->setPosition(startPos.x - (ABS(offsetX) >  CELL_WIDTH ?  CELL_WIDTH : offsetX), startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
		// 				}		
		// 				break;
		// 			default:
		// 				break;
		// 			}
		// 		}

#pragma region 右
		if( offsetX > 0 && ABS(offsetX) >= ABS(offsetY))	//右
		{
			switch (tag)
			{
			case HeroType::CAO_CAO:
				{
					if( x >= COLUMS - 2)
						break;
					if( m__layoutInfo.flags[y][x+2] == HeroType::NO_HERO && m__layoutInfo.flags[y+1][x+2] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x + (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y);						
					}					
				}
				break;
			case HeroType::HUANG_ZHONG_H:
			case HeroType::MA_CHAO_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::GUAN_YU_H:
				{
					if(x >= 2)
						break;
					if(x == 0 && m__layoutInfo.flags[y][x+2] == HeroType::NO_HERO && m__layoutInfo.flags[y][x + 3] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x + (offsetX > (2 * CELL_WIDTH) ? (2 * CELL_WIDTH) : offsetX), startPos.y);	
					}
					else if( m__layoutInfo.flags[y][x + 2] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x + (ABS(offsetX) > CELL_WIDTH ?  CELL_WIDTH : ABS(offsetX)), startPos.y);
					}
				}
				break;
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
			case HeroType::GUAN_YU_V:
				{
					if(x >= COLUMS - 1)
						break;
					if(m__layoutInfo.flags[y][x+1] == HeroType::NO_HERO && m__layoutInfo.flags[y+1][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x + (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y);
					}	
				}

				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				if(x >= COLUMS - 1)
					break;
				if(x <= 1 && m__layoutInfo.flags[y][x+1] == HeroType::NO_HERO && m__layoutInfo.flags[y][x + 2] == HeroType::NO_HERO)
				{
					sprite->setPosition(startPos.x + (offsetX > (2 * CELL_WIDTH) ? (2 * CELL_WIDTH) : offsetX), startPos.y);	
				}
				else if( m__layoutInfo.flags[y][x + 1] == HeroType::NO_HERO)
				{
					sprite->setPosition(startPos.x + (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y);
					if( y <= 3 && ABS(offsetX) >= CELL_WIDTH && offsetY > 0 && m__layoutInfo.flags[y+1][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
					}
					if( y >= 1 && ABS(offsetX) >= CELL_WIDTH && offsetY < 0 && m__layoutInfo.flags[y-1][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
					}
				}
				break;
			default:
				break;
			}
		}

#pragma endregion

#pragma region 左
		if(offsetX < 0 && ABS(offsetX) >= ABS(offsetY) )	//左
		{
			switch (tag)
			{
			case HeroType::CAO_CAO:
				{
					if(x == 0)
						break;
					if(m__layoutInfo.flags[y][x-1] == HeroType::NO_HERO && m__layoutInfo.flags[y+1][x-1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x - (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y);					
					}					
				}
				break;
			case HeroType::HUANG_ZHONG_H:
			case HeroType::MA_CHAO_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::GUAN_YU_H:
				{
					if(x == 0)
						break;
					if(x == 2 && m__layoutInfo.flags[y][x-1] == HeroType::NO_HERO && m__layoutInfo.flags[y][x - 2] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x - (ABS(offsetX) > (2 * CELL_WIDTH) ? (2 * CELL_WIDTH) : ABS(offsetX)), startPos.y);
					}
					else if( m__layoutInfo.flags[y][x - 1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x - (ABS(offsetX) > CELL_WIDTH ?  CELL_WIDTH : ABS(offsetX)), startPos.y);

					}
				}
				break;
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
			case HeroType::GUAN_YU_V:
				{
					if(x == 0)
						break;
					if(x >= 1 && y <= ROWS - 2 && m__layoutInfo.flags[y][x - 1] == HeroType::NO_HERO && m__layoutInfo.flags[y+1][x-1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x - (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y);

					}	
				}

				break;			
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				{
					if( x == 0)
						break;
					if(x >= 2 && m__layoutInfo.flags[y][x - 1] == HeroType::NO_HERO && m__layoutInfo.flags[y][x - 2] == HeroType::NO_HERO)
					{						
						sprite->setPosition(startPos.x - (ABS(offsetX) > (2 * CELL_WIDTH) ? (2 * CELL_WIDTH) : ABS(offsetX)), startPos.y);	
					}
					else if(m__layoutInfo.flags[y][x - 1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x - (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y);

						if( y <= 3 && ABS(offsetX) >= CELL_WIDTH && offsetY > 0 && m__layoutInfo.flags[y+1][x-1] == HeroType::NO_HERO)
						{
							sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
						}
						if( y >= 1 && ABS(offsetX) >= CELL_WIDTH && offsetY < 0 && m__layoutInfo.flags[y-1][x-1] == HeroType::NO_HERO)
						{
							sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));	
						}

					}
				}
				break;
			default:
				break;
			}
		}

#pragma endregion 

#pragma region 下
		if(offsetY < 0 && ABS(offsetX) < ABS(offsetY))	//下
		{
			switch (tag)
			{
			case HeroType::CAO_CAO:
				{
					if(y == 0)
						break;
					if( m__layoutInfo.flags[y - 1][x] == HeroType::NO_HERO && m__layoutInfo.flags[y-1][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
					}					
				}
				break;
			case HeroType::HUANG_ZHONG_H:
			case HeroType::MA_CHAO_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::GUAN_YU_H:
				{
					if(y == 0)
						break;
					if( m__layoutInfo.flags[y - 1][x] == HeroType::NO_HERO && m__layoutInfo.flags[y-1][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
					}					
				}
				break;
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
			case HeroType::GUAN_YU_V:
				{
					if( y == 0)
						break;
					if( y > 1 && m__layoutInfo.flags[y-1][x] == HeroType::NO_HERO && m__layoutInfo.flags[y-2][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y - (ABS(offsetY) > (2 * CELL_HEIGHT) ? (2 * CELL_HEIGHT) : ABS(offsetY)));
					}	
					else if( m__layoutInfo.flags[y-1][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
					}
				}

				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				{   
					if( y == 0)
						break;

					if( y > 1 && m__layoutInfo.flags[y-1][x] == HeroType::NO_HERO && m__layoutInfo.flags[y-2][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y - (ABS(offsetY) > (2 * CELL_HEIGHT) ? (2 * CELL_HEIGHT) : ABS(offsetY)));
					}	
					else if( m__layoutInfo.flags[y-1][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y - (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
						if( x < 3 && ABS(offsetY) >= CELL_WIDTH && offsetX > 0 && m__layoutInfo.flags[y-1][x+1] == HeroType::NO_HERO)
						{
							sprite->setPosition(startPos.x +  (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y - CELL_HEIGHT);	
						}
						if( x >= 1 && ABS(offsetY) >= CELL_WIDTH && offsetX < 0 && m__layoutInfo.flags[y-1][x-1] == HeroType::NO_HERO)
						{
							sprite->setPosition(startPos.x -  (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y - CELL_HEIGHT);	
						}
					}

				}
				break;
			default:
				break;
			}
		}

#pragma endregion 

#pragma region 上
		if(offsetY > 0 && ABS(offsetX) < ABS(offsetY))	//上
		{
			switch (tag)
			{
			case HeroType::CAO_CAO:
				{
					if(y >= 3)
						break;
					if( m__layoutInfo.flags[y + 2][x] == HeroType::NO_HERO && m__layoutInfo.flags[y + 2][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
					}					
				}
				break;
			case HeroType::HUANG_ZHONG_H:
			case HeroType::MA_CHAO_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::GUAN_YU_H:
				{
					if(y == ROWS - 1)
						break;
					if( m__layoutInfo.flags[y + 1][x] == HeroType::NO_HERO && m__layoutInfo.flags[y+1][x+1] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
					}					
				}
				break;
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
			case HeroType::GUAN_YU_V:
				{
					if( y >= ROWS - 2)
						break;
					if( y < 2 && m__layoutInfo.flags[y+2][x] == HeroType::NO_HERO && m__layoutInfo.flags[y+3][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y + (ABS(offsetY) > (2 * CELL_HEIGHT) ? (2 * CELL_HEIGHT) : ABS(offsetY)));
					}	
					else if( m__layoutInfo.flags[y+2][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));
					}
				}

				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				{
					if( y == 4)
						break;

					if( y < 3 && m__layoutInfo.flags[y+1][x] == HeroType::NO_HERO && m__layoutInfo.flags[y+2][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y + (ABS(offsetY) > (2 * CELL_HEIGHT) ? (2 * CELL_HEIGHT) : ABS(offsetY)));
					}	
					else if( m__layoutInfo.flags[y+1][x] == HeroType::NO_HERO)
					{
						sprite->setPosition(startPos.x , startPos.y + (ABS(offsetY) > CELL_HEIGHT ? CELL_HEIGHT : ABS(offsetY)));

						if( x < 3 && ABS(offsetY) >= CELL_HEIGHT && offsetX > 0 && m__layoutInfo.flags[y+1][x+1] == HeroType::NO_HERO)
						{
							sprite->setPosition(startPos.x +  (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y + CELL_HEIGHT);	
						}
						if( x >= 1 && ABS(offsetY) >= CELL_HEIGHT && offsetX < 0 && m__layoutInfo.flags[y+1][x-1] == HeroType::NO_HERO)
						{
							sprite->setPosition(startPos.x -  (ABS(offsetX) > CELL_WIDTH ? CELL_WIDTH : ABS(offsetX)), startPos.y + CELL_HEIGHT);	
						}
					}

				}
				break;
			default:
				break;
			}
		}

#pragma endregion 

	};

	listener->onTouchEnded = [&](Touch *touch, Event *event)
	{
		isTouched = false;
		auto sprite = static_cast<Sprite *>(event->getCurrentTarget());

		float offsetX = sprite->getPosition().x - startPos.x;
		float offsetY = sprite->getPosition().y - startPos.y;

#pragma region xxx

		do
		{
			if(ABS(offsetX) >= CELL_WIDTH && offsetY > 0 && offsetX > 0)	//右上
			{
				if(ABS(offsetY) > CELL_HEIGHT * 0.5f)
				{
					sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y + CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y + 1, x + 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y, x + 1);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}

			if(ABS(offsetX) >= CELL_WIDTH && offsetY < 0 && offsetX > 0)	//右下
			{
				if(ABS(offsetY) > CELL_HEIGHT * 0.5f)
				{
					sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y - CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y - 1, x + 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y, x + 1);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}

			if(ABS(offsetX) >= CELL_WIDTH && offsetY < 0 && offsetX < 0)	//左下
			{
				if(ABS(offsetY) > CELL_HEIGHT * 0.5f)
				{
					sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y - CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y - 1, x - 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y, x - 1);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}

			if(ABS(offsetX) >= CELL_WIDTH && offsetY > 0 && offsetX < 0)	//左上
			{
				if(ABS(offsetY) > CELL_HEIGHT * 0.5f)
				{
					sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y + CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y + 1, x - 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y, x - 1);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}


			if(ABS(offsetY) >= CELL_HEIGHT && offsetY > 0 && offsetX > 0)	//上右
			{
				if(ABS(offsetX) > CELL_WIDTH * 0.5f)
				{
					sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y + CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y + 1, x + 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x, startPos.y + CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y + 1, x);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}

			if(ABS(offsetY) >= CELL_HEIGHT && offsetY > 0 && offsetX < 0)	//上左
			{
				if(ABS(offsetX) > CELL_WIDTH * 0.5f)
				{
					sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y + CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y + 1, x - 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x, startPos.y + CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y + 1, x);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}

			if(ABS(offsetY) >= CELL_HEIGHT && offsetY < 0 && offsetX > 0)	//下右
			{
				if(ABS(offsetX) > CELL_WIDTH * 0.5f)
				{
					sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y - CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y - 1, x + 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x, startPos.y - CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y - 1, x);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}

			if(ABS(offsetY) >= CELL_HEIGHT && offsetY < 0 && offsetX < 0)	//下左
			{
				if(ABS(offsetX) > CELL_WIDTH * 0.5f)
				{
					sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y - CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y - 1, x - 1);
						break;
					default:
						break;
					}
				}
				else
				{
					sprite->setPosition(startPos.x, startPos.y - CELL_HEIGHT);
					switch (sprite->getTag())
					{
					case HeroType::XIAO_BING_1:
					case HeroType::XIAO_BING_2:
					case HeroType::XIAO_BING_3:
					case HeroType::XIAO_BING_4:
					case HeroType::XIAO_BING_5:
					case HeroType::XIAO_BING_6:
						swapCell(y, x, y - 1, x);
						break;
					default:
						break;
					}
				}

				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;
				break;

			}


			if(ABS(offsetX)>= CELL_WIDTH / 2 && ABS(offsetX) < CELL_WIDTH * 1.5f && offsetX > 0)
			{
				sprite->setPosition(startPos.x + CELL_WIDTH, startPos.y);		
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{
				case HeroType::CAO_CAO:
					swapCell(y, x, y, x + 2);
					swapCell(y + 1, x, y + 1, x + 2);
					break;
				case HeroType::HUANG_ZHONG_H:
				case HeroType::MA_CHAO_H:
				case HeroType::ZHANG_FEI_H:
				case HeroType::ZHAO_YUN_H:
				case HeroType::GUAN_YU_H:
					swapCell(y, x, y, x + 2);
					break;
				case HeroType::HUANG_ZHONG_V:
				case HeroType::MA_CHAO_V:
				case HeroType::ZHANG_FEI_V:
				case HeroType::ZHAO_YUN_V:
				case HeroType::GUAN_YU_V:
					swapCell(y, x, y, x + 1);
					swapCell(y + 1, x, y + 1, x + 1);
					break;
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y, x + 1);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetX)>= CELL_WIDTH / 2 && ABS(offsetX) < CELL_WIDTH * 1.5f &&  offsetX < 0)
			{
				sprite->setPosition(startPos.x - CELL_WIDTH, startPos.y);	
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{
				case HeroType::CAO_CAO:
					swapCell(y, x + 1, y, x - 1);
					swapCell(y + 1, x + 1, y + 1, x - 1);
					break;
				case HeroType::HUANG_ZHONG_H:
				case HeroType::MA_CHAO_H:
				case HeroType::ZHANG_FEI_H:
				case HeroType::ZHAO_YUN_H:
				case HeroType::GUAN_YU_H:
					swapCell(y, x + 1, y, x - 1);
					break;
				case HeroType::HUANG_ZHONG_V:
				case HeroType::MA_CHAO_V:
				case HeroType::ZHANG_FEI_V:
				case HeroType::ZHAO_YUN_V:
				case HeroType::GUAN_YU_V:
					swapCell(y, x, y, x - 1);
					swapCell(y + 1, x, y + 1, x - 1);
					break;
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y, x - 1);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetX) >= CELL_WIDTH * 1.5f &&  offsetX < 0)
			{
				sprite->setPosition(startPos.x - 2 * CELL_WIDTH, startPos.y);	
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{

				case HeroType::HUANG_ZHONG_H:
				case HeroType::MA_CHAO_H:
				case HeroType::ZHANG_FEI_H:
				case HeroType::ZHAO_YUN_H:
				case HeroType::GUAN_YU_H:
					swapCell(y, x + 1, y, x - 1);
					swapCell(y, x, y, x - 2);

					break;				
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y, x - 2);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetX) >= CELL_WIDTH * 1.5f &&  offsetX > 0)
			{
				sprite->setPosition(startPos.x + 2 * CELL_WIDTH, startPos.y);	
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{

				case HeroType::HUANG_ZHONG_H:
				case HeroType::MA_CHAO_H:
				case HeroType::ZHANG_FEI_H:
				case HeroType::ZHAO_YUN_H:
				case HeroType::GUAN_YU_H:
					swapCell(y, x, y, x + 2);
					swapCell(y, x + 1, y, x + 3);
					break;				
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y, x + 2);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetY) >= CELL_HEIGHT / 2 && ABS(offsetY) < CELL_HEIGHT * 1.5f && offsetY > 0)
			{
				sprite->setPosition(startPos.x , startPos.y + CELL_HEIGHT);	
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{
				case HeroType::CAO_CAO:
					swapCell(y, x,  y + 2, x);
					swapCell(y, x+1, y + 2, x + 1);
					break;
				case HeroType::HUANG_ZHONG_H:
				case HeroType::MA_CHAO_H:
				case HeroType::ZHANG_FEI_H:
				case HeroType::ZHAO_YUN_H:
				case HeroType::GUAN_YU_H:
					swapCell(y, x,  y + 1, x);
					swapCell(y, x + 1, y + 1, x + 1);
					break;
				case HeroType::HUANG_ZHONG_V:
				case HeroType::MA_CHAO_V:
				case HeroType::ZHANG_FEI_V:
				case HeroType::ZHAO_YUN_V:
				case HeroType::GUAN_YU_V:
					swapCell(y, x, y + 2, x);
					break;
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y + 1, x);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetY) >= CELL_HEIGHT / 2 && ABS(offsetY) < CELL_HEIGHT * 1.5f && offsetY < 0)
			{
				sprite->setPosition(startPos.x , startPos.y - CELL_HEIGHT);
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{
				case HeroType::CAO_CAO:
					swapCell(y + 1, x,  y - 1, x);
					swapCell(y + 1, x+1, y - 1, x + 1);
					break;
				case HeroType::HUANG_ZHONG_H:
				case HeroType::MA_CHAO_H:
				case HeroType::ZHANG_FEI_H:
				case HeroType::ZHAO_YUN_H:
				case HeroType::GUAN_YU_H:
					swapCell(y, x,  y -1, x);
					swapCell(y, x + 1, y - 1, x + 1);
					break;
				case HeroType::HUANG_ZHONG_V:
				case HeroType::MA_CHAO_V:
				case HeroType::ZHANG_FEI_V:
				case HeroType::ZHAO_YUN_V:
				case HeroType::GUAN_YU_V:
					swapCell(y + 1, x, y - 1, x);
					break;
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y - 1, x);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetY) >= CELL_HEIGHT * 1.5f && offsetY < 0)
			{
				sprite->setPosition(startPos.x , startPos.y - 2 * CELL_HEIGHT);
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{				
				case HeroType::HUANG_ZHONG_V:
				case HeroType::MA_CHAO_V:
				case HeroType::ZHANG_FEI_V:
				case HeroType::ZHAO_YUN_V:
				case HeroType::GUAN_YU_V:
					swapCell(y, x, y - 2, x);
					swapCell(y + 1, x, y - 1, x);
					break;
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y - 2, x);
					break;
				default:
					break;
				}
				break;
			}
			if(ABS(offsetY) >= CELL_HEIGHT * 1.5f && offsetY > 0)
			{
				sprite->setPosition(startPos.x , startPos.y + 2 * CELL_HEIGHT);
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("select.wav");
				m__userData.step++;

				switch (sprite->getTag())
				{				
				case HeroType::HUANG_ZHONG_V:
				case HeroType::MA_CHAO_V:
				case HeroType::ZHANG_FEI_V:
				case HeroType::ZHAO_YUN_V:
				case HeroType::GUAN_YU_V:
					swapCell(y, x, y + 2, x);
					swapCell(y + 1, x, y + 3, x);
					break;
				case HeroType::XIAO_BING_1:
				case HeroType::XIAO_BING_2:
				case HeroType::XIAO_BING_3:
				case HeroType::XIAO_BING_4:
				case HeroType::XIAO_BING_5:
				case HeroType::XIAO_BING_6:
					swapCell(y, x, y + 2, x);
					break;
				default:
					break;
				}
				break;
			}
			sprite->setPosition(startPos);
		}while (0);
#pragma endregion

#ifdef _DEBUG
		debug_show_layout();
#endif

		std::string str = StringUtils::format(m__strStep.c_str(), m__userData.step);
		static_cast<LabelTTF *>(this->getChildByTag(60))->setString(str.c_str());


		if(m__layoutInfo.flags[0][1] == HeroType::CAO_CAO && m__layoutInfo.flags[0][2] == HeroType::CAO_CAO)	//胜利条件
		{
			if(isDirected)
			{
				clip->removeFromParent();
				hand->removeFromParent();
			}
			this->scheduleOnce(SEL_SCHEDULE(&NewGameMainScene::winFunc), 0.5f);
			return;
		}

		if((m__userData.step == int(m__layoutInfo.minStep * 1.5) || 
			m__userData.step == int(m__layoutInfo.minStep * 2) || 
			m__userData.step == int(m__layoutInfo.minStep * 2.5)) && isNotice)
		{
			str = StringUtils::format(GET_STR("stepPrompt"), m__userData.step);
			PopLayer *popLayer = PopLayer::create("message.png");
			popLayer->setTitle("title.png");
			popLayer->setContentText(str.c_str(), 30,50, 250);
			popLayer->addButton("used.png", "used.png",NULL,
				ButtonType::BT_CHECK_CHEAT_OK);
			popLayer->addButton("no_used.png", "no_used.png", NULL,
				ButtonType::BT_CHECK_CHEAT_CANCEL);
			popLayer->addButton("un_notice.png", "un_notice.png", NULL, 
				ButtonType::BT_UNNOTICE);
			popLayer->setCallbackFunc(this,
				(SEL_CallFuncN) &NewGameMainScene::popLayerCallFunc);
			this->addChild(popLayer, 50);
		}

		if(isDirected && _directVec2 == sprite->getPosition())
		{
			_currentDirectStep += 1;
			NewPlayerDirector(DirectStep(_currentDirectStep));
		}

	};

	listener->setSwallowTouches(true);

	for (auto &item: m__heros)
	{
		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), item);
	}

	EventListenerKeyboard *keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event)
	{
		//log("key Press!");
		if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE )
		{
			GameManager::getInstance()->SaveCurrentMission(m__userData.index, m__layoutInfo);
			Director::getInstance()->popToRootScene();
		}

	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
}




void NewGameMainScene::swapCell(int x, int y, int x1, int y1)
{
	int tmp = m__layoutInfo.flags[x][y];
	m__layoutInfo.flags[x][y] = m__layoutInfo.flags[x1][y1];
	m__layoutInfo.flags[x1][y1] = (HeroType)tmp;
}



#ifdef _DEBUG
void NewGameMainScene::debug_show_layout()
{
	log("-------------------------------");
	for (int i = ROWS - 1; i >= 0 ; i--)
	{
		for (int j = 0; j < COLUMS; j++)
		{
			printf("%d  " , m__layoutInfo.flags[i][j]);
		}
		printf("\n");
	}
	printf("-------------------------------");

}
#endif // DEBUG

bool isChange = false;
//计时
void NewGameMainScene::changeTime(float dt) {
	m__userData.usedTime += 1;
	String str;
	int hour = m__userData.usedTime / 3600;
	int min = (m__userData.usedTime % 3600) / 60;
	int sec = m__userData.usedTime - hour * 3600 - min * 60;

	str.initWithFormat("%02d:%02d:%02d", hour, min, sec);
	static_cast<LabelTTF *>(this->getChildByTag(50))->setString(str.getCString());	

	if(isChange)
	{
		std::string strCheat = StringUtils::format("%d", GameManager::getInstance()->getCheatCount());
		static_cast<Label *>(this->getChildByTag(150))->setString(strCheat);
		isChange = false;
	}

}


//重置武将的布局
void NewGameMainScene::resetHerosPosition() 
{
	GameManager::getInstance()->getHeroLayoutInfoByIdx(m__userData.index - 1, m__layoutInfo);
	m__userData.usedTime = 0;
	m__userData.step = 0;
	this->setHerosPosition(true);	
	this->addListener();
}

void NewGameMainScene::winFunc(float dt)
{
	//移除遮罩层
	auto clip = this->getChildByTag(800);
	if (clip)
		clip->removeFromParent();

	//停止计时和步数
	this->unscheduleAllSelectors();

	String str;
	int hour = m__userData.usedTime / 3600;
	int min = (m__userData.usedTime % 3600) / 60;
	int sec = m__userData.usedTime - hour * 3600 - min * 60;

	str.initWithFormat(GET_STR("complate"), m__userData.step, hour, min, sec);
	PopLayer *popLayer = PopLayer::create("message.png");
	popLayer->setTitle("title.png");
	popLayer->setContentText(str.getCString(), 30,150, 200);
	popLayer->addButton("next.png", "next.png",NULL,
		ButtonType::BT_NEXT_MISSION_OK);
	popLayer->addButton("share.png", "share.png", NULL,
		ButtonType::BT_NEXT_MISSION_SHARE);
	popLayer->setCallbackFunc(this,
		(SEL_CallFuncN) &NewGameMainScene::popLayerCallFunc);
	this->addChild(popLayer, 50);

	//保存关卡完成度
	m__userData.isDone = true;
	GameManager::getInstance()->saveMissionData(m__userData);

	//改变下一关的解锁状态)
	MissionInfo nextMision =  GameManager::getInstance()->getMissionStatus()[m__userData.index];
	if(nextMision.isLocked)
	{
		nextMision.index = m__userData.index + 1;
		nextMision.step = 0;
		nextMision.usedTime = 0;
		nextMision.isDone = false;
		nextMision.isLocked = false;
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

		isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "postMissionIdx", "(I)V");
		if(!isHave)
		{
			log("jni->%s/postMissionIdx:not Exsit", "org/cocos2dx/cpp/AppActivity");
		}
		else
		{
			mInfo.env->CallIntMethod(jobj, mInfo.methodID, nextMision.index);
		}
#endif
	}

	GameManager::getInstance()->saveMissionData(nextMision);
}

void NewGameMainScene::cheat()
{
	isNotice = false;

	m__currentCheatStep = 0;
	resetHerosPosition();
	std::string strCheat = StringUtils::format("%d", GameManager::getInstance()->getCheatCount());
	static_cast<Label *>(this->getChildByTag(150))->setString(strCheat);

	Layer *shadeLayer = Layer::create();
	shadeLayer->setTag(800);

	m__sprite = findHero(ROWS - m__layoutInfo.solutions[m__currentCheatStep].row - 1, m__layoutInfo.solutions[m__currentCheatStep].colum);
	//m__sprite->runAction(RepeatForever::create(Blink::create(0.25, 1)));
	m__sprite->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.25f), FadeOut::create(0.25f), NULL)));

	EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch *touch, Event *event)
	{
		auto rect = m__sprite->getBoundingBox();
		if(!rect.containsPoint(touch->getLocation()))
		{
			return true;
		}

		int _x_ = m__layoutInfo.solutions[m__currentCheatStep].colum;
		int _y_ = ROWS - m__layoutInfo.solutions[m__currentCheatStep].row - 1;

		m__sprite->stopAllActions();
		m__sprite->runAction(FadeIn::create(0.25f));
		auto pos = m__sprite->getPosition();
		switch (m__layoutInfo.solutions[m__currentCheatStep].action)
		{
		case 0:				//上
			m__sprite->setPosition(pos.x, pos.y + CELL_HEIGHT);
			switch (m__sprite->getTag())
			{
			case HeroType::CAO_CAO:
				swapCellEx(_x_, _y_ - 1, _x_, _y_ + 1);
				swapCellEx(_x_ + 1, _y_ - 1, _x_ + 1, _y_ + 1);
				break;
			case HeroType::GUAN_YU_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::MA_CHAO_H:
			case HeroType::HUANG_ZHONG_H:
				swapCellEx(_x_, _y_, _x_, _y_ + 1);
				swapCellEx(_x_ + 1, _y_, _x_ + 1, _y_ + 1);
				break;
			case HeroType::GUAN_YU_V:
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
				swapCellEx(_x_, _y_ - 1, _x_, _y_ + 1);
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_, _y_ + 1);
				break;
			default:
				break;
			}
			break;
		case 1:				//下
			m__sprite->setPosition(pos.x, pos.y - CELL_HEIGHT);
			switch (m__sprite->getTag())
			{
			case HeroType::CAO_CAO:
				swapCellEx(_x_, _y_ , _x_, _y_ - 2);
				swapCellEx(_x_ + 1, _y_, _x_ + 1, _y_ - 2);
				break;
			case HeroType::GUAN_YU_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::MA_CHAO_H:
			case HeroType::HUANG_ZHONG_H:
				swapCellEx(_x_, _y_, _x_, _y_ - 1);
				swapCellEx(_x_ + 1, _y_, _x_ + 1, _y_ - 1);
				break;
			case HeroType::GUAN_YU_V:
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
				swapCellEx(_x_, _y_, _x_, _y_ - 2);
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_, _y_ - 1);
				break;
			default:
				break;
			}

			break;
		case 2:				//左
			m__sprite->setPosition(pos.x - CELL_WIDTH, pos.y);

			switch (m__sprite->getTag())
			{
			case HeroType::CAO_CAO:
				swapCellEx(_x_ + 1, _y_ , _x_ - 1, _y_);
				swapCellEx(_x_ + 1, _y_ - 1, _x_ - 1, _y_  - 1);
				break;
			case HeroType::GUAN_YU_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::MA_CHAO_H:
			case HeroType::HUANG_ZHONG_H:
				swapCellEx(_x_ - 1, _y_, _x_ + 1, _y_);
				break;
			case HeroType::GUAN_YU_V:
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
				swapCellEx(_x_, _y_, _x_ - 1, _y_ );
				swapCellEx(_x_, _y_ - 1, _x_ - 1, _y_ - 1);
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ - 1, _y_ );
				break;
			default:
				break;
			}
			break;
		case 3:				//右
			m__sprite->setPosition(pos.x + CELL_WIDTH, pos.y);

			switch (m__sprite->getTag())
			{
			case HeroType::CAO_CAO:
				swapCellEx(_x_ , _y_ , _x_ + 2, _y_);
				swapCellEx(_x_ , _y_ - 1, _x_ + 2, _y_  - 1);
				break;
			case HeroType::GUAN_YU_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::MA_CHAO_H:
			case HeroType::HUANG_ZHONG_H:
				swapCellEx(_x_ , _y_, _x_ + 2, _y_);
				break;
			case HeroType::GUAN_YU_V:
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
				swapCellEx(_x_, _y_, _x_ + 1, _y_ );
				swapCellEx(_x_, _y_ - 1, _x_ + 1, _y_ - 1);
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ + 1, _y_ );
				break;
			default:
				break;
			}
			break;
		case 4:				//上两步
			m__sprite->setPosition(pos.x, pos.y + 2 * CELL_HEIGHT);

			switch (m__sprite->getTag())
			{

			case HeroType::GUAN_YU_V:
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
				swapCellEx(_x_, _y_, _x_ , _y_ + 2 );
				swapCellEx(_x_, _y_ - 1, _x_, _y_ + 1);
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ , _y_ + 2);
				break;
			default:
				break;
			}
			break;
		case 5:				//下两步
			m__sprite->setPosition(pos.x, pos.y - 2 * CELL_HEIGHT);

			switch (m__sprite->getTag())
			{

			case HeroType::GUAN_YU_V:
			case HeroType::HUANG_ZHONG_V:
			case HeroType::MA_CHAO_V:
			case HeroType::ZHANG_FEI_V:
			case HeroType::ZHAO_YUN_V:
				swapCellEx(_x_, _y_, _x_ , _y_ - 2 );
				swapCellEx(_x_, _y_ - 1, _x_, _y_ - 3);
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ , _y_ - 2);
				break;
			default:
				break;
			}
			break;			
		case 6:				//左两步
			m__sprite->setPosition(pos.x - 2 * CELL_WIDTH, pos.y);

			switch (m__sprite->getTag())
			{

			case HeroType::GUAN_YU_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::MA_CHAO_H:
			case HeroType::HUANG_ZHONG_H:
				swapCellEx(_x_, _y_, _x_ - 2 , _y_ );
				swapCellEx(_x_ + 1, _y_, _x_ - 1 , _y_ );
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ - 2 , _y_);
				break;
			default:
				break;
			}
			break;
		case 7:				//右两步
			m__sprite->setPosition(pos.x + 2 * CELL_WIDTH, pos.y);
			switch (m__sprite->getTag())
			{

			case HeroType::GUAN_YU_H:
			case HeroType::ZHANG_FEI_H:
			case HeroType::ZHAO_YUN_H:
			case HeroType::MA_CHAO_H:
			case HeroType::HUANG_ZHONG_H:
				swapCellEx(_x_ , _y_, _x_ + 2 , _y_ ); 
				swapCellEx(_x_ + 1, _y_, _x_ + 3 , _y_ );
				break;
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ + 2 , _y_);
				break;
			default:
				break;
			}
			break;
		case 8:				//左上转弯
			m__sprite->setPosition(pos.x - CELL_WIDTH, pos.y + CELL_HEIGHT);
			switch (m__sprite->getTag())
			{			
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ - 1 , _y_ + 1);
				break;
			default:
				break;
			}

			break;
		case 9:				//左下
			m__sprite->setPosition(pos.x - CELL_WIDTH, pos.y - CELL_HEIGHT);
			switch (m__sprite->getTag())
			{			
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ - 1 , _y_ - 1);
				break;
			default:
				break;
			}
			break;
		case 10:
			m__sprite->setPosition(pos.x + CELL_WIDTH, pos.y + CELL_HEIGHT);
			switch (m__sprite->getTag())
			{			
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ + 1 , _y_ + 1);
				break;
			default:
				break;
			}
			break;
		case 11:
			m__sprite->setPosition(pos.x + CELL_WIDTH, pos.y - CELL_HEIGHT);
			switch (m__sprite->getTag())
			{			
			case HeroType::XIAO_BING_1:
			case HeroType::XIAO_BING_2:
			case HeroType::XIAO_BING_3:
			case HeroType::XIAO_BING_4:
			case HeroType::XIAO_BING_5:
			case HeroType::XIAO_BING_6:
				swapCellEx(_x_, _y_, _x_ + 1 , _y_ - 1);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

#ifdef _DEBUG
		debug_show_layout();
#endif
		m__currentCheatStep++;
		m__userData.step++;

		std::string str = StringUtils::format(m__strStep.c_str(), m__userData.step);
		static_cast<LabelTTF *>(this->getChildByTag(60))->setString(str.c_str());

		Size visibleSize = Director::getInstance()->getVisibleSize();
		// 		if(static_cast<Sprite *>(this->getChildByTag(CAO_CAO))->getPosition() < Vec2(visibleSize.width - 640 + CELL_WIDTH + 40, visibleSize.height - 960 + 130) && 
		// 			 Vec2(visibleSize.width - 640 + CELL_WIDTH + 10, visibleSize.height - 960 + 100) < static_cast<Sprite *>(this->getChildByTag(CAO_CAO))->getPosition())	//胜利条件
		// 		{
		// 			this->scheduleOnce(SEL_SCHEDULE(&NewGameMainScene::winFunc), 0.25f);
		// 			return true;
		// 		}

		auto caocao = static_cast<Sprite *>(this->getChildByTag(CAO_CAO));
		if(caocao->getBoundingBox().getMidX() > visibleSize.width - 640 + 2 * CELL_WIDTH + 10 && 
			caocao->getBoundingBox().getMidX() < visibleSize.width - 640 + 2 * CELL_WIDTH + 30 && 
			caocao->getBoundingBox().getMidY() > visibleSize.height - 960 + CELL_HEIGHT + 100 && 
			caocao->getBoundingBox().getMidY() < visibleSize.height - 960 + CELL_HEIGHT + 120 )	//胜利条件
		{
			this->scheduleOnce(SEL_SCHEDULE(&NewGameMainScene::winFunc), 0.25f);
			return true;
		}	

		m__sprite = findHero(ROWS - m__layoutInfo.solutions[m__currentCheatStep].row - 1, m__layoutInfo.solutions[m__currentCheatStep].colum);
		m__sprite->runAction(RepeatForever::create(Sequence::create(FadeIn::create(0.25f), FadeOut::create(0.25f), NULL)));
		return true;
	};
	listener->setSwallowTouches(true);

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, shadeLayer);
	//this->_eventDispatcher->addEventListenerWithFixedPriority(listener, -128);

	this->addChild(shadeLayer, 80);

}

Sprite * NewGameMainScene::findHero(int row, int col)
{	

	int tag = m__layoutInfo.flags[row][col];

	switch(tag)
	{
	case 1:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::CAO_CAO;}) ;
			if(idx != m__heros.end())
				return *idx;
		}

		break;
	case 2:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::ZHAO_YUN_H || 
				sprite->getTag() == HeroType::ZHAO_YUN_V;}) ;
			if(idx != m__heros.end())
				return *idx;
			break;
		}

	case 3:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::HUANG_ZHONG_H || 
				sprite->getTag() == HeroType::HUANG_ZHONG_V;}) ;
			if(idx != m__heros.end())
				return *idx;
			break;
		}

	case 4:
		{


			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::MA_CHAO_H || 
				sprite->getTag() == HeroType::MA_CHAO_V;}) ;
			if(idx != m__heros.end())
				return *idx;
			break;
		}
	case 5:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::ZHANG_FEI_H || 
				sprite->getTag() == HeroType::ZHANG_FEI_V;}) ;
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	case 6:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::GUAN_YU_H || 
				sprite->getTag() == HeroType::GUAN_YU_V;}) ;
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	case 7:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::XIAO_BING_1;});
			if(idx != m__heros.end())	
				return *idx;
		}
		break;
	case 8:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::XIAO_BING_2;});
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	case 9:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::XIAO_BING_3;});
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	case 10:	
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::XIAO_BING_4;});
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	case 11:	
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::XIAO_BING_5;});
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	case 12:
		{
			auto idx = std::find_if(m__heros.begin(), m__heros.end(), [](const Sprite *sprite)->bool{return sprite->getTag() == HeroType::XIAO_BING_6;});
			if(idx != m__heros.end())
				return *idx;
		}
		break;
	default:
		break;
	}


	return NULL;

}

void NewGameMainScene::swapCellEx(int x, int y, int x1, int y1)
{
	int tmp = m__layoutInfo.flags[y][x];
	m__layoutInfo.flags[y][x] = m__layoutInfo.flags[y1][x1];
	m__layoutInfo.flags[y1][x1] = (HeroType)tmp;
}

void NewGameMainScene::NewPlayerDirector(DirectStep step)
{
	if(!isDirected)
	{
		isNotice = false;
		isDirected = true;

		clip = ClippingNode::create(); //设置裁剪节点	
		clip->setInverted(true); //设置底板可见
		clip->setAlphaThreshold(0); //设置透明度Alpha值为0
		clip->setTag(800);
		this->addChild(clip, 10);

		shadeLayer = LayerColor::create(Color4B(0 , 0, 0, 150));
		clip->addChild(shadeLayer); //在裁剪节点添加一个灰色的透明层

		stencil = DrawNode::create();

		EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();

		listener->onTouchBegan = [&](Touch *touch, Event *event)
		{
			if(m__sprite->getBoundingBox().containsPoint(touch->getLocation()))
				return false;
			return true;
		};
		listener->setSwallowTouches(true);
		this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, shadeLayer);

		hand = Sprite::create("stencil/hand.png");
		hand->setAnchorPoint(Vec2(0, 0.5));
		this->addChild(hand, 10);

	}


	switch (step)
	{
	case STEP1:
		{
			EventListenerTouchOneByOne *listener = EventListenerTouchOneByOne::create();
			m__sprite = this->findHero(2,1);			


			Rect rect = m__sprite->getBoundingBox();
			Vec2 vec2s[] = 
			{
				Vec2(rect.getMinX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMinY() + 3 * CELL_HEIGHT ),
				Vec2(rect.getMinX(), rect.getMinY() + 3 * CELL_HEIGHT)
			};	//顶点一定要按逆时针从左下角开始顺序，不然绘制出现bug
			stencil->drawPolygon(vec2s, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));

			clip->setStencil(stencil);


			hand->setPosition(rect.getMidX(), rect.getMidY());			
			auto pos = hand->getPosition();
			hand->runAction(RepeatForever::create(Sequence::create(
				MoveBy::create(1.0f, Vec2(0, 2 * CELL_HEIGHT)),
				DelayTime::create(0.5f),
				Place::create(pos), NULL)));

			labelTip = Label::createWithSystemFont(GET_STR("step1"), GET_STR("fontName"), 30 );

			labelTip->enableShadow();
			
			labelTip->setPosition(m__sprite->getPosition() + Vec2(150, -150));
			shadeLayer->addChild(labelTip);

			_directVec2 = Vec2(m__sprite->getPosition().x, m__sprite->getPosition().y + 2 * CELL_HEIGHT);
		}		
		break;
	case STEP2:
		{
			log("Step 2");

			hand->stopAllActions();
			m__sprite = this->findHero(2,0);

			Rect rect = m__sprite->getBoundingBox();
			Vec2 vec2s[] = 
			{
				Vec2(rect.getMinX(), rect.getMinY()),
				Vec2(rect.getMaxX() + CELL_WIDTH, rect.getMinY()),
				Vec2(rect.getMaxX() + CELL_WIDTH, rect.getMaxY() ),
				Vec2(rect.getMinX(), rect.getMaxY()),
			};	//凹多边形要拆解成凸多边形来画，否则也有bug...fuck
			Vec2 vec2s_2[] = 
			{
				Vec2(rect.getMaxX(), rect.getMaxY()),
				Vec2(rect.getMaxX() + CELL_WIDTH, rect.getMaxY()),
				Vec2(rect.getMaxX() + CELL_WIDTH, rect.getMaxY() + CELL_HEIGHT ),
				Vec2(rect.getMaxX(), rect.getMaxY() + CELL_HEIGHT),
			};
			stencil->clear();
			stencil->drawPolygon(vec2s, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));
			stencil->drawPolygon(vec2s_2, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));

			hand->setPosition(rect.getMidX(), rect.getMidY());			

			auto pos = hand->getPosition();
			hand->runAction(RepeatForever::create(
				Sequence::create(MoveBy::create(0.5f, Vec2(CELL_WIDTH, 0)),
				MoveBy::create(0.5f, Vec2(0, CELL_HEIGHT)),
				DelayTime::create(0.5f),
				Place::create(pos), NULL)));			

			labelTip->setPosition(m__sprite->getPosition() + Vec2(150, -75));
			labelTip->setString(GET_STR("step2"));

			_directVec2 = Vec2(m__sprite->getPosition().x + CELL_WIDTH, m__sprite->getPosition().y + CELL_HEIGHT);
		}
		break;
	case STEP3:
		{
			log("Step 3");
			hand->stopAllActions();
			m__sprite = this->findHero(1,0);		

			Rect rect = m__sprite->getBoundingBox();
			Vec2 vec2s[] = 
			{
				Vec2(rect.getMinX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMaxY() + CELL_HEIGHT),
				Vec2(rect.getMinX(), rect.getMaxY() + CELL_HEIGHT),
			};	

			stencil->clear();
			stencil->drawPolygon(vec2s, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));

			hand->setPosition(rect.getMidX(), rect.getMidY());			

			auto pos = hand->getPosition();
			hand->runAction(RepeatForever::create(
				Sequence::create(MoveBy::create(0.5f, Vec2(0, CELL_HEIGHT)),
				DelayTime::create(0.5f),
				Place::create(pos), NULL)));


			labelTip->setPosition(m__sprite->getPosition() + Vec2(400, 225));
			labelTip->setString(GET_STR("step3"));

			_directVec2 = Vec2(m__sprite->getPosition().x, m__sprite->getPosition().y + CELL_HEIGHT);
		}
		break;
	case STEP4:
		log("Step 4");
		{
			hand->stopAllActions();
			m__sprite = this->findHero(0,1);		

			Rect rect = m__sprite->getBoundingBox();
			Vec2 vec2s[] = 
			{
				Vec2(rect.getMinX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMaxY() + CELL_HEIGHT ),
				Vec2(rect.getMinX(), rect.getMaxY() + CELL_HEIGHT),
			};	//凹多边形要拆解成凸多边形来画，否则也有bug...fuck
			Vec2 vec2s_2[] = 
			{
				Vec2(rect.getMinX() - CELL_WIDTH, rect.getMaxY()),
				Vec2(rect.getMinX(), rect.getMaxY()),
				Vec2(rect.getMinX() , rect.getMaxY() + CELL_HEIGHT ),
				Vec2(rect.getMinX() - CELL_WIDTH, rect.getMaxY() + CELL_HEIGHT),
			};

			stencil->clear();
			stencil->drawPolygon(vec2s, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));
			stencil->drawPolygon(vec2s_2, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));

			hand->setPosition(rect.getMidX(), rect.getMidY());			

			auto pos = hand->getPosition();
			hand->runAction(RepeatForever::create(
				Sequence::create(
				MoveBy::create(0.5f, Vec2(0, CELL_HEIGHT)),
				MoveBy::create(0.5f, Vec2(-CELL_WIDTH, 0)),				
				DelayTime::create(0.5f),
				Place::create(pos), NULL)));


			labelTip->setPosition(m__sprite->getPosition() + Vec2(150, 370));
			labelTip->setString(GET_STR("step4"));

			_directVec2 = Vec2(m__sprite->getPosition().x - CELL_WIDTH, m__sprite->getPosition().y + CELL_HEIGHT);
		}
		break;
	case STEP5:
		log("Step 5");
		{
			hand->stopAllActions();
			m__sprite = this->findHero(0,2);		

			Rect rect = m__sprite->getBoundingBox();
			Vec2 vec2s[] = 
			{
				Vec2(rect.getMinX() - CELL_WIDTH, rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMinY()),
				Vec2(rect.getMaxX(), rect.getMaxY()),
				Vec2(rect.getMinX() - CELL_WIDTH, rect.getMaxY()),
			};				

			stencil->clear();
			stencil->drawPolygon(vec2s, sizeof(vec2s) / sizeof(vec2s[0]), Color4F(0,0,0,1), 0, Color4F(0,0,0,1));

			hand->setPosition(rect.getMidX(), rect.getMidY());			

			auto pos = hand->getPosition();
			hand->runAction(RepeatForever::create(
				Sequence::create(
				MoveBy::create(0.5f, Vec2(-CELL_WIDTH, 0)),
				DelayTime::create(0.5f),
				Place::create(pos), NULL)));

			labelTip->setPosition(m__sprite->getPosition() + Vec2(0, 400));
			labelTip->setString(GET_STR("step5"));

		}
		break;	
	default:
		break;
	}	
}

NewGameMainScene * NewGameMainScene::context = NULL;


extern "C"{		

	void Java_org_cocos2dx_cpp_AppActivity_reCharge3()
	{
		GameManager::getInstance()->changeCheatCount(GameManager::getInstance()->getCheatCount() + 3);
		isChange = true;
	}

	void Java_org_cocos2dx_cpp_AppActivity_reCharge1()
	{
		GameManager::getInstance()->changeCheatCount(GameManager::getInstance()->getCheatCount() + 1);
		isChange = true;
	}
};
