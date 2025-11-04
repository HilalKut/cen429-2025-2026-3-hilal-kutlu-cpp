#include "DatabaseManager.h"
#include "digitaljournal.h" // User ve Entry'nin tam tanımı için bu gereklidir!
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& path) : dbPath(path), db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open() {
    if (sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK) {
        initializeDatabase();
        return true;
    }
    std::cerr << "Veritabani acilamiyor: " << sqlite3_errmsg(db) << std::endl;
    return false;
}

void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

void DatabaseManager::initializeDatabase() {
    const char* createUserTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Users (
            Username TEXT PRIMARY KEY NOT NULL,
            PasswordHash TEXT NOT NULL,
            Salt TEXT NOT NULL
        );
    )";
    const char* createEntriesTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Entries (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Username TEXT NOT NULL,
            Title TEXT NOT NULL,
            EncryptedContent TEXT NOT NULL,
            Mood TEXT,
            Timestamp INTEGER NOT NULL,
            FOREIGN KEY(Username) REFERENCES Users(Username)
        );
    )";
    char* errMsg = nullptr;
    sqlite3_exec(db, createUserTableSQL, 0, 0, &errMsg);
    sqlite3_exec(db, createEntriesTableSQL, 0, 0, &errMsg);
    if (errMsg) {
        std::cerr << "Tablo olusturma hatasi: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

bool DatabaseManager::saveUser(const User& user) {
    std::string sql = "INSERT INTO Users (Username, PasswordHash, Salt) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr; // ÖNEMLİ: Pointer'ı null olarak ilklendir.
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, user.passwordHash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, user.salt.c_str(), -1, SQLITE_STATIC);
        
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Kullanici kaydedilemedi: " << sqlite3_errmsg(db) << std::endl;
        }
        
        sqlite3_finalize(stmt); // DÜZELTME: finalize() if bloğunun içine taşındı.
        return success;
    }
    return false;
}

bool DatabaseManager::loadUser(const std::string& username, User& user) {
    std::string sql = "SELECT PasswordHash, Salt FROM Users WHERE Username = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool found = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user.username = username;
            user.passwordHash = (const char*)sqlite3_column_text(stmt, 0);
            user.salt = (const char*)sqlite3_column_text(stmt, 1);
            found = true;
        }
        sqlite3_finalize(stmt); // DÜZELTME: finalize() if bloğunun içine taşındı.
    }
    return found;
}

bool DatabaseManager::updateUserPassword(const std::string& username, const std::string& newSalt, const std::string& newPasswordHash) {
    std::string sql = "UPDATE Users SET PasswordHash = ?, Salt = ? WHERE Username = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, newPasswordHash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, newSalt.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
        
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Parola guncellenemedi: " << sqlite3_errmsg(db) << std::endl;
        }
        
        sqlite3_finalize(stmt); // DÜZELTME: finalize() if bloğunun içine taşındı.
        return success;
    }
    return false;
}

bool DatabaseManager::saveEntry(const std::string& username, const Entry& entry) {
    std::string sql = "INSERT INTO Entries (Username, Title, EncryptedContent, Mood, Timestamp) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, entry.title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, entry.content.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, entry.mood.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 5, entry.timestamp);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Giris kaydedilemedi: " << sqlite3_errmsg(db) << std::endl;
        }

        sqlite3_finalize(stmt); // DÜZELTME: finalize() if bloğunun içine taşındı.
        return success;
    }
    return false;
}

std::vector<Entry> DatabaseManager::loadEntries(const std::string& username) {
    std::vector<Entry> entries;
    std::string sql = "SELECT Title, EncryptedContent, Mood, Timestamp FROM Entries WHERE Username = ? ORDER BY Timestamp DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Entry entry;
            entry.title = (const char*)sqlite3_column_text(stmt, 0);
            entry.content = (const char*)sqlite3_column_text(stmt, 1);
            entry.mood = (const char*)sqlite3_column_text(stmt, 2);
            entry.timestamp = sqlite3_column_int64(stmt, 3);
            entries.push_back(entry);
        }
        sqlite3_finalize(stmt); // DÜZELTME: finalize() if bloğunun içine taşındı.
    }
    return entries;
}