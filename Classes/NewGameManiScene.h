#ifndef NEW_GAME_MAIN_SCENE_H_
#define NEW_GAME_MAIN_SCENE_H_

#include "cocos2d.h"
#include "GameMacros.h"
USING_NS_CC;


enum DirectStep{
	STEP0 = 0,
	STEP1 = 1,
	STEP2, 
	STEP3, 
	STEP4, 
	STEP5, 
	STEP6, 
	STEP7, 
};

class NewGameMainScene : public Layer
{
public:
	NewGameMainScene();
	~NewGameMainScene();

	static Scene *createScene(int idx,  bool isContinue);	
	static NewGameMainScene *context;

	virtual bool init();
	virtual bool initWithMissionIdx(int idx, bool isContinue);

	CREATE_FUNC(NewGameMainScene);
	static NewGameMainScene * create(int idx, bool isContinue);


	void popLayerCallFunc(Node *);

private:
	void addHero(const char *fileName, const char *name,int tag, int x, int y);
	void setHerosPosition(bool isReset);
	void addListener();
	void swapCell(int x, int y, int x1, int y1);
	void swapCellEx(int x, int y, int x1, int y1);
	void checkCheat(Ref *ref);
	void changeTime(float dt);
	void resetHerosPosition();
	void winFunc(float dt);
	void cheat();
	void NewPlayerDirector(DirectStep step);

	
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	void addAdBanner();
	void postInfo();
#endif
	Sprite * findHero(int x, int y);

#ifdef _DEBUG
	void debug_show_layout();
#endif

private:
	Vector<Sprite *> m__heros;
	HeroLayoutInfo m__layoutInfo;
	MissionInfo m__userData;					//用户数据 （步数，时间等）		
	const std::string m__strStep;
	Sprite *m__sprite;
	Rect winRect;

	ClippingNode *clip;
	Layer *shadeLayer;
	Sprite *hand;
	DrawNode *stencil; 
	Label *labelTip;

	int _currentDirectStep;
	int m__currentCheatStep;					//开启秘籍时进行到第几步
	bool m__isContinue;
	bool isNotice;
	bool isDirected;
	Vec2 _directVec2;

};

#endif // !NEW_GAME_MAIN_SCENE_H_
