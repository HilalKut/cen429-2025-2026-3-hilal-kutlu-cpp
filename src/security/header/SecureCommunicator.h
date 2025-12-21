/* src/security/header/SecureCommunicator.h */

#ifndef SECURE_COMMUNICATOR_H
#define SECURE_COMMUNICATOR_H

#include <string>

class SecureCommunicator {
public:
    SecureCommunicator(const std::string& host);

    // Bir sunucuya güvenli veri gönderme simülasyonu yapar.
    bool sendData(const std::string& data);

private:
    std::string serverHost;
    const std::string pinnedServerCertificateHash = "EXPECTED_SERVER_CERT_HASH_HERE";
    std::string sessionKey; // Oturum anahtarını tutmak için yeni üye

    void establishTLSConnection();
    bool verifyCertificatePinning();

    // Rubrik Maddesi: Oturum Anahtarı Yönetimi (Simülasyon)
    bool performDiffieHellmanExchange();
};

#endif // SECURE_COMMUNICATOR_H