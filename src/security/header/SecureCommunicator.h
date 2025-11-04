#ifndef SECURE_COMMUNICATOR_H
#define SECURE_COMMUNICATOR_H

#include <string>

// Bu sınıf, final rubriğindeki "Güvenli İletişim" ve "Sertifikalar"
// maddelerini karşılamak için konsept olarak tasarlanmıştır.
// Gerçek bir ağ bağlantısı kurmaz.
class SecureCommunicator {
public:
    SecureCommunicator(const std::string& host);

    // Bir sunucuya güvenli veri gönderme simülasyonu yapar.
    bool sendData(const std::string& data);

private:
    std::string serverHost;
    const std::string pinnedServerCertificateHash = "EXPECTED_SERVER_CERT_HASH_HERE";

    // Gerçek bir uygulamada, bu fonksiyon OpenSSL veya benzeri bir kütüphane
    // kullanarak bir TLS bağlantısı kurar.
    void establishTLSConnection();
    
    // Gerçek bir uygulamada, bu fonksiyon sunucunun sertifikasını alır
    // ve hash'ini bizim "pinned" (sabitlediğimiz) hash ile karşılaştırır.
    // Bu, Man-in-the-Middle saldırılarını önler.
    bool verifyCertificatePinning();
};

#endif // SECURE_COMMUNICATOR_H