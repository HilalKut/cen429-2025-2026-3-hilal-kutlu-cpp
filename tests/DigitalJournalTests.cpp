/* tests/DigitalJournalTests.cpp */
#include "gtest/gtest.h"
#include "digitaljournal.h"
#include "CryptoUtils.h"
#include "SecurityUtils.h"

class DigitalJournalAppTest : public ::testing::Test {
protected:
    const std::string dbPath = "test_unit.db";
    void SetUp() override { std::remove(dbPath.c_str()); }
    void TearDown() override { std::remove(dbPath.c_str()); }
};

// Test 1: Kayıt ve Giriş Mantığı (Rubrik: Güvenlik Analizi)
TEST_F(DigitalJournalAppTest, RegistrationAndLoginLogic) {
    DigitalJournalApp app(dbPath);
    EXPECT_TRUE(app.registerUser("admin", "Sifre123!"));
    EXPECT_FALSE(app.registerUser("admin", "FarkliSifre")); // Aynı kullanıcı engellenmeli
    EXPECT_TRUE(app.loginUser("admin", "Sifre123!"));
    EXPECT_TRUE(app.loggedIn);
}

// Test 2: Veri Gizliliği ve Şifreleme (Rubrik: Veri Güvenliği)
TEST_F(DigitalJournalAppTest, EncryptionIntegrity) {
    DigitalJournalApp app(dbPath);
    app.registerUser("user1", "pass1");
    app.loginUser("user1", "pass1");
    
    std::string secret = "Gizli Notum";
    app.createEntry("Gunluk 1", secret, "Mutlu");
    
    auto entries = app.viewAllEntries();
    ASSERT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0].content, secret); // Uygulama içinden okuyunca çözülmüş olmalı
}

// Test 3: Kriptografi Fonksiyonları (Rubrik: Kriptografi)
TEST(SecurityLogicTest, CryptoCheck) {
    std::string key = "super_secret_key";
    std::string data = "Hassas Veri";
    std::string enc = CryptoUtils::AES_Encrypt_Simulated(data, key);
    EXPECT_NE(data, enc);
    EXPECT_EQ(data, CryptoUtils::AES_Decrypt_Simulated(enc, key));
}

// Test 4: Kod Sertleştirme (Rubrik: String Gizleme)
TEST(SecurityLogicTest, ObfuscationCheck) {

    std::vector<char> hidden = {
        static_cast<char>('T' ^ 0xAA), 
        static_cast<char>('e' ^ 0xAA), 
        static_cast<char>('s' ^ 0xAA), 
        static_cast<char>('t' ^ 0xAA)
    };
    EXPECT_EQ(SecurityUtils::getObfuscatedString(hidden), "Test");
}