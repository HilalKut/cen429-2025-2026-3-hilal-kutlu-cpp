#ifndef DIGITALJOURNAL_H
#define DIGITALJOURNAL_H

#include <string>
#include <vector>
#include <ctime>
#include <memory>
#include "Models.h"
#include "DatabaseManager.h"
#include "CryptoUtils.h"

class DigitalJournalApp {
public:
    explicit DigitalJournalApp(const std::string& dbPath);
    ~DigitalJournalApp();
    void run();

    // --- TEST EDİLEBİLİR FONKSİYONLAR (PUBLIC YAPILDI) ---
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    void logoutUser();
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    bool isLoggedIn() const;
    bool createEntry(const std::string& title, const std::string& content, const std::string& mood);
    std::vector<Entry> viewAllEntries() const;
    std::vector<Entry> searchEntries(const std::string& keyword) const;
    std::vector<Entry> filterEntriesByDate(time_t startDate, time_t endDate) const;
    std::vector<Entry> filterEntriesByMood(const std::string& mood) const;
    void displayEntry(const Entry& entry) const;

private:
    User currentUser;
    bool loggedIn;
    std::unique_ptr<DatabaseManager> dbManager;

    // Yardımcı İç Fonksiyonlar
    std::string generateSalt();
    std::string hashPassword(const std::string& password, const std::string& salt);
    std::string encryptData(const std::string& data, const std::string& key) const;
    std::string decryptData(const std::string& encryptedData, const std::string& key) const;
    void secureClearString(std::string& s);
    void secureClearMemory(void* ptr, size_t size);

    // Arayüz Fonksiyonları
    void showMainMenu();
    void showLoginRegisterMenu();
    void handleLogin();
    void handleRegister();
    void handleCreateEntry();
    void handleViewEntries();
    void handleSearchEntries();
    void handleFilterEntriesByDate();
    void handleFilterEntriesByMood();
    void handleChangePassword();
    void handleViewMoodHistory();
};

#endif