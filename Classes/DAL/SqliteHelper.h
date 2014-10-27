#ifndef SQLITE_HELP_H_
#define SQLITE_HELP_H_

#include "sqlite3.h"
#include "cocos2d.h"
#include "GameMacros.h"

class SqlHelper
{
public:
	static sqlite3 * openDB(std::string dbName);
	static void createTable(sqlite3 *pdb);
	static void insertRecord(sqlite3 *pdb, int idx, int step, long time, bool isDone, bool isLocked);
	static void updateRecord(sqlite3 *pdb, int idx, int step, long time, bool isDone, bool isLocked);
	static void readRecord(sqlite3 *pdb, std::vector<MissionInfo>& infos);
	static bool isExist(sqlite3 * pdb, int idx);
	static bool isTableExistedWithTableName(sqlite3 *pDb, std::string tableName);
	static void createCheatCount(sqlite3 *pdb);
	static void changeCheatCount(sqlite3 *pdb, int n);
	static int getCheatCount(sqlite3 *pdb);
};

#endif