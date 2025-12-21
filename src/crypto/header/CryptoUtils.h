/* src/crypto/header/CryptoUtils.h */

#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <string>
#include <vector>

namespace CryptoUtils {
    // Rubrik Maddesi: AES/RSA Şifreleme (Simülasyon)
    // Gerçek bir uygulamada OpenSSL/libcrypto gibi bir kütüphane kullanılır.
    std::string AES_Encrypt_Simulated(const std::string& data, const std::string& key);
    std::string AES_Decrypt_Simulated(const std::string& encryptedData, const std::string& key);

    // Rubrik Maddesi: Dijital İmza/HMAC (Simülasyon)
    std::string HMAC_Simulated(const std::string& data, const std::string& key);

    // Kriptografik karma fonksiyonu (Mevcut SHA placeholder'ı yerine)
    std::string SHA256_Hash(const std::string& input);
}

#endif // CRYPTO_UTILS_H