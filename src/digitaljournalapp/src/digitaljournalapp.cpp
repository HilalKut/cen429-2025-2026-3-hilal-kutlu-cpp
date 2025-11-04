#include "digitaljournal.h"
#include "SecurityUtils.h" // Güvenlik modülünü dahil et
#include <iostream>
#include <limits>
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>
#include <memory>
#include <map> // map başlık dosyası

// Basit bir karma algoritması ve salt üretimi için yardımcı fonksiyonlar
// GERÇEK UYGULAMADA: Kriptografik olarak güvenli kütüphaneler (örn. OpenSSL, Crypto++) kullanılmalıdır.
// Bu örnek, kavramı göstermek amaçlıdır.
std::string _simple_sha256_placeholder(const std::string& input) {
    // Bu sadece bir yer tutucudur. Gerçek bir SHA256 algoritması burada olmalı.
    size_t hash = 0;
    for (char c : input) {
        hash = (hash * 31 + c);
    }
    return "hashed_" + std::to_string(hash);
}

std::string DigitalJournalApp::generateSalt() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(0, 255);
    std::string salt_str;
    for (int i = 0; i < 16; ++i) { // 16 byte salt
        salt_str += static_cast<char>(distrib(generator));
    }
    return salt_str;
}

std::string DigitalJournalApp::hashPassword(const std::string& password, const std::string& salt) {
    std::string saltedPassword = password + salt;
    return _simple_sha256_placeholder(saltedPassword);
}

// Vize Rubriği: Veri Güvenliği (Depolama ve Kullanım Anında)
// Basit bir XOR şifreleme. Gerçek uygulamada AES gibi güçlü bir algoritma kullanılmalıdır.
std::string DigitalJournalApp::encryptData(const std::string& data, const std::string& key) const {
    std::string output = data;
    if (key.empty()) return output; // Anahtar boşsa şifreleme yapma
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

// Vize Rubriği: Veri Güvenliği (Bellek Koruması)
void DigitalJournalApp::secureClearString(std::string& s) {
    std::fill(s.begin(), s.end(), 0);
    s.clear();
}

void DigitalJournalApp::secureClearMemory(void* ptr, size_t size) {
    if (ptr) {
        volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);
        while (size--) {
            *p++ = 0;
        }
    }
}

DigitalJournalApp::DigitalJournalApp() : loggedIn(false) {}

DigitalJournalApp::~DigitalJournalApp() {
    if (loggedIn) {
        logoutUser();
    }
    secureClearString(currentUser.passwordHash);
    secureClearString(currentUser.salt);
    secureClearString(currentUser.username);
}

