#include "digitaljournal.h"
#include "SecurityUtils.h"
#include "colors.h" // Renkleri dahil et
#include "utils/utils.h"

#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>
#include <memory>
#include <map>

// --- Arayüz Yardımcı Fonksiyonları (Bildirimler) ---
void clearScreen();
void printHeader(const std::string& title);
int getValidatedInput(int min, int max);


// --- Kriptografi Yardımcı Fonksiyonları ---

std::string _simple_sha256_placeholder(const std::string& input) {
    size_t hash = 0;
    for (char c : input) {
        hash = (hash * 31 + c);
    }
    return "hashed_" + std::to_string(hash);
}

// ... (Geri kalan tüm fonksiyonlarınız aynı kalacak, sadece tekrar eden blokları kaldıracağız) ...
// (Buraya daha önce verdiğim, tekrar eden blokları olmayan, tam ve doğru 
// digitaljournal.cpp dosyasının içeriğini yapıştırıyorum)

std::string DigitalJournalApp::generateSalt() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    std::string salt_str;
    for (int i = 0; i < 16; ++i) {
        salt_str += static_cast<char>(distrib(generator));
    }
    return salt_str;
}

std::string DigitalJournalApp::hashPassword(const std::string& password, const std::string& salt) {
    std::string saltedPassword = password + salt;
    return _simple_sha256_placeholder(saltedPassword);
}

std::string DigitalJournalApp::encryptData(const std::string& data, const std::string& key) const {
    std::string output = data;
    if (key.empty()) return output;
    for (size_t i = 0; i < data.size(); ++i) {
        output[i] = data[i] ^ key[i % key.size()];
    }
    return output;
}

std::string DigitalJournalApp::decryptData(const std::string& encryptedData, const std::string& key) const {
    std::string output = encryptedData;
    if (key.empty()) return output;
    for (size_t i = 0; i < encryptedData.size(); ++i) {
        output[i] = encryptedData[i] ^ key[i % key.size()];
    }
    return output;
}

void DigitalJournalApp::secureClearString(std::string& s) {
    std::fill(s.begin(), s.end(), 0);
    s.clear();
}

void DigitalJournalApp::secureClearMemory(void* ptr, size_t size) {
    if (ptr) {
        volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);
        while (size--) *p++ = 0;
    }
}

