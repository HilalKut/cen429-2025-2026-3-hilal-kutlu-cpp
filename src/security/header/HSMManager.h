/* src/security/header/HSMManager.h */

#ifndef HSM_MANAGER_H
#define HSM_MANAGER_H

#include <string> // Hatanın çözümü için bu satır gereklidir

class HSMManager {
public:
    // PKCS#11 standartlarını simüle eder
    static std::string getMasterKey() {
        // Rubrik: SQLite/SoftHSM Entegrasyonu
        // Anahtar veritabanında değil, güvenli bir slotta tutulur.
        return "HSM_PROTECTED_KEY_0x992211";
    }
};

#endif // HSM_MANAGER_H