#ifndef GAME_MACROS_H_
#define GAME_MACROS_H_

#include "cocos2d.h"

//Config
#define   CONFIG_NO_EPAY	1

//Macros Definition
#define ROWS 5
#define COLUMS 4
#define MISSION_COUNT 	cocos2d::UserDefault::getInstance()->getIntegerForKey("missionCount", 0)
#define SET_MISSION_COUNT(x)	do { cocos2d::UserDefault::getInstance()->setIntegerForKey("missionCount", x);}while(0)
#define MISSION_DATA_FILE  "layout_array.array"
#define MISSION_RECORD_PATH "mission_record.plist"
#define CN_STR_FILE	"Strings.plist"

#define CELL_WIDTH	150
#define CELL_HEIGHT 150

#define MAX(x, y)  ( (x) > (y) ? (x) : (y) )
#define MIN(x, y)  ( (x) < (y) ? (x) : (y) )
#define ABS(x)	   ( (x) >= 0 ? (x) : (0 - (x)) )

#define userDefault		UserDefault::getInstance()
#define GET_STR(key)	FileUtils::getInstance()->getValueMapFromFile(CN_STR_FILE).at(key).asString().c_str()

enum CanMovedDirection{
	MOVEABLE_LEFT = 1,	
	MOVEABLE_LEFT_RIGHT,	
	MOVEABLE_LEFT_UP,
	MOVEABLE_LEFT_DOWN,
	MOVEABLE_RIGHT,
	MOVEABLE_RIGHT_UP,
	MOVEABLE_RIGHT_DOWN,
	MOVEABLE_UP,
	MOVEABLE_UP_DOWN,
	MOVEABLE_DOWN,
	UNMOVEABLE
};

enum HeroType{
	NO_HERO = 0,
	CAO_CAO = 1,
	GUAN_YU_H,
	GUAN_YU_V,
	ZHAO_YUN_H,
	ZHAO_YUN_V,
	ZHANG_FEI_H,
	ZHANG_FEI_V,
	MA_CHAO_H,
	MA_CHAO_V,
	HUANG_ZHONG_H,
	HUANG_ZHONG_V,
	XIAO_BING_1,
	XIAO_BING_2,
	XIAO_BING_3,
	XIAO_BING_4,
	XIAO_BING_5,
	XIAO_BING_6,
};

enum ImgType
{
	IMG_V = 1,
	IMG_H,
	IMG_C,
	IMG_CAOCAO
};


typedef struct _HeroInfo{
	HeroType hero;
	const char *img;
}HeroInfo;


typedef struct _SolutionItem{
	int row;
	int colum;
	int action;
}SolutionItem;

typedef struct _HeroLayoutInfo{
	HeroType flags [ROWS][COLUMS];
	int minStep;
	std::vector<SolutionItem> solutions;
}HeroLayoutInfo;

typedef struct _MissionInfo{
	unsigned int index;
	int step ;
	long usedTime;
	bool isDone;
	bool isLocked;
}MissionInfo;


static HeroInfo g_hero_infos[] = {
	{ HeroType::CAO_CAO, "caocao0.png" },
	{ HeroType::GUAN_YU_H, "guanyu2.png"},
	{ HeroType::GUAN_YU_V, "guanyu1.png"},
	{ HeroType::HUANG_ZHONG_H, "huangzhong2.png"},
	{ HeroType::HUANG_ZHONG_V, "huangzhong1.png"},
	{ HeroType::MA_CHAO_H, "machao2.png"},
	{ HeroType::MA_CHAO_V, "machao1.png"},
	{ HeroType::ZHAO_YUN_H, "zhaoyun2.png"},
	{ HeroType::ZHAO_YUN_V, "zhaoyun1.png"},
	{ HeroType::ZHANG_FEI_H, "zhangfei2.png"},
	{ HeroType::ZHANG_FEI_V, "zhangfei1.png"},
	{ HeroType::XIAO_BING_1, "xiaobing0.png"},
	{ HeroType::XIAO_BING_2, "xiaobing0.png"},
	{ HeroType::XIAO_BING_3, "xiaobing0.png"},
	{ HeroType::XIAO_BING_4, "xiaobing0.png"},
	{ HeroType::XIAO_BING_5, "xiaobing0.png"},
	{ HeroType::XIAO_BING_6, "xiaobing0.png"},
};


/*static MissionInfo g_mission_items[MISSION_COUNT];*/



#endif // !GAME_MACROS_H_
