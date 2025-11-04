#include "gtest/gtest.h"
#include "digitaljournal.h" // Test edilecek sınıfımız
#include <cstdio>           // remove() fonksiyonu ile test veritabanını silmek için
#include <fstream>          // Dosyanın varlığını kontrol etmek için


// --- Test Fixture ---
// Bu sınıf, her test için ortak olan kurulum (SetUp) ve temizlik (TearDown)
// işlemlerini yönetir. Bu sayede her testimiz temiz bir ortamda başlar.
class DigitalJournalAppTest : public ::testing::Test {
protected:
    // Her test için benzersiz bir veritabanı dosyası adı belirliyoruz.
    const std::string testDbPath = "test_journal_for_gtest.db";
    
    // Test edilecek olan app nesnemiz için bir pointer.
    std::unique_ptr<DigitalJournalApp> app;

    // Bu fonksiyon her bir TEST_F'ten ÖNCE çalışır.
    void SetUp() override {
        // Önceki bir test başarısız olduysa kalmış olabilecek eski
        // veritabanı dosyasını silerek temiz bir başlangıç yapıyoruz.
        remove(testDbPath.c_str());

        // Test app nesnemizi, özel test veritabanı yolumuzla oluşturuyoruz.
        app = std::make_unique<DigitalJournalApp>(testDbPath);
    }

    // Bu fonksiyon her bir TEST_F'ten SONRA çalışır.
    void TearDown() override {
        // app nesnesinin destructor'ı veritabanı bağlantısını kapatacaktır.
        // Biz sadece oluşturulan test veritabanı dosyasını siliyoruz.
        remove(testDbPath.c_str());
    }
};

// --- Test Senaryoları ---

// TEST_F, Test Fixture'ımızı (DigitalJournalAppTest) kullanan bir test tanımlar.
// Bu test, başarılı bir kullanıcı kaydını ve ardından aynı kullanıcıyı
// tekrar kaydetmeye çalışmanın başarısız olmasını doğrular.
TEST_F(DigitalJournalAppTest, RegistrationSuccessAndFailure) {
    // Başarılı bir şekilde yeni bir kullanıcı kaydetmeyi dene.
    // app->registerUser'ın private olduğuna dikkat edin. Bu testi çalıştırmak için
    // DigitalJournalApp sınıfına FRIEND_TEST eklemeniz gerekir.
    // Şimdilik, bu fonksiyonları public yaptığımızı varsayalım.
    
    // Düzeltme: Fonksiyonları public yapmak yerine FRIEND_TEST kullanalım.
    // DigitalJournalApp sınıfına `FRIEND_TEST(DigitalJournalAppTest, RegistrationSuccessAndFailure);` ekleyin.

    // Başarılı kayıt senaryosu
    ASSERT_TRUE(app->registerUser("testuser", "password123"));

    // Başarısız kayıt senaryosu (aynı kullanıcı adı)
    ASSERT_FALSE(app->registerUser("testuser", "anotherpassword"));
}

// Bu test, doğru ve yanlış parolalarla yapılan giriş denemelerini doğrular.
TEST_F(DigitalJournalAppTest, LoginSuccessAndFailure) {
    // Önce test için bir kullanıcı oluşturalım.
    app->registerUser("testuser", "password123");

    // Başarısız giriş (yanlış parola)
    ASSERT_FALSE(app->loginUser("testuser", "wrongpassword"));
    ASSERT_FALSE(app->isLoggedIn());

    // Başarısız giriş (var olmayan kullanıcı)
    ASSERT_FALSE(app->loginUser("nonexistentuser", "password123"));
    ASSERT_FALSE(app->isLoggedIn());

    // Başarılı giriş
    ASSERT_TRUE(app->loginUser("testuser", "password123"));
    ASSERT_TRUE(app->isLoggedIn());
}

// Bu test, bir günlük girdisinin oluşturulmasını, şifrelenerek kaydedilmesini
// ve ardından okunurken doğru bir şekilde şifresinin çözülmesini doğrular.
TEST_F(DigitalJournalAppTest, CreateAndRetrieveEntry) {
    app->registerUser("testuser", "password123");
    app->loginUser("testuser", "password123");

    const std::string title = "My Test Entry";
    const std::string content = "This is the secret content of my entry.";
    const std::string mood = "Tested";

    // Yeni bir giriş oluştur.
    ASSERT_TRUE(app->createEntry(title, content, mood));

    // Tüm girişleri görüntüle.
    std::vector<Entry> entries = app->viewAllEntries();

    // Sonuçları doğrula.
    ASSERT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0].title, title);
    EXPECT_EQ(entries[0].content, content); // En önemli kontrol: Şifre çözmenin doğru çalıştığını doğrular.
    EXPECT_EQ(entries[0].mood, mood);
}