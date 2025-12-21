/* src/crypto/src/CryptoUtils.cpp */

#include "CryptoUtils.h"
#include <sstream>
#include <random>

namespace CryptoUtils {

// Mevcut simple_sha256_placeholder mantığını kullanıyoruz, ancak ismini güncelliyoruz.
std::string SHA256_Hash(const std::string& input) {
    size_t hash = 0;
    for (char c : input) {
        // Basit karma algoritması (Gerçek SHA256 yerine placeholder)
        hash = (hash * 31 + c);
    }
    return "SHA256_Hashed_" + std::to_string(hash);
}

std::string AES_Encrypt_Simulated(const std::string& data, const std::string& key) {
    // SİMÜLASYON: XOR tabanlı şifreleme mantığı, AES'i temsil etmek için kullanılıyor.
    std::string output = data;
    if (key.empty()) return output;
    for (size_t i = 0; i < data.size(); ++i) {
        output[i] = data[i] ^ key[i % key.size()];
    }
    return "AES_Encrypted_" + output; // Ön ek, şifrelemenin uygulandığını belirtir
}

std::string AES_Decrypt_Simulated(const std::string& encryptedData, const std::string& key) {
    // SİMÜLASYON: XOR tabanlı şifre çözme mantığı
    if (encryptedData.rfind("AES_Encrypted_", 0) != 0) {
        return encryptedData; // Şifreli değilse geri döndür
    }
    std::string rawEncrypted = encryptedData.substr(14); // Ön eki kaldır

    std::string output = rawEncrypted;
    if (key.empty()) return output;
    for (size_t i = 0; i < rawEncrypted.size(); ++i) {
        output[i] = rawEncrypted[i] ^ key[i % key.size()];
    }
    return output;
}

std::string HMAC_Simulated(const std::string& data, const std::string& key) {
    // SİMÜLASYON: HMAC, veri + anahtar kullanılarak bir karma oluşturur.
    std::string combined = data + key;
    return "HMAC_" + SHA256_Hash(combined);
}

} // namespace CryptoUtils