/* src/crypto/header/WhiteboxCrypto.h */

#ifndef WHITEBOX_CRYPTO_H
#define WHITEBOX_CRYPTO_H

#include <string>

namespace Whitebox {
    // Rubrik Maddesi: Whitebox Kriptografi (Konsept)
    // Gerçek Whitebox, korunan bir anahtarla oluşturulan karmaşık tablolardan oluşur.

    std::string whiteboxEncrypt(const std::string& data);
    std::string whiteboxDecrypt(const std::string& encryptedData);
}

#endif // WHITEBOX_CRYPTO_H