bool DigitalJournalApp::loadUserData(const std::string& username, User& user) {
    std::string userFilePath = "users/" + username + ".dat";
    std::ifstream file(userFilePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::getline(file, user.username);
    std::getline(file, user.passwordHash);
    std::getline(file, user.salt);

    file.close();
    return true;
}

bool DigitalJournalApp::saveUserData(const User& user) {
#ifdef _WIN32
    system("mkdir users");
#else
    system("mkdir -p users");
#endif

    std::string userFilePath = "users/" + user.username + ".dat";
    std::ofstream file(userFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Hata: Kullanici verileri dosyasi kaydedilemiyor." << std::endl;
        return false;
    }

    file << user.username << std::endl;
    file << user.passwordHash << std::endl;
    file << user.salt << std::endl;

    file.close();
    return true;
}

bool DigitalJournalApp::loadEntries(const std::string& username, std::vector<Entry>& entries) const {
    std::string entriesFilePath = "entries/" + username + "_entries.dat";
    std::ifstream file(entriesFilePath, std::ios::binary);
    if (!file.is_open()) {
        return false; // Henüz hiç giriş yoksa bu normal bir durum.
    }

    // Geliştirilmiş Veri Güvenliği: Her kullanıcının verisi kendi parola hash'i ile şifrelenir.
    std::string user_key = currentUser.passwordHash;

    entries.clear();
    Entry entry;
    std::string line;
    while (std::getline(file, line)) { // title
        entry.title = line;
        if (!std::getline(file, line)) break; // content
        entry.content = decryptData(line, user_key); // Şifreli içeriği çöz
        if (!std::getline(file, line)) break; // mood
        entry.mood = line;
        if (!std::getline(file, line)) break; // timestamp
        try {
            entry.timestamp = std::stoll(line);
        } catch (const std::exception& e) {
            continue;
        }
        entries.push_back(entry);
    }

    file.close();
    return true;
}

bool DigitalJournalApp::saveEntries(const std::string& username, const std::vector<Entry>& entries) const {
#ifdef _WIN32
    system("mkdir entries");
#else
    system("mkdir -p entries");
#endif

    std::string entriesFilePath = "entries/" + username + "_entries.dat";
    std::ofstream file(entriesFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Hata: Gunluk girisleri dosyasi kaydedilemiyor." << std::endl;
        return false;
    }

    std::string user_key = currentUser.passwordHash;

    for (const auto& entry : entries) {
        file << entry.title << std::endl;
        file << encryptData(entry.content, user_key) << std::endl; // İçeriği şifrele
        file << entry.mood << std::endl;
        file << entry.timestamp << std::endl;
    }

    file.close();
    return true;
}

bool DigitalJournalApp::registerUser(const std::string& username, const std::string& password) {
    User tempUser;
    if (loadUserData(username, tempUser)) {
        std::cout << "Bu kullanici adi zaten mevcut." << std::endl;
        return false;
    }

    User newUser;
    newUser.username = username;
    newUser.salt = generateSalt();
    newUser.passwordHash = hashPassword(password, newUser.salt);

    if (saveUserData(newUser)) {
        std::cout << "Kayit basarili. Lutfen giris yapin." << std::endl;
        secureClearString(newUser.passwordHash);
        secureClearString(newUser.salt);
        return true;
    }
    return false;
}

bool DigitalJournalApp::loginUser(const std::string& username, const std::string& password) {
    User tempUser;
    if (!loadUserData(username, tempUser)) {
        std::cout << "Kullanici adi veya parola hatali." << std::endl;
        return false;
    }

    std::string hashedPasswordAttempt = hashPassword(password, tempUser.salt);
    if (hashedPasswordAttempt == tempUser.passwordHash) {
        loggedIn = true;
        currentUser = tempUser;
        currentUserFilePath = "entries/" + username + "_entries.dat";
        
        // Vize Rubriği: Kod Sertleştirme (String Gizleme)
        std::vector<char> msg = {('G'^0xAA), ('i'^0xAA), ('r'^0xAA), ('i'^0xAA), ('s'^0xAA), (' '^0xAA), ('b'^0xAA), ('a'^0xAA), ('s'^0xAA), ('a'^0xAA), ('r'^0xAA), ('i'^0xAA), ('l'^0xAA), ('i'^0xAA), ('.' ^0xAA)};
        std::cout << SecurityUtils::getObfuscatedString(msg) << " Hos geldiniz, " << currentUser.username << "!" << std::endl;

        secureClearString(hashedPasswordAttempt);
        secureClearString(tempUser.passwordHash);
        secureClearString(tempUser.salt);
        return true;
    } else {
        std::cout << "Kullanici adi veya parola hatali." << std::endl;
        secureClearString(hashedPasswordAttempt);
        secureClearString(tempUser.passwordHash);
        secureClearString(tempUser.salt);
        return false;
    }
}

void DigitalJournalApp::logoutUser() {
    if (loggedIn) {
        std::cout << "Cikis yapiliyor..." << std::endl;
        loggedIn = false;
        secureClearString(currentUser.username);
        secureClearString(currentUser.passwordHash);
        secureClearString(currentUser.salt);
        currentUserFilePath.clear();
    } else {
        std::cout << "Zaten cikis yapmis durumdasiniz." << std::endl;
    }
}

bool DigitalJournalApp::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (!loggedIn) {
        std::cout << "Bu islemi yapmak icin giris yapmalisiniz." << std::endl;
        return false;
    }

    std::string oldPasswordHashAttempt = hashPassword(oldPassword, currentUser.salt);
    if (oldPasswordHashAttempt != currentUser.passwordHash) {
        std::cout << "Mevcut parolaniz hatali." << std::endl;
        secureClearString(oldPasswordHashAttempt);
        return false;
    }
    secureClearString(oldPasswordHashAttempt);

    std::string newSalt = generateSalt();
    std::string newPasswordHash = hashPassword(newPassword, newSalt);

    currentUser.salt = newSalt;
    currentUser.passwordHash = newPasswordHash;

    if (saveUserData(currentUser)) {
        std::cout << "Parola basariyla degistirildi." << std::endl;
        secureClearString(newPasswordHash);
        secureClearString(newSalt);
        return true;
    } else {
        std::cout << "Parola degistirilirken bir hata olustu." << std::endl;
        secureClearString(newPasswordHash);
        secureClearString(newSalt);
        return false;
    }
}

bool DigitalJournalApp::isLoggedIn() const {
    return loggedIn;
}

