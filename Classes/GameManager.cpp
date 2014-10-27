#include "GameManager.h"
#include "GameMacros.h"
#include <iostream>
#include "DAL/SqliteHelper.h"


static int count = sizeof(g_hero_infos) / sizeof(HeroInfo);

GameManager::GameManager(void):_heros(count)
{

}


GameManager::~GameManager(void)
{

}

GameManager * GameManager::getInstance()
{
	if(_manager == NULL)
	{
		_manager = new GameManager();
		_manager->init();
	}

	return _manager;
}

void GameManager::freeInstance()
{
	delete _manager;
	_manager = NULL;
}

bool GameManager::init()
{
	for(int i = 0; i < count; ++i)
	{
		Sprite *sprite = Sprite::create(g_hero_infos[i].img);
		sprite->setTag(g_hero_infos[i].hero);
		_heros.insert(g_hero_infos[i].hero, sprite);
	}

	if(MISSION_COUNT == 0)
		SET_MISSION_COUNT(43);

	loadMissionData();
	loadLayoutData();



	return true;
}


//读取配置文件内容
void GameManager::loadLayoutData()
{
	std::string str = FileUtils::getInstance()->getStringFromFile(MISSION_DATA_FILE);
	split(str, "\r\n");
}

//分割字符串
void GameManager::split(std::string &src, const char *deliem)
{
	std::string strtemp;

	std::string::size_type pos1, pos2;
	pos2 = src.find(deliem);
	pos1 = 0;       
	while (std::string::npos != pos2)
	{
		_layoutInfoStrs.push_back(src.substr(pos1, pos2 - pos1));
		pos1 = pos2 + 1;
		pos2 = src.find(deliem, pos1);
	}
	pos1++;
	_layoutInfoStrs.push_back(src.substr(pos1));
}

//根据索引加载对应关卡的默认布局信息
void GameManager::getHeroLayoutInfoByIdx(int idx, HeroLayoutInfo &info)
{

	HeroLayoutInfo tmp;
	std::string strTmp;
	CCASSERT(idx < _layoutInfoStrs.size(), "Sorry, This Mission Info is missing!");
	const char *str = _layoutInfoStrs[idx].c_str();
	for( int i = ROWS - 1; i >=0 ; --i)
		for(int j = 0; j < COLUMS; ++j)
		{
			strTmp.clear();

			while(!isdigit(*str))
				str++;

			while (isdigit(*str))
			{
				strTmp.append(1, *str++);
			}			
			tmp.flags[i][j] = static_cast<HeroType>(atoi(strTmp.c_str()));

		}

		//get min step
		while(!isdigit(*str))
			str++;
		strTmp.clear();
		while (isdigit(*str))
		{
			strTmp.append(1, *str++);
		}					
		tmp.minStep = static_cast<HeroType>(atoi(strTmp.c_str()));

		while(!isdigit(*str))
			str++;
		strTmp.clear();
		while(*str != '\0')
		{
			SolutionItem item;
			strTmp.clear();
			while(*str != ',')
				strTmp.append(1, *str++);
			str++;
			item.colum = atoi(strTmp.c_str());

			strTmp.clear();
			while(*str != '/')
				strTmp.append(1, *str++);
			str++;
			item.row = atoi(strTmp.c_str());

			strTmp.clear();
			while (*str != '|' && *str != '\0')
				strTmp.append(1, *str++);
			item.action = atoi(strTmp.c_str());

			tmp.solutions.push_back(item);
			str++;
		}


#if 0
		//加载解法
		while (*str != '\0')
		{
			strTmp.clear();
			SolutionItem item;
			while(!isdigit(*str))
				str++;

			while (isdigit(*str))
			{
				strTmp.append(1, *str++);
			}	
			item.type = static_cast<HeroType>(atoi(strTmp.c_str())); 

			strTmp.clear();
			while(!isdigit(*str))
				str++;
			while (isdigit(*str))
			{
				strTmp.append(1, *str++);
			}				
			item.action = static_cast<CanMovedDirection>(atoi(strTmp.c_str())); 

			tmp.solutions.push_back(item);
		}
		tmp.minStep = tmp.solutions.size();
#endif


		std::swap(tmp, info);
}

