#ifndef DIGITALJOURNAL_H
#define DIGITALJOURNAL_H

#include <string>
#include <vector>
#include <ctime>
#include <memory> // unique_ptr için
#include "DatabaseManager.h" // <-- EKLENMESİ GEREKEN EN ÖNEMLİ SATIR
#include "gtest/gtest.h"

// Forward declaration of DatabaseManager is not needed since we include the full header
// class DatabaseManager;

// Entry yapısı: Bir günlük girdisini temsil eder
struct Entry {
    std::string title;
    std::string content; // Bu, şifreli veya şifresiz olabilir
    std::string mood;
    time_t timestamp;
};

// User yapısı: Kullanıcı bilgilerini temsil eder
struct User {
    std::string username;
    std::string passwordHash;
    std::string salt;
};

// Dijital Günlük Uygulamasının ana sınıfı
class DigitalJournalApp {
        // --- DOST TEST SINIFLARI ---
    // Bu makrolar, aşağıdaki testlerin bu sınıfın private üyelerine
    // erişmesine izin verir.
    FRIEND_TEST(DigitalJournalAppTest, RegistrationSuccessAndFailure);
    FRIEND_TEST(DigitalJournalAppTest, LoginSuccessAndFailure);
    FRIEND_TEST(DigitalJournalAppTest, CreateAndRetrieveEntry);
    
public:
    explicit DigitalJournalApp(const std::string& dbPath);
    ~DigitalJournalApp();

    // Uygulamanın ana döngüsünü çalıştırır
    void run();

private:
    // --- Üye Değişkenler ---
    User currentUser;
    bool loggedIn;
    std::unique_ptr<DatabaseManager> dbManager; // Veritabanı yöneticisi

    // --- Kriptografi Fonksiyonları ---
    std::string generateSalt();
    std::string hashPassword(const std::string& password, const std::string& salt);
    std::string encryptData(const std::string& data, const std::string& key) const;
    std::string decryptData(const std::string& encryptedData, const std::string& key) const;

    // --- Bellek Güvenliği ---
    void secureClearString(std::string& s);
    void secureClearMemory(void* ptr, size_t size);

    // --- Ana İşlevsellik ---
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

    // --- Kullanıcı Arayüzü Yardımcıları ---
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

#endif // DIGITALJOURNAL_H