bool DigitalJournalApp::createEntry(const std::string& title, const std::string& content, const std::string& mood) {
    if (!loggedIn) {
        std::cout << "Gunluk girisi olusturmak icin giris yapmalisiniz." << std::endl;
        return false;
    }

    std::vector<Entry> entries;
    loadEntries(currentUser.username, entries);

    Entry newEntry;
    newEntry.title = title;
    newEntry.content = content;
    newEntry.mood = mood;
    newEntry.timestamp = time(nullptr);

    entries.push_back(newEntry);

    if (saveEntries(currentUser.username, entries)) {
        std::cout << "Gunluk girisi basariyla olusturuldu." << std::endl;
        return true;
    }
    return false;
}

std::vector<Entry> DigitalJournalApp::viewAllEntries() const {
    std::vector<Entry> entries;
    if (!loggedIn) {
        std::cout << "Gunlukleri gormek icin giris yapmalisiniz." << std::endl;
        return entries;
    }
    loadEntries(currentUser.username, entries);
    return entries;
}

std::vector<Entry> DigitalJournalApp::searchEntries(const std::string& keyword) const {
    std::vector<Entry> allEntries = viewAllEntries();
    std::vector<Entry> foundEntries;
    for (const auto& entry : allEntries) {
        if (entry.title.find(keyword) != std::string::npos ||
            entry.content.find(keyword) != std::string::npos) {
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
    std::cout << "Baslik: " << entry.title << std::endl;

    char timeBuffer[80];
    struct tm* tm_info;
    tm_info = localtime(&entry.timestamp);
    strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", tm_info);
    std::cout << "Tarih:  " << timeBuffer << std::endl;

    std::cout << "Duygu:  " << entry.mood << std::endl;
    std::cout << "Icerik: " << entry.content << std::endl;
    std::cout << "---------------------------------------" << std::endl;
}

void DigitalJournalApp::showLoginRegisterMenu() {
    int choice;
    do {
        std::cout << "\n--- Giris/Kayit Menusu ---" << std::endl;
        std::cout << "1. Giris Yap" << std::endl;
        std::cout << "2. Kayit Ol" << std::endl;
        std::cout << "3. Cikis" << std::endl;
        std::cout << "Seciminiz: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Gecersiz giris. Lutfen bir sayi girin." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: handleLogin(); break;
            case 2: handleRegister(); break;
            case 3: std::cout << "Uygulamadan cikiliyor..." << std::endl; break;
            default: std::cout << "Gecersiz secim. Lutfen tekrar deneyin." << std::endl; break;
        }
    } while (choice != 3 && !isLoggedIn());
}

void DigitalJournalApp::handleLogin() {
    std::string username, password;
    std::cout << "Kullanici Adi: ";
    std::getline(std::cin, username);
    std::cout << "Parola: ";
    std::getline(std::cin, password);

    if (!loginUser(username, password)) {
        std::cout << "Giris basarisiz. Lutfen tekrar deneyin." << std::endl;
    }
    secureClearString(password);
}

void DigitalJournalApp::handleRegister() {
    std::string username, password, confirmPassword;
    std::cout << "Yeni Kullanici Adi: ";
    std::getline(std::cin, username);
    std::cout << "Yeni Parola: ";
    std::getline(std::cin, password);
    std::cout << "Parolayi Tekrar Girin: ";
    std::getline(std::cin, confirmPassword);

    if (password != confirmPassword) {
        std::cout << "Parolalar eslesmiyor." << std::endl;
        secureClearString(password);
        secureClearString(confirmPassword);
        return;
    }

    if (!registerUser(username, password)) {
        std::cout << "Kayit basarisiz." << std::endl;
    }
    secureClearString(password);
    secureClearString(confirmPassword);
}

void DigitalJournalApp::showMainMenu() {
    int choice;
    do {
        std::cout << "\n--- Ana Menu ---" << std::endl;
        std::cout << "1. Yeni Gunluk Girisi Olustur" << std::endl;
        std::cout << "2. Tum Gunluk Girislerini Goruntule" << std::endl;
        std::cout << "3. Gunluk Girisi Ara (Anahtar Kelimeye Gore)" << std::endl;
        std::cout << "4. Gunluk Girislerini Tarihe Gore Filtrele" << std::endl;
        std::cout << "5. Gunluk Girislerini Duyguya Gore Filtrele" << std::endl;
        std::cout << "6. Parolayi Degistir" << std::endl;
        std::cout << "7. Duygu Gecmisini Goruntule" << std::endl;
        std::cout << "8. Cikis Yap" << std::endl;
        std::cout << "Seciminiz: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Gecersiz giris. Lutfen bir sayi girin." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: handleCreateEntry(); break;
            case 2: handleViewEntries(); break;
            case 3: handleSearchEntries(); break;
            case 4: handleFilterEntriesByDate(); break;
            case 5: handleFilterEntriesByMood(); break;
            case 6: handleChangePassword(); break;
            case 7: handleViewMoodHistory(); break;
            case 8: logoutUser(); break;
            default: std::cout << "Gecersiz secim. Lutfen tekrar deneyin." << std::endl; break;
        }
    } while (isLoggedIn());
}

