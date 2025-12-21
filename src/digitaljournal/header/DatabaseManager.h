/* src/digitaljournal/header/DatabaseManager.h */
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include "sqlite3.h"
#include "Models.h"

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();
    bool open();
    void close();
    bool saveUser(const User& user);
    bool loadUser(const std::string& username, User& user);
    bool updateUserPassword(const std::string& username, const std::string& newSalt, const std::string& newPasswordHash);
    bool saveEntry(const std::string& username, const Entry& entry);
    std::vector<Entry> loadEntries(const std::string& username);

private:
    std::string dbPath;
    sqlite3* db;
    void initializeDatabase();
};
#endif