DigitalJournalApp::DigitalJournalApp(const std::string& dbPath) : loggedIn(false) {
    dbManager = std::make_unique<DatabaseManager>(dbPath);
    if (!dbManager->open()) {
        std::cerr << RED << "Kritik hata: Veritabani baslatilamadi." << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
}

DigitalJournalApp::~DigitalJournalApp() {
    if (loggedIn) {
        logoutUser();
    }
    secureClearString(currentUser.passwordHash);
    secureClearString(currentUser.salt);
    secureClearString(currentUser.username);
}

bool DigitalJournalApp::registerUser(const std::string& username, const std::string& password) {
    User tempUser;
    if (dbManager->loadUser(username, tempUser)) {
        std::cout << YELLOW << "Bu kullanici adi zaten mevcut." << RESET << std::endl;
        return false;
    }
    User newUser;
    newUser.username = username;
    newUser.salt = generateSalt();
    newUser.passwordHash = hashPassword(password, newUser.salt);
    if (dbManager->saveUser(newUser)) {
        std::cout << GREEN << "Kayit basarili. Lutfen giris yapin." << RESET << std::endl;
        secureClearString(newUser.passwordHash);
        secureClearString(newUser.salt);
        return true;
    }
    return false;
}

bool DigitalJournalApp::loginUser(const std::string& username, const std::string& password) {
    User tempUser;
    if (!dbManager->loadUser(username, tempUser)) {
        std::cout << RED << "Kullanici adi veya parola hatali." << RESET << std::endl;
        return false;
    }
    std::string hashedPasswordAttempt = hashPassword(password, tempUser.salt);
    if (hashedPasswordAttempt == tempUser.passwordHash) {
        loggedIn = true;
        currentUser = tempUser;
        std::vector<char> msg = {
            static_cast<char>('G' ^ 0xAA), static_cast<char>('i' ^ 0xAA), static_cast<char>('r' ^ 0xAA),
            static_cast<char>('i' ^ 0xAA), static_cast<char>('s' ^ 0xAA), static_cast<char>(' ' ^ 0xAA),
            static_cast<char>('b' ^ 0xAA), static_cast<char>('a' ^ 0xAA), static_cast<char>('s' ^ 0xAA),
            static_cast<char>('a' ^ 0xAA), static_cast<char>('r' ^ 0xAA), static_cast<char>('i' ^ 0xAA),
            static_cast<char>('l' ^ 0xAA), static_cast<char>('i' ^ 0xAA), static_cast<char>('.' ^ 0xAA)
        };
        std::cout << GREEN << SecurityUtils::getObfuscatedString(msg) << " Hos geldiniz, " << currentUser.username << "!" << RESET << std::endl;
        secureClearString(hashedPasswordAttempt);
        return true;
    } else {
        std::cout << RED << "Kullanici adi veya parola hatali." << RESET << std::endl;
        secureClearString(hashedPasswordAttempt);
        return false;
    }
}

bool DigitalJournalApp::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (!loggedIn) return false;
    std::string oldPasswordHashAttempt = hashPassword(oldPassword, currentUser.salt);
    if (oldPasswordHashAttempt != currentUser.passwordHash) {
        std::cout << RED << "Mevcut parolaniz hatali." << RESET << std::endl;
        secureClearString(oldPasswordHashAttempt);
        return false;
    }
    secureClearString(oldPasswordHashAttempt);
    std::string newSalt = generateSalt();
    std::string newPasswordHash = hashPassword(newPassword, newSalt);
    if (dbManager->updateUserPassword(currentUser.username, newSalt, newPasswordHash)) {
        currentUser.salt = newSalt;
        currentUser.passwordHash = newPasswordHash;
        std::cout << GREEN << "Parola basariyla degistirildi." << RESET << std::endl;
        secureClearString(newPasswordHash);
        secureClearString(newSalt);
        return true;
    } else {
        std::cout << RED << "Parola degistirilirken bir hata olustu." << RESET << std::endl;
        secureClearString(newPasswordHash);
        secureClearString(newSalt);
        return false;
    }
}

bool DigitalJournalApp::createEntry(const std::string& title, const std::string& content, const std::string& mood) {
    if (!loggedIn) return false;
    Entry newEntry;
    newEntry.title = title;
    newEntry.content = encryptData(content, currentUser.passwordHash);
    newEntry.mood = mood;
    newEntry.timestamp = time(nullptr);
    if (dbManager->saveEntry(currentUser.username, newEntry)) {
        std::cout << GREEN << "Gunluk girisi basariyla olusturuldu." << RESET << std::endl;
        return true;
    }
    return false;
}

std::vector<Entry> DigitalJournalApp::viewAllEntries() const {
    if (!loggedIn) return {};
    std::vector<Entry> entries = dbManager->loadEntries(currentUser.username);
    for (auto& entry : entries) {
        entry.content = decryptData(entry.content, currentUser.passwordHash);
    }
    return entries;
}

void DigitalJournalApp::logoutUser() {
    if (loggedIn) {
        std::cout << YELLOW << "Cikis yapiliyor..." << RESET << std::endl;
        loggedIn = false;
        secureClearString(currentUser.username);
        secureClearString(currentUser.passwordHash);
        secureClearString(currentUser.salt);
    }
}

bool DigitalJournalApp::isLoggedIn() const { return loggedIn; }

