#include "SqliteHelper.h"

USING_NS_CC;


sqlite3* SqlHelper::openDB(std::string dbName)
{
	sqlite3 *pdb=NULL;  
	std::string path=cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath()+ dbName + ".db3";  
	std::string sql;  
	int result;  
	result=sqlite3_open(path.c_str(),&pdb);  
	if(result!=SQLITE_OK)  
	{
		CCLog("open database failed,  number%d",result);  
		return NULL;
	}
	return pdb;
}

void SqlHelper::createTable(sqlite3 *pdb)
{
	if(isTableExistedWithTableName(pdb, "MissionRecord"))
		return;
	std::string sql = "CREATE  TABLE  IF NOT EXISTS \"main\".\"MissionRecord\" (\"index\" INTEGER PRIMARY KEY NOT NULL, \"step\" INTEGER, \"used_time\" INTEGER, \"id_done\" INTEGER, \"is_locked\" INTEGER)";
	int result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	if(result != SQLITE_OK)
	{
		CCLog("open database failed,  number%d",result);
		return;
	}

	int count = MISSION_COUNT;
	for(int i = 0; i < count; ++i)
	{
		MissionInfo info;
		if( i == 0)
			info.isLocked = false;
		else
			info.isLocked = true;
		
		info.index = i + 1;
		info.isDone = false;
		info.step = 0;
		info.usedTime = 0;
		insertRecord(pdb, info.index, info.step, info.usedTime, info.isDone, info.isLocked);
	}

}

bool  SqlHelper::isTableExistedWithTableName(sqlite3 *pDb, std::string tableName)  
{  
	
	std::string dbExecs="";  
	dbExecs.append("select * from sqlite_master where \"type\"= 'table' and \"name\"='");  
	dbExecs.append(tableName);  
	dbExecs.append("'");  

	char **re;
	int col;
	int row;
	int result = sqlite3_get_table(pDb,dbExecs.c_str(), &re, &row, &col, NULL);
	if(row == 0)
		return false;
	return true;
}  

void SqlHelper::insertRecord(sqlite3 * pdb, int idx, int step, long time, bool isDone, bool isLocked)
{
	String sql;
	sql.initWithFormat( "INSERT INTO \"main\".\"MissionRecord\" (\"index\",\"step\",\"used_time\",\"id_done\",\"is_locked\") VALUES (%d, %d, %d, %d, %d) ", idx, step, time, isDone, isLocked );
	int result = sqlite3_exec(pdb, sql.getCString(), NULL, NULL, NULL);
	if( result != SQLITE_OK)
	{
		CCLog("insert database failed,  number%d",result);
	}
}

void SqlHelper::updateRecord(sqlite3 *pdb, int idx, int step, long time, bool isDone, bool isLocked)
{
	String sql;
	sql.initWithFormat("UPDATE \"main\".\"MissionRecord\" SET  \"step\" = %d,\"used_time\" = %d, \"id_done\" = %d, \"is_locked\" = %d WHERE \"index\" = %d", step, time, isDone, isLocked, idx);
	int result = sqlite3_exec(pdb, sql.getCString(), NULL, NULL, NULL);
	if( result != SQLITE_OK)
	{
		CCLog("update database failed,  number%d",result);
	}
}

void SqlHelper::readRecord(sqlite3 *pdb, std::vector<MissionInfo> & infos)
{	
	std::vector<MissionInfo> tmpInfo;
	char **re;
	int col;
	int row;
	int result = sqlite3_get_table(pdb, "select * from MissionRecord", &re, &row, &col, NULL);
	if(result != SQLITE_OK)
		return;

	for(int i = 1; i <= row; ++i)
	{
		MissionInfo tmp;
		tmp.index = atoi(re[col * i]);
		tmp.step = atoi(re[col * i + 1]);
		tmp.usedTime = atoi(re[col * i + 2]);
		tmp.isDone = static_cast<bool>(atoi(re[col * i + 3]));
		tmp.isLocked = static_cast<bool>(atoi(re[col * i + 4]));

		tmpInfo.push_back(tmp);
	}

	swap(tmpInfo, infos);
}

bool SqlHelper::isExist(sqlite3 *pDB, int idx)
{
	char **re;
	int col = 0;
	int row = 0;
	String sql;
	sql.initWithFormat("select * from MissionRecord where \"index\"=%d", idx);
	int result = sqlite3_get_table(pDB, sql.getCString(), &re, &row, &col, NULL);
	if(row == 0)
		return false;
	return true;
}

void SqlHelper::changeCheatCount(sqlite3 *pdb, int n)
{
	String sql;
	if(!isTableExistedWithTableName(pdb, "CheatCount"))
	{
		sql = "CREATE  TABLE  IF NOT EXISTS \"main\".\"CheatCount\" ( \"count\" INTEGER)";
		int result = sqlite3_exec(pdb, sql.getCString(), NULL, NULL, NULL);
		if(result != SQLITE_OK)
		{
			CCLog("open database failed,  number%d",result);
			return;
		}

		sql.initWithFormat( "INSERT INTO \"main\".\"CheatCount\" (\"count\") VALUES (%d)", n );
		result = sqlite3_exec(pdb, sql.getCString(), NULL, NULL, NULL);
		if(result != SQLITE_OK)
		{
			CCLog("insert cheat_count failed,  number%d",result);
			return;
		}
	}
	else
	{
		sql.initWithFormat("UPDATE \"main\".\"CheatCount\" SET \"count\" = %d ", n);
		int result = sqlite3_exec(pdb, sql.getCString(), NULL, NULL, NULL);
		if( result != SQLITE_OK)
		{
			CCLog("update database failed,  number%d",result);
		}
	}




}

int SqlHelper::getCheatCount(sqlite3 *pdb)
{
	if(!isTableExistedWithTableName(pdb, "CheatCount"))
#if _DEBUG
		return 100;
#else
		return 1;
#endif
	char **re;
	int col;
	int row;
	int result = sqlite3_get_table(pdb, "select * from CheatCount", &re, &row, &col, NULL);
	if(result != SQLITE_OK)
		return 0;

	int i = atoi(re[col]);
	return i;
}




