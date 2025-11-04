#ifndef SECURITY_UTILS_H
#define SECURITY_UTILS_H

#include <string>
#include <vector>

namespace SecurityUtils {

    // --- RASP: Anti-Debug ---
    // Uygulamanın bir debugger'a bağlı olup olmadığını kontrol eder.
    void detectDebugger();

    // --- RASP: Tamper Detection / Checksum ---
    // Uygulamanın kendi dosyasının bütünlüğünü kontrol eder.
    // `argv0` main fonksiyonundan gelen program yoludur.
    void verifyAppIntegrity(const char* appPath);
    std::string calculateSHA256(const std::string& filePath); // Yardımcı fonksiyon

    // --- Kod Sertleştirme: String Gizleme ---
    // Derlenmiş dosyadaki string'leri gizlemek için basit bir yöntem.
    std::string getObfuscatedString(const std::vector<char>& obfuscatedChars);

    // --- Kod Sertleştirme: Kontrol Akışı Gizleme (Opaque Predicate) ---
    // Statik analizi zorlaştırmak için her zaman doğru olan ama karmaşık görünen bir koşul.
    bool isAlwaysTrue();

} // namespace SecurityUtils

#endif // SECURITY_UTILS_H