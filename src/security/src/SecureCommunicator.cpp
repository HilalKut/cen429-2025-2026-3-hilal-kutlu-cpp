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

bool SecureCommunicator::sendData(const std::string& data) {
    std::cout << "\n--- Guvenli Veri Gonderme Simulasyonu ---" << std::endl;
    establishTLSConnection();
    if (verifyCertificatePinning()) {
        std::cout << "[+] Veri gonderiliyor: " << data.substr(0, 20) << "..." << std::endl;
        std::cout << "[+] Veri basariyla gonderildi." << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        return true;
    } else {
        std::cout << "[-] Veri gonderilemedi. Guvenli baglanti kurulamadi." << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        return false;
    }
}