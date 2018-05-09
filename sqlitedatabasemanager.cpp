#include "sqlitedatabasemanager.h"
#include "logger.hpp"

SQLiteDatabaseManager::SQLiteDatabaseManager(std::string name)
:
    mName(name)
{
    
}

SQLiteDatabaseManager::~SQLiteDatabaseManager()
{

}


// *** Public Methods *** //

bool SQLiteDatabaseManager::open()
{
    if(mDatabase != nullptr)
        close();
        
    return (sqlite3_open(mName.data(), &mDatabase) == SQLITE_OK) ? true : false;
}

bool SQLiteDatabaseManager::openDatabase(const std::string& name)
{
    mName = name;
    
    return open();
}


std::vector< std::vector< std::string > > SQLiteDatabaseManager::query(std::string query)
{
    std::vector<std::vector<std::string> > results;
    sqlite3_stmt *statement;

    if(sqlite3_prepare_v2(mDatabase, query.data(), -1, &statement, 0) == SQLITE_OK)
    {
        int columnCount = sqlite3_column_count(statement);
        
        while(true)
        {
            auto evaluation = sqlite3_step(statement);
                        
            if(evaluation == SQLITE_ROW)
            {
                std::vector<std::string> columns;
                                
                for(int columnIndex = 0; columnIndex < columnCount; columnIndex++)
                {
                    auto text = reinterpret_cast<const char*>(sqlite3_column_text(statement, columnIndex));
                    if(text != nullptr)
                        columns.emplace_back(text);
                    else
                        columns.emplace_back("");
                }
                
                results.push_back(columns);
            }
            else
                break;  
        }
        
        sqlite3_finalize(statement);
    }

    std::string error = sqlite3_errmsg(mDatabase);
    
    if(error != "not an error")
        LOG("Database query error: ", error);
    
    return results;
}

bool SQLiteDatabaseManager::close()
{
    if(sqlite3_close(mDatabase) == SQLITE_OK)
    {
        mDatabase = nullptr;
        return true;
    }
    else
        return false;
}

void SQLiteDatabaseManager::deleteTable(const std::string& name)
{    
    auto result = query("SELECT name FROM sqlite_master WHERE type='table' AND name='" + name + "';");

    if(result.size() > 0)
    {
        beginTransaction();
        query("DROP TABLE " + name + ";");
        commitTransaction();
    }
}

void SQLiteDatabaseManager::duplicateTable(const std::string& sourceTable, const std::string& destinationTable)
{
    auto result = query("SELECT sql fROM sqlite_master WHERE type = 'table' AND name = '" + sourceTable + "';");
    auto createStatement = result[0][0] + ";";
    auto pos = createStatement.find(sourceTable);
    createStatement.replace(pos, sourceTable.length(), destinationTable);
    query(createStatement);
    query("INSERT INTO " + destinationTable + " SELECT * FROM " + sourceTable + ";");
}

bool SQLiteDatabaseManager::beginTransaction()
{
    return (sqlite3_exec(mDatabase, "BEGIN TRANSACTION", NULL, NULL, NULL) == SQLITE_OK) ? true : false;
}

bool SQLiteDatabaseManager::commitTransaction()
{
    return (sqlite3_exec(mDatabase, "COMMIT TRANSACTION", NULL, NULL, NULL) == SQLITE_OK) ? true : false;
}
