#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include "sqlite3.h"

// --- FORWARD DECLARATIONS ---
// digitaljournal.h dosyasını include etmek yerine,
// bu struct'ların var olduğunu derleyiciye bildiriyoruz.
struct User;
struct Entry;

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();
    bool open();
    void close();

    // Kullanıcı işlemleri
    bool saveUser(const User& user); // Referans olduğu için tam tanım gerekmez
    bool loadUser(const std::string& username, User& user); // Referans
    bool updateUserPassword(const std::string& username, const std::string& newSalt, const std::string& newPasswordHash);

    // Günlük işlemleri
    bool saveEntry(const std::string& username, const Entry& entry); // Referans
    std::vector<Entry> loadEntries(const std::string& username); // Tam tanım gerekir (aşağıya bakınız)

private:
    std::string dbPath;
    sqlite3* db;
    void initializeDatabase();
};

#endif // DATABASE_MANAGER_H