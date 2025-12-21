#include "digitaljournal/header/digitaljournal.h"
#include "security/header/SecurityUtils.h"
#include "ui/header/colors.h" 
#include <iostream>
#include <string> 
#include <sstream> 
#include "ui/header/UIUtils.h"

int main(int argc, char* argv[]) {
    // --- GÜVENLİK KATMANI 1: RASP (Runtime Application Self-Protection) ---
    // Uygulama, çalışmaya başlamadan önce kendini koruma altına alır.
    
    // RASP - Anti-Debugging: Bir hata ayıklayıcının varlığını kontrol eder.
    // Rubrik Maddesi: RASP Teknikleri, Anti-debug mekanizmaları
    SecurityUtils::detectDebugger();

    // RASP - Tamper Detection: Uygulamanın kendi dosya bütünlüğünü (checksum) doğrular.
    // Rubrik Maddesi: RASP Teknikleri, Checksum doğrulama, Tamper tespiti
    SecurityUtils::verifyAppIntegrity(argv[0]);

    // --- GÜVENLİK KATMANI 2: KOD SERTLEŞTİRME (Code Hardening) ---
    // Tersine mühendisliği ve statik analizi zorlaştıran teknikler.

    // Kod Sertleştirme - Opaque Predicate: Analizi zorlaştıran, sonucu belli olan karmaşık koşul.
    // Rubrik Maddesi: Kod Sertleştirme, Kontrol akışı gizleme
    if (SecurityUtils::isAlwaysTrue()) {
         std::cout << "Guvenlik kontrolleri tamamlandi." << std::endl;
    }

    // Kod Sertleştirme - Opaque Loop: Anlamsız ama analizi yavaşlatan karmaşık döngü.
    // Rubrik Maddesi: Kod Sertleştirme, Opaque Loops
    long dummy_counter = 0;
    for (int i = 0; i < 5; i++) {
        if ((i * i + 10) % 2 == 0) dummy_counter += i; else dummy_counter -= i;
    }
    
    // Uygulama ana mantığı başlar
    DigitalJournalApp app("journal.db"); 
    app.run();
    
    return 0;
}