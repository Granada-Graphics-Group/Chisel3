#ifndef SQLITEDATABASEMANAGER_H
#define SQLITEDATABASEMANAGER_H

#include <sqlite3.h>
#include <vector>
#include <string>

class SQLiteDatabaseManager
{
public:
    SQLiteDatabaseManager(std::string name);
    ~SQLiteDatabaseManager();
    
    std::string name() const { return  mName; }
    sqlite3* database() const { return mDatabase; }
    bool isDatabaseOpen() const { return mDatabase != nullptr; }
    
    void setName(const std::string& name) { mName = name; }
    
    bool open();
    bool openDatabase(const std::string& name);    
    std::vector<std::vector<std::string>> query(std::string query);
    bool close();
    
    void deleteTable(const std::string& name);
    void duplicateTable(const std::string& sourceTable, const std::string& destinationTable);
    
    bool beginTransaction();
    bool commitTransaction();    

private:
    std::string mName;
    sqlite3* mDatabase = nullptr;    
};

#endif // SQLITEDATABASEMANAGER_H
