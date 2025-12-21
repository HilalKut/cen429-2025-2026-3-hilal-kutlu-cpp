# Digital Journal Güvenlik Mimarisi Özeti

## 1. Mimarinin Katmanları (Savunma Derinliği)

1.  **Dış Katman (RASP & Hardening):** Uygulama başlatılırken çalışır. (Vize Rubriği)
    *   `main.cpp`: Anti-Debug ve Tamper Detection kontrollerini başlatır.
    *   `SecurityUtils`: Opaque Predicate ve String Obfuscation içerir.
2.  **Uygulama Katmanı (`DigitalJournalApp`):** İş mantığını ve UI etkileşimini yönetir.
    *   `secureClearMemory`: Dinamik verilerin (Parola, Salt) bellekten silinmesini zorlar.
    *   `loginUser`: Parola/Salt hash'leme işlemini koordine eder.
3.  **Kritik Hizmetler Katmanı (Kripto & Veri):** En hassas işlemlerin yapıldığı katmandır.
    *   `CryptoUtils`: Tüm kriptografik işlemleri (AES Simülasyonu, SHA256, HMAC) soyutlar.
    *   `SecureCommunicator`: TLS Simülasyonu ve Sertifika Pinning gibi Güvenli İletişim mekanizmalarını uygular.

## 2. Kriptografi Mimarisi (Final Rubriği)

| Kripto Mekanizması | Rubrik Maddesi | Kullanım Amacı | Anahtar Yönetimi |
| :--- | :--- | :--- | :--- |
| **AES Simülasyonu** | AES/RSA Şifreleme | Günlük içeriğini depolama ve iletim için şifreleme. | Kullanıcının salted-hashed parolası (güçlendirilmiş anahtar) kullanılır. |
| **SHA256 Simülasyonu** | Dijital İmza/HMAC | Parola hash'leme ve HMAC ile veri bütünlüğü doğrulama. | |
| **HMAC Simülasyonu** | Dijital İmza/HMAC | İletilen verinin (konsept) bütünlüğünü ve kaynağını doğrular. | |
| **Diffie-Hellman Simülasyonu**| Oturum Anahtarı Yönetimi | `SecureCommunicator` içinde güvenli oturum anahtarı oluşturma. | Geçici oturum anahtarı oluşturulur, iş bittikten sonra imha edilir. |
| **Sertifika Pinning** | Güvenli İletişim | Sunucu sertifikasının (konsept) Man-in-the-Middle saldırılarına karşı doğrulanması. | Sertifika Hash'i sabit (pinned) olarak kodlanmıştır. |

## 3. Kod Sertleştirme Mimarisi

*   **Veri Koruması:** Bellek koruması (`secureClearMemory`) ile parolaların döküm saldırılarına karşı korunması.
*   **Kontrol Akışı:** Statik analizi zorlaştırmak için `isAlwaysTrue` gibi Opaque Predicate'lar kullanılır.