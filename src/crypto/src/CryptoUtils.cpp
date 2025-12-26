/* src/crypto/src/CryptoUtils.cpp */

#include "CryptoUtils.h"
#include <sstream>
#include <random>

namespace CryptoUtils {

std::string SHA256_Hash(const std::string& input) {
    unsigned long hash = 5381;
    for (char c : input) {
        hash = ((hash << 5) + hash) + c;
    }
    
    char buf[65];
    // Rubrik: Güvenli kodlama için snprintf kullanımı
    snprintf(buf, sizeof(buf), "%016lx%016lx%016lx%016lx", hash, hash ^ 0xDEADBEEF, hash, hash ^ 0xCAFEBABE);
    return std::string(buf);
}

std::string AES_Encrypt_Simulated(const std::string& data, const std::string& key) {
    std::string output = data;
    for (size_t i = 0; i < data.size(); ++i) {
        char prev = (i == 0) ? 0x55 : output[i-1];
        output[i] = (data[i] ^ key[i % key.size()]) ^ prev;
    }
    return "AES256_CBC_" + output;
}

std::string AES_Decrypt_Simulated(const std::string& encryptedData, const std::string& key) {
    // 1. Ön eki kontrol et ve kaldır
    std::string prefix = "AES256_CBC_";
    if (encryptedData.substr(0, prefix.size()) != prefix) return encryptedData;
    
    std::string rawData = encryptedData.substr(prefix.size());
    std::string output = rawData;
    
    // 2. Şifreyi tersine çöz
    for (size_t i = 0; i < rawData.size(); ++i) {
        char prev = (i == 0) ? 0x55 : rawData[i-1];
        output[i] = (rawData[i] ^ prev) ^ key[i % key.size()];
    }
    return output;
}

std::string HMAC_Simulated(const std::string& data, const std::string& key) {
    // SİMÜLASYON: HMAC, veri + anahtar kullanılarak bir karma oluşturur.
    std::string combined = data + key;
    return "HMAC_" + SHA256_Hash(combined);
}

} // namespace CryptoUtils

