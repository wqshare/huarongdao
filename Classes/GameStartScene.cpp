#include "GameStartScene.h"
#include "SelectMissionScene.h"
#include "GameMainScene.h"
#include "NewGameManiScene.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"
#include "jni.h"

#endif

USING_NS_CC;

#define MAIN_ITEM_LINE_SPACE 40

bool GameStartScene::init()
{
	if( !Layer::init() )
		return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Size winSize = Director::getInstance()->getWinSize();

	Sprite *bgSprite = Sprite::create("background.png");
	bgSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	bgSprite->setPosition(winSize.width / 2, winSize.height / 2);
	float scale_x = winSize.width / bgSprite->getContentSize().width;
	float scale_y = winSize.height / bgSprite->getContentSize().height;
	if(scale_x > scale_y)
		bgSprite->setScale(scale_x);
	else
		bgSprite->setScale(scale_y);


	MenuItemImage *startItem = MenuItemImage::create("main_button_1.png", "main_button_1.png", CC_CALLBACK_1(GameStartScene::startMenuCallback, this));
	MenuItemImage *continueItem = MenuItemImage::create("main_button_2.png", "main_button_2.png", "main_button_2_disable.png", CC_CALLBACK_1(GameStartScene::continueMenuCallback, this));
	MenuItemImage *shareItem = MenuItemImage::create("main_button_3.png", "main_button_3.png", CC_CALLBACK_1(GameStartScene::shareMenuCallback, this));


	startItem->setAnchorPoint(Vec2::ZERO);
	continueItem->setAnchorPoint(Vec2::ZERO);
	shareItem->setAnchorPoint(Vec2::ZERO);

	continueItem->setTag(500);

	startItem->setPosition(origin.x + 50, visibleSize.height / 2);
	continueItem->setPosition(startItem->getPosition().x, startItem->getPosition().y - MAIN_ITEM_LINE_SPACE - startItem->getContentSize().height);
	shareItem->setPosition(continueItem->getPosition().x, continueItem->getPosition().y - MAIN_ITEM_LINE_SPACE - startItem->getContentSize().height);		


	//item->setContentSize(Size(150, 50));


	Menu *menu = Menu::create(startItem, continueItem, shareItem, NULL);
	menu->setPosition(Vec2::ZERO);
	menu->setTag(499);

	this->addChild(bgSprite, 1);
	this->addChild(menu, 2);


	EventListenerKeyboard *keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event)
	{
		log("key Press!");
		if(keyCode == EventKeyboard::KeyCode::KEY_BACK || keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE )
		{
			Director::getInstance()->end();
		}

	};
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

// #ifdef _DEBUG
// 	this->schedule(schedule_selector(GameStartScene::debug_show_Coordinates), 10.0f);
// #endif // _DEBUG


	return true;
}

// #ifdef _DEBUG
// void debug_show_Coordinates(float dt)
// {
// 	log("visibleSize: %f, %f ", Director::getInstance()->getVisibleSize().width, Director::getInstance()->getVisibleSize().height);
// 	log("origin: %f, %f ", Director::getInstance()->getVisibleOrigin().x, Director::getInstance()->getVisibleOrigin().y);
// 	log("winSize: %f, %f ", Director::getInstance()->getWinSize().width, Director::getInstance()->getWinSize().height);
// 	log("frameSize: %f, %f ", Director::getInstance()->getOpenGLView()->getFrameSize().width, Director::getInstance()->getOpenGLView()->getFrameSize().width);
// }
// #endif // _DEBUG

cocos2d::Scene * GameStartScene::CreateScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameStartScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

void GameStartScene::startMenuCallback(cocos2d::Ref *pSender)
{	
	Director::getInstance()->pushScene(TransitionSlideInR::create(0.2f, SelectMissionScene::createScene()));
}

void GameStartScene::continueMenuCallback(cocos2d::Ref *pSender)
{
	int idx = UserDefault::getInstance()->getIntegerForKey("index");
	Director::getInstance()->pushScene(TransitionSlideInR::create(0.2f, NewGameMainScene::createScene(idx - 1, true)));
}

void GameStartScene::shareMenuCallback(cocos2d::Ref *pSender)
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

	isHave = JniHelper::getMethodInfo(mInfo, "org/cocos2dx/cpp/AppActivity", "showMoreGame", "()V");
	if(!isHave)
	{
		log("jni->%s/showMoreGame:not Exsit", "org/cocos2dx/cpp/AppActivity");
	}
	else
	{
		log("jni->%s/showMoreGame:Exsit", "org/cocos2dx/cpp/AppActivity");
		mInfo.env->CallVoidMethod(jobj, mInfo.methodID);
	}
	
#else
	MessageBox(GET_STR("share_msg"), GET_STR("prompt"));
#endif
}

void GameStartScene::onEnter()
{
	Layer::onEnter();
	if( UserDefault::getInstance()->getIntegerForKey("index", -1) == -1)
	{
		static_cast<MenuItemImage *>(static_cast<Menu *>(this->getChildByTag(499))->getChildByTag(500))->setEnabled(false);
	}
	else
	{
		static_cast<MenuItemImage *>(static_cast<Menu *>(this->getChildByTag(499))->getChildByTag(500))->setEnabled(true);
	}
}

