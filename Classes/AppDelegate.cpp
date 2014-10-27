#include "AppDelegate.h"
#include "Discard/HelloWorldScene.h"
#include "GameStartScene.h"
#include "GameManager.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
	
}

AppDelegate::~AppDelegate() 
{
//	GameManager::getInstance()->saveMissionData();
	GameManager::freeInstance();
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director	
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("Hua Rong Dao");
        director->setOpenGLView(glview);
    }

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	glview->setFrameSize(720, 1020);
#endif
	//director->setContentScaleFactor(1.0);
// 
	Size refrenceSize(640, 960);
	Size frameSize = glview->getFrameSize();

	float scale_x = frameSize.width / refrenceSize.width;
	float scale_y = frameSize.height / refrenceSize.height;

	if(scale_x < scale_y)
	{
		Size winSize = Size(refrenceSize.width , frameSize.height / scale_x);
		glview->setDesignResolutionSize(winSize.width, winSize.height, ResolutionPolicy::SHOW_ALL);

	}
	else
	{
		Size winSize = Size(frameSize.width / scale_y, refrenceSize.height);
		glview->setDesignResolutionSize(winSize.width, winSize.height, ResolutionPolicy::SHOW_ALL);

	}

		
    // turn on display FPS
   // director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = GameStartScene::CreateScene();

	//init GameManager
	GameManager::getInstance();

	//load effect
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("select.wav");
    // run
    director->runWithScene(scene);





    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

//	GameManager::getInstance()->saveMissionData();
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
