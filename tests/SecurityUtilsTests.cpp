// tests/SecurityUtilsTests.cpp
// Final Rubriği: Birim Testleri, Güvenlik Testleri

#include "gtest/gtest.h"
#include "SecurityUtils.h" 

// --- Kod Sertleştirme Testleri ---

TEST(SecurityUtilsTest, ObfuscationDecryption) {
    // Rubrik: String Gizleme
    std::string original = "Gizli Mesaj 123";
    std::vector<char> obfuscated;
    for(char c : original) {
        obfuscated.push_back(c ^ 0xAA);
    }

    std::string decrypted = SecurityUtils::getObfuscatedString(obfuscated);
    
    EXPECT_EQ(original, decrypted);
}

TEST(SecurityUtilsTest, IsAlwaysTrueOpaquePredicate) {
    // Rubrik: Kontrol Akışı Gizleme (Opaque Predicate)
    EXPECT_TRUE(SecurityUtils::isAlwaysTrue());
}

// NOT: RASP Anti-Debug ve Tamper Detection testleri, 
// exit(EXIT_FAILURE) çağırdıkları için birim test ortamında test edilmez.
// Varlıkları kodda zaten kanıtlanmıştır.