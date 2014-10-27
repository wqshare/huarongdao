#ifndef GAME_MAIN_SCENE_H_
#define GAME_MAIN_SCENE_H_

#include "cocos2d.h"
#include "GameMacros.h"
#include "data/UserData.h"

USING_NS_CC;

class GameMainScene : public Layer
{
public:

	GameMainScene();


	virtual bool init();
	virtual bool initWithMissionIdx(int idx, bool isContinue);

	CREATE_FUNC(GameMainScene);
	static GameMainScene * create(int idx, bool isContinue);

public:
	static Scene * createScene(int idx, bool isContinue);

private:
	CanMovedDirection checkMoveDirection(Sprite *sprite);
	void refreshLayoutInfo();
	void changeTime(float dt);
	void cheat(Ref *pRef);
	void resetHerosPosition();
	void winFunc(float dt);

	void popLayerCallFunc(Node *);

private:

	std::vector<Rect> _emptyCells;			//空格子的位置
	Vector<Sprite *> _heros;				//武将集合
	Rect _winCell;							//判断胜利的矩形区域
			
	Vec2 _beginPos;							//触摸开始点
	Vec2 _currentPos;						//被点击的武将的当前位置
		
	CanMovedDirection _currentDirection;	//被点击的武将当前可移动的方向
	HeroLayoutInfo _layoutInfo;				//武将布局数组
	
	MissionInfo _userData;					//用户数据 （步数，时间等）	

	int _currentCheatStep;					//开启秘籍时进行到第几步

	const std::string _strStep;
};

#endif // GAME_MAIN_SCENE_H_
