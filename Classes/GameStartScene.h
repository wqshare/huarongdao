#ifndef GAME_START_SCENE_H_
#define GAME_START_SCENE_H_

#include "cocos2d.h"

class GameStartScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene * CreateScene();

	CREATE_FUNC(GameStartScene);
	virtual bool init();
private:
	void startMenuCallback(cocos2d::Ref *pSender);
	void continueMenuCallback(cocos2d::Ref *pSender);
	void shareMenuCallback(cocos2d::Ref *pSender);

	virtual void onEnter();
// #ifdef _DEBUG
// 	void debug_show_Coordinates(float dt);
// #endif // _DEBUG

};

#endif // !GAME_START_SCENE_H_