std::vector<Entry> DigitalJournalApp::searchEntries(const std::string& keyword) const {
    std::vector<Entry> allEntries = viewAllEntries();
    std::vector<Entry> foundEntries;
    for (const auto& entry : allEntries) {
        if (entry.title.find(keyword) != std::string::npos || entry.content.find(keyword) != std::string::npos) {
            foundEntries.push_back(entry);
        }
    }
    return foundEntries;
}

std::vector<Entry> DigitalJournalApp::filterEntriesByDate(time_t startDate, time_t endDate) const {
    std::vector<Entry> allEntries = viewAllEntries();
    std::vector<Entry> filteredEntries;
    for (const auto& entry : allEntries) {
        if (entry.timestamp >= startDate && entry.timestamp <= endDate) {
            filteredEntries.push_back(entry);
        }
    }
    return filteredEntries;
}

std::vector<Entry> DigitalJournalApp::filterEntriesByMood(const std::string& mood) const {
    std::vector<Entry> allEntries = viewAllEntries();
    std::vector<Entry> filteredEntries;
    for (const auto& entry : allEntries) {
        if (entry.mood == mood) {
            filteredEntries.push_back(entry);
        }
    }
    return filteredEntries;
}

void DigitalJournalApp::displayEntry(const Entry& entry) const {
    std::cout << "---------------------------------------" << std::endl;
    std::cout << BLUE << "Baslik: " << RESET << entry.title << std::endl;
    char timeBuffer[80];
    struct tm* tm_info = localtime(&entry.timestamp);
    strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", tm_info);
    std::cout << BLUE << "Tarih:  " << RESET << timeBuffer << std::endl;
    std::cout << BLUE << "Duygu:  " << RESET << entry.mood << std::endl;
    std::cout << BLUE << "Icerik: " << RESET << entry.content << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

void DigitalJournalApp::showLoginRegisterMenu() {
    int choice = 0;
    do {
        clearScreen();
        printHeader("GIRIS / KAYIT");
        std::cout << "1. Giris Yap" << std::endl;
        std::cout << "2. Kayit Ol" << std::endl;
        std::cout << YELLOW << "3. Uygulamadan Cik" << RESET << std::endl;
        std::cout << std::endl;
        choice = getValidatedInput(1, 3);
        switch (choice) {
            case 1: handleLogin(); break;
            case 2: handleRegister(); break;
            case 3: std::cout << "Uygulamadan cikiliyor..." << std::endl; break;
        }
        if (choice != 3 && !isLoggedIn()) {
            std::cout << "\nDevam etmek icin Enter'a basin...";
            std::cin.get();
        }
    } while (choice != 3 && !isLoggedIn());
}

void DigitalJournalApp::handleLogin() {
    clearScreen();
    printHeader("KULLANICI GIRISI");
    std::string username, password;
    std::cout << "Kullanici Adi: ";
    std::getline(std::cin, username);
    std::cout << "Parola: ";
    std::getline(std::cin, password);
    if (!loginUser(username, password)) {
        std::cout << RED << "Giris basarisiz. Lutfen tekrar deneyin." << RESET << std::endl;
    }
    secureClearString(password);
}

void DigitalJournalApp::handleRegister() {
    clearScreen();
    printHeader("YENI KULLANICI KAYDI");
    std::string username, password, confirmPassword;
    std::cout << "Yeni Kullanici Adi: ";
    std::getline(std::cin, username);
    std::cout << "Yeni Parola: ";
    std::getline(std::cin, password);
    std::cout << "Parolayi Tekrar Girin: ";
    std::getline(std::cin, confirmPassword);
    if (password != confirmPassword) {
        std::vector<char> msg = {
            static_cast<char>('P' ^ 0xAA), static_cast<char>('a' ^ 0xAA), static_cast<char>('r' ^ 0xAA),
            static_cast<char>('o' ^ 0xAA), static_cast<char>('l' ^ 0xAA), static_cast<char>('a' ^ 0xAA),
            static_cast<char>('l' ^ 0xAA), static_cast<char>('a' ^ 0xAA), static_cast<char>('r' ^ 0xAA),
            static_cast<char>(' ' ^ 0xAA), static_cast<char>('e' ^ 0xAA), static_cast<char>('s' ^ 0xAA),
            static_cast<char>('l' ^ 0xAA), static_cast<char>('e' ^ 0xAA), static_cast<char>('s' ^ 0xAA),
            static_cast<char>('m' ^ 0xAA), static_cast<char>('i' ^ 0xAA), static_cast<char>('y' ^ 0xAA),
            static_cast<char>('o' ^ 0xAA), static_cast<char>('r' ^ 0xAA), static_cast<char>('.' ^ 0xAA)
        };
        std::cout << RED << SecurityUtils::getObfuscatedString(msg) << RESET << std::endl;
        secureClearString(password);
        secureClearString(confirmPassword);
        return;
    }
    if (!registerUser(username, password)) {
        std::cout << RED << "Kayit basarisiz." << RESET << std::endl;
    }
    secureClearString(password);
    secureClearString(confirmPassword);
}

void DigitalJournalApp::showMainMenu() {
    int choice = 0;
    do {
        clearScreen();
        printHeader("ANA MENU");
        std::cout << "1. Yeni Gunluk Girisi Olustur" << std::endl;
        std::cout << "2. Tum Gunluk Girislerini Goruntule" << std::endl;
        std::cout << "3. Gunluk Girisi Ara" << std::endl;
        std::cout << "4. Gunlukleri Tarihe Gore Filtrele" << std::endl;
        std::cout << "5. Gunlukleri Duyguya Gore Filtrele" << std::endl;
        std::cout << "6. Parolayi Degistir" << std::endl;
        std::cout << "7. Duygu Gecmisini Goruntule" << std::endl;
        std::cout << YELLOW << "8. Cikis Yap" << RESET << std::endl;
        std::cout << std::endl;
        choice = getValidatedInput(1, 8);
        switch (choice) {
            case 1: handleCreateEntry(); break;
            case 2: handleViewEntries(); break;
            case 3: handleSearchEntries(); break;
            case 4: handleFilterEntriesByDate(); break;
            case 5: handleFilterEntriesByMood(); break;
            case 6: handleChangePassword(); break;
            case 7: handleViewMoodHistory(); break;
            case 8: logoutUser(); break;
        }
        if (choice != 8) {
            std::cout << "\nDevam etmek icin Enter'a basin...";
            std::cin.get();
        }
    } while (choice != 8);
}

void DigitalJournalApp::handleCreateEntry() {
    clearScreen();
    printHeader("YENI GUNLUK GIRISI");
    std::string title, content, mood;
    std::cout << "Baslik: ";
    std::getline(std::cin, title);
    std::cout << "Icerik: ";
    std::getline(std::cin, content);
    std::cout << "Duygu (Orn: Happy, Sad, Stressed): ";
    std::getline(std::cin, mood);
    createEntry(title, content, mood);
}

void DigitalJournalApp::handleViewEntries() {
    clearScreen();
    printHeader("TUM GUNLUKLER");
    std::vector<Entry> entries = viewAllEntries();
    if (entries.empty()) {
        std::cout << "Henuz gunluk girisi bulunmuyor." << std::endl;
        return;
    }
    for (const auto& entry : entries) {
        displayEntry(entry);
    }
}

void DigitalJournalApp::handleSearchEntries() {
    clearScreen();
    printHeader("GIRIS ARA");
    std::string keyword;
    std::cout << "Aranacak Anahtar Kelime: ";
    std::getline(std::cin, keyword);
    std::vector<Entry> foundEntries = searchEntries(keyword);
    if (foundEntries.empty()) {
        std::cout << "Anahtar kelimeyle eslesen giris bulunamadi." << std::endl;
        return;
    }
    for (const auto& entry : foundEntries) {
        displayEntry(entry);
    }
}

void DigitalJournalApp::handleFilterEntriesByDate() {
    clearScreen();
    printHeader("TARIHE GORE FILTRELE");
    std::cout << "Baslangic Tarihi (YYYY-MM-DD HH:MM:SS): ";
    std::string startDateStr;
    std::getline(std::cin, startDateStr);
    std::cout << "Bitis Tarihi (YYYY-MM-DD HH:MM:SS): ";
    std::string endDateStr;
    std::getline(std::cin, endDateStr);
    struct tm tm_start = {0}, tm_end = {0};
    std::istringstream ss_start(startDateStr);
    ss_start >> std::get_time(&tm_start, "%Y-%m-%d %H:%M:%S");
    if (ss_start.fail()) {
        std::cout << RED << "Gecersiz baslangic tarihi formati." << RESET << std::endl;
        return;
    }
    std::istringstream ss_end(endDateStr);
    ss_end >> std::get_time(&tm_end, "%Y-%m-%d %H:%M:%S");
    if (ss_end.fail()) {
        std::cout << RED << "Gecersiz bitis tarihi formati." << RESET << std::endl;
        return;
    }
    time_t startDate = mktime(&tm_start);
    time_t endDate = mktime(&tm_end);
    std::vector<Entry> filteredEntries = filterEntriesByDate(startDate, endDate);
    if (filteredEntries.empty()) {
        std::cout << "Belirtilen tarihler arasinda giris bulunamadi." << std::endl;
    } else {
        for (const auto& entry : filteredEntries) {
            displayEntry(entry);
        }
    }
}

void DigitalJournalApp::handleFilterEntriesByMood() {
    clearScreen();
    printHeader("DUYGUYA GORE FILTRELE");
    std::string mood;
    std::cout << "Filtrelemek istediginiz Duygu: ";
    std::getline(std::cin, mood);
    std::vector<Entry> filteredEntries = filterEntriesByMood(mood);
    if (filteredEntries.empty()) {
        std::cout << "Bu duyguyla eslesen giris bulunamadi." << std::endl;
    } else {
        for (const auto& entry : filteredEntries) {
            displayEntry(entry);
        }
    }
}

void DigitalJournalApp::handleChangePassword() {
    clearScreen();
    printHeader("PAROLA DEGISTIR");
    std::string oldPassword, newPassword, confirmNewPassword;
    std::cout << "Mevcut Parola: ";
    std::getline(std::cin, oldPassword);
    std::cout << "Yeni Parola: ";
    std::getline(std::cin, newPassword);
    std::cout << "Yeni Parolayi Tekrar Girin: ";
    std::getline(std::cin, confirmNewPassword);
    if (newPassword != confirmNewPassword) {
        std::cout << RED << "Yeni parolalar eslesmiyor." << RESET << std::endl;
        secureClearString(oldPassword);
        secureClearString(newPassword);
        secureClearString(confirmNewPassword);
        return;
    }
    if (!changePassword(oldPassword, newPassword)) {
        std::cout << RED << "Parola degistirme basarisiz." << RESET << std::endl;
    }
    secureClearString(oldPassword);
    secureClearString(newPassword);
    secureClearString(confirmNewPassword);
}

void DigitalJournalApp::handleViewMoodHistory() {
    clearScreen();
    printHeader("DUYGU GECMISI");
    std::vector<Entry> allEntries = viewAllEntries();
    if (allEntries.empty()) {
        std::cout << "Henuz gunluk girisi bulunmuyor." << std::endl;
        return;
    }
    std::map<std::string, int> moodCounts;
    for (const auto& entry : allEntries) {
        moodCounts[entry.mood]++;
    }
    std::cout << "----------------------" << std::endl;
    for (const auto& pair : moodCounts) {
        std::cout << pair.first << ": " << pair.second << " kez" << std::endl;
    }
    std::cout << "----------------------" << std::endl;
}

void DigitalJournalApp::run() {
    showLoginRegisterMenu();
    if (isLoggedIn()) {
        showMainMenu();
    }
}