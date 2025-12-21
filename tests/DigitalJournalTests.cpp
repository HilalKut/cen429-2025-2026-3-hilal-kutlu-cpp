#include "gtest/gtest.h"
#include "digitaljournal.h"
#include "CryptoUtils.h"
#include "SecurityUtils.h"

class DigitalJournalAppTest : public ::testing::Test {
protected:
    const std::string dbPath = "test_unit.db";
    std::unique_ptr<DigitalJournalApp> app;

    void SetUp() override { 
        std::remove(dbPath.c_str()); 
        app = std::make_unique<DigitalJournalApp>(dbPath);
    }
    void TearDown() override { 
        app.reset();
        std::remove(dbPath.c_str()); 
    }
};

// --- 1. KAYIT VE GİRİŞ (HATA SENARYOLARI DAHİL) ---
TEST_F(DigitalJournalAppTest, RegistrationAndLoginLogic) {
    // Başarılı kayıt
    EXPECT_TRUE(app->registerUser("admin", "Sifre123!"));
    // Hata: Aynı kullanıcı adı (Coverage: registerUser failure branch)
    EXPECT_FALSE(app->registerUser("admin", "FarkliSifre"));
    // Hata: Olmayan kullanıcıyla giriş (Coverage: loginUser failure branch)
    EXPECT_FALSE(app->loginUser("olmayan", "sifre"));
    // Hata: Yanlış şifreyle giriş (Coverage: loginUser wrong password branch)
    EXPECT_FALSE(app->loginUser("admin", "YanlisSifre"));
    // Başarılı giriş
    EXPECT_TRUE(app->loginUser("admin", "Sifre123!"));
    EXPECT_TRUE(app->isLoggedIn());
}

// --- 2. ŞİFRE DEĞİŞTİRME VE ÇIKIŞ ---
TEST_F(DigitalJournalAppTest, UserAccountManagement) {
    app->registerUser("user1", "eski_sifre");
    app->loginUser("user1", "eski_sifre");
    
    // Başarılı şifre değiştirme (Coverage: changePassword success)
    EXPECT_TRUE(app->changePassword("eski_sifre", "yeni_sifre"));
    // Hata: Yanlış eski şifre (Coverage: changePassword failure branch)
    EXPECT_FALSE(app->changePassword("yanlis_eski", "deneme"));
    
    // Çıkış yapma (Coverage: logoutUser function)
    app->logoutUser();
    EXPECT_FALSE(app->isLoggedIn());
}

// --- 3. GÜNLÜK İŞLEMLERİ, ARAMA VE TARİH FİLTRELEME ---
TEST_F(DigitalJournalAppTest, EntryAndSearchOperations) {
    app->registerUser("tester", "pass");
    app->loginUser("tester", "pass");
    
    app->createEntry("Kritik Gun", "Gizli mesaj", "Stressed");
    app->createEntry("Mutlu An", "Bugun hava guzel", "Happy");
    
    // İçerik kontrolü
    auto entries = app->viewAllEntries();
    ASSERT_EQ(entries.size(), 2);
    
    // Arama (Coverage: searchEntries success)
    auto results = app->searchEntries("Gizli");
    EXPECT_EQ(results.size(), 1);
    // Hata: Bulunamayan arama (Coverage: searchEntries empty branch)
    EXPECT_TRUE(app->searchEntries("Uzayli").empty());
    
    // Duygu filtreleme
    auto moodResults = app->filterEntriesByMood("Happy");
    EXPECT_EQ(moodResults.size(), 1);

    // Tarih filtreleme (Coverage: filterEntriesByDate function)
    time_t simNow = time(nullptr);
    auto dateResults = app->filterEntriesByDate(simNow - 3600, simNow + 3600);
    EXPECT_FALSE(dateResults.empty());
}

// --- 4. KRİPTOGRAFİ VE GÜVENLİK ---
TEST(SecurityLogicTest, AdvancedCryptoTests) {
    // AES Simülasyonu
    std::string key = "key";
    std::string data = "data";
    std::string enc = CryptoUtils::AES_Encrypt_Simulated(data, key);
    EXPECT_EQ(data, CryptoUtils::AES_Decrypt_Simulated(enc, key));
    
    // SHA ve HMAC Coverage (Daha önce yaptığımız %93'ü korur)
    EXPECT_FALSE(CryptoUtils::SHA256_Hash("test").empty());
    EXPECT_TRUE(CryptoUtils::HMAC_Simulated("d", "k").find("HMAC_") == 0);
}

TEST(SecurityLogicTest, ObfuscationCheck) {
    std::vector<char> hidden = {
        static_cast<char>('T' ^ 0xAA), static_cast<char>('e' ^ 0xAA), 
        static_cast<char>('s' ^ 0xAA), static_cast<char>('t' ^ 0xAA)
    };
    EXPECT_EQ(SecurityUtils::getObfuscatedString(hidden), "Test");
}