//加载关卡完成度信息
void GameManager::loadMissionData()
{
	sqlite3 *pDb = SqlHelper::openDB("MissionRecord");
	SqlHelper::createTable(pDb);
	SqlHelper::readRecord(pDb, _mision_status);
}

//保存关卡完成度信息
void GameManager::saveMissionData(MissionInfo info)
{	
	sqlite3 *pDb = SqlHelper::openDB("MissionRecord");
	if(SqlHelper::isExist(pDb, info.index))
		SqlHelper::updateRecord(pDb, info.index, info.step, info.usedTime, info.isDone, info.isLocked);
	else
		SqlHelper::insertRecord(pDb, info.index, info.step, info.usedTime, info.isDone, info.isLocked);
}

//保存当前关卡布局，供继续游戏使用
void GameManager::SaveCurrentMission(int idx, HeroLayoutInfo &layout)
{

	ValueMap valueMap;
	std::string value;

	for( int i = ROWS - 1; i >=0 ; --i)
		for(int j = 0; j < COLUMS; ++j)
		{			
			value.append(Value(layout.flags[i][j]).asString());
			value.append(1, ',');
		}
		Value val(value);
		Value val_idx(idx);
		valueMap["index"] = val_idx;
		valueMap["layout"] = val;

		UserDefault::getInstance()->setIntegerForKey("index", idx);
		UserDefault::getInstance()->setStringForKey("layout", value);
		/*	FileUtils::getInstance()->writeToFile(valueMap, MISSION_RECORD_PATH);*/
}

//加载上次保存的关卡布局
void GameManager::loadCurrentMission(HeroLayoutInfo & layout)
{
	// 	ValueMap valueMap = FileUtils::getInstance()->getValueMapFromFile(MISSION_RECORD_PATH);
	// 	int idx = valueMap.at("index").asInt();
	// 	auto str = valueMap["layout"].asString();
	int idx = UserDefault::getInstance()->getIntegerForKey("index");
	auto str = UserDefault::getInstance()->getStringForKey("layout");
	log("str: %s", str.c_str());
	std::string strTmp;

	int k = 0;
	for( int i = ROWS - 1; i >=0 ; --i)
		for(int j = 0; j < COLUMS; ++j)
		{
			strTmp.clear();

			while(!isdigit(str[k]))
				k++;

			while (isdigit(str[k]))
			{
				strTmp.append(1, str[k++]);
			}			
			layout.flags[i][j] = static_cast<HeroType>(atoi(strTmp.c_str()));
		}

}

void GameManager::changeCheatCount(int n)
{
	sqlite3 *pDb = SqlHelper::openDB("MissionRecord");
	SqlHelper::changeCheatCount(pDb, n);
}

int GameManager::getCheatCount()
{
	sqlite3 *pDb = SqlHelper::openDB("MissionRecord");
	return SqlHelper::getCheatCount(pDb);
}

bool GameManager::isFirstPlay()
{
	bool ret = UserDefault::getInstance()->getBoolForKey("isFirst", true);
	log("First: %s", ret ? "true" : "false");

	return ret;
}

void GameManager::setPlayed()
{
	UserDefault::getInstance()->setBoolForKey("isFirst", false);
	log("set played");
}

void GameManager::updateLayoutData(std::string filePath)
{
	log("Update Mission Layout File");
	ssize_t fileSize;
	unsigned char *data = FileUtils::getInstance()->getFileData(filePath, "r", &fileSize);
}



GameManager * GameManager::_manager = NULL;


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
	void Java_org_cocos2dx_cpp_AppActivity_updateMissionFile(JNIEnv *env, jobject obj, jstring string)
	{
		jboolean isCopy = '\0';
		const char *filePath = env->GetStringUTFChars(string, &isCopy);
		GameManager::getInstance()->updateLayoutData(filePath);
		env->ReleaseStringUTFChars(string, filePath);
	}

};
#endif
