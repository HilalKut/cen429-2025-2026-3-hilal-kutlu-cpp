/* src/security/src/SecureCommunicator.cpp */

#include "SecureCommunicator.h"
#include <iostream>

SecureCommunicator::SecureCommunicator(const std::string& host) : serverHost(host) {}

void SecureCommunicator::establishTLSConnection() {
    // SİMÜLASYON: Gerçek bir TLS bağlantısı burada kurulur.
    std::cout << "[+] TLS baglantisi baslatiliyor: " << serverHost << std::endl;
    std::cout << "[+] Baglanti sifreli." << std::endl;
}

bool SecureCommunicator::verifyCertificatePinning() {
    // SİMÜLASYON: Sunucudan gelen sertifikanın hash'i hesaplanır.
    std::string receivedCertificateHash = "EXPECTED_SERVER_CERT_HASH_HERE"; // Simülasyon için eşleşiyor.

    std::cout << "[+] Sunucu sertifikasi dogrulaniyor (Certificate Pinning)..." << std::endl;
    if (receivedCertificateHash == pinnedServerCertificateHash) {
        std::cout << "[+] Sertifika dogrulandi. Guvenli baglanti." << std::endl;
        return true;
    } else {
        std::cout << "[-] UYARI: Sertifika uyusmazligi! Man-in-the-Middle saldirisi olabilir!" << std::endl;
        return false;
    }
}

// Oturum Anahtarı Yönetimi için simülasyon
bool SecureCommunicator::performDiffieHellmanExchange() {
    // SİMÜLASYON: Gerçek bir uygulamada burada büyük asal sayılar kullanılır.
    std::cout << "[+] Diffie-Hellman anahtar degisimi baslatiliyor..." << std::endl;
    // Basit bir oturum anahtarı oluşturma simülasyonu
    sessionKey = "SESSION_KEY_" + std::to_string(rand() % 1000); 
    std::cout << "[+] Oturum Anahtari basariyla olusturuldu ve degistirildi (Oturum Anahtari Yonetimi)." << std::endl;
    return true;
}

bool SecureCommunicator::sendData(const std::string& data) {
    std::cout << "\n--- Guvenli Veri Gonderme Simulasyonu ---" << std::endl;
    establishTLSConnection();
    if (verifyCertificatePinning()) {
        if (performDiffieHellmanExchange()) { // Oturum anahtarı değişimi ekleniyor
            // Gerçek bir senaryoda, veri bu 'sessionKey' ile şifrelenirdi.
            std::cout << "[+] Veri (Oturum Anahtari ile) gonderiliyor: " << data.substr(0, 20) << "..." << std::endl;
            std::cout << "[+] Veri basariyla gonderildi." << std::endl;
            std::cout << "-----------------------------------------" << std::endl;
            return true;
        }
    }
    std::cout << "[-] Veri gonderilemedi. Guvenli baglanti kurulamadi." << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    return false;
}