void DigitalJournalApp::handleCreateEntry() {
    std::string title, content, mood;
    std::cout << "Baslik: ";
    std::getline(std::cin, title);
    std::cout << "Icerik: ";
    std::getline(std::cin, content);
    std::cout << "Duygu (Orn: Happy, Sad, Stressed, Relaxed): ";
    std::getline(std::cin, mood);

    createEntry(title, content, mood);
}

void DigitalJournalApp::handleViewEntries() {
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
        std::cout << "Gecersiz baslangic tarihi formati." << std::endl;
        return;
    }

    std::istringstream ss_end(endDateStr);
    ss_end >> std::get_time(&tm_end, "%Y-%m-%d %H:%M:%S");
    if (ss_end.fail()) {
        std::cout << "Gecersiz bitis tarihi formati." << std::endl;
        return;
    }

    time_t startDate = mktime(&tm_start);
    time_t endDate = mktime(&tm_end);

    std::vector<Entry> filteredEntries = filterEntriesByDate(startDate, endDate);
    if (filteredEntries.empty()) {
        std::cout << "Belirtilen tarihler arasinda giris bulunamadi." << std::endl;
        return;
    }
    for (const auto& entry : filteredEntries) {
        displayEntry(entry);
    }
}

void DigitalJournalApp::handleFilterEntriesByMood() {
    std::string mood;
    std::cout << "Filtrelemek istediginiz Duygu (Orn: Happy, Sad, Stressed, Relaxed): ";
    std::getline(std::cin, mood);

    std::vector<Entry> filteredEntries = filterEntriesByMood(mood);
    if (filteredEntries.empty()) {
        std::cout << "Bu duyguyla eslesen giris bulunamadi." << std::endl;
        return;
    }
    for (const auto& entry : filteredEntries) {
        displayEntry(entry);
    }
}

void DigitalJournalApp::handleChangePassword() {
    std::string oldPassword, newPassword, confirmNewPassword;
    std::cout << "Mevcut Parola: ";
    std::getline(std::cin, oldPassword);
    std::cout << "Yeni Parola: ";
    std::getline(std::cin, newPassword);
    std::cout << "Yeni Parolayı Tekrar Girin: ";
    std::getline(std::cin, confirmNewPassword);

    if (newPassword != confirmNewPassword) {
        std::cout << "Yeni parolalar eslesmiyor." << std::endl;
        secureClearString(oldPassword);
        secureClearString(newPassword);
        secureClearString(confirmNewPassword);
        return;
    }

    if (!changePassword(oldPassword, newPassword)) {
        std::cout << "Parola degistirme basarisiz." << std::endl;
    }
    secureClearString(oldPassword);
    secureClearString(newPassword);
    secureClearString(confirmNewPassword);
}

void DigitalJournalApp::handleViewMoodHistory() {
    std::vector<Entry> allEntries = viewAllEntries();
    if (allEntries.empty()) {
        std::cout << "Henuz gunluk girisi bulunmuyor, duygu gecmisi goruntulenemiyor." << std::endl;
        return;
    }

    // std::map kullanarak her bir duygunun kaç kez tekrarlandığını sayalım.
    std::map<std::string, int> moodCounts;
    for (const auto& entry : allEntries) {
        moodCounts[entry.mood]++;
    }

    std::cout << "\n--- Duygu Gecmisi ---" << std::endl;
    if (moodCounts.empty()) {
        std::cout << "Gosterilecek duygu gecmisi yok." << std::endl;
    } else {
        for (const auto& pair : moodCounts) {
            std::cout << pair.first << ": " << pair.second << " kez" << std::endl;
        }
    }
    std::cout << "----------------------" << std::endl;
}

void DigitalJournalApp::run() {
    showLoginRegisterMenu();

    // Kullanıcı başarıyla giriş yaptıysa ana menüyü göster.
    if (isLoggedIn()) {
        showMainMenu();
    }
}