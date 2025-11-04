#include "gtest/gtest.h"
#include "SecurityUtils.h"

// String gizleme ve çözme fonksiyonunun doğru çalışıp çalışmadığını test eder.
TEST(SecurityUtilsTest, ObfuscationDecryption) {
    std::string original = "Bu bir test metnidir.";
    std::vector<char> obfuscated;
    for(char c : original) {
        obfuscated.push_back(c ^ 0xAA);
    }

    std::string decrypted = SecurityUtils::getObfuscatedString(obfuscated);
    
    EXPECT_EQ(original, decrypted);
}