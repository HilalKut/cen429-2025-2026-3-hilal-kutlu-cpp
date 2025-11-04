#include "SecurityUtils.h"
#include <iostream>
#include <fstream>
#include <vector>

// Platforma özgü kütüphaneler
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ptrace.h> // Linux için
#include <unistd.h>     // getpid() için
#endif

#if defined(__APPLE__) || defined(__MACH__)
#include <sys/sysctl.h> // sysctl için (sadece macOS)
#endif


// --- RASP: Anti-Debug ---
void SecurityUtils::detectDebugger() {
    bool isDebugging = false;

    #if defined(_WIN32)
    // --- Windows Yöntemi ---
    if (IsDebuggerPresent()) {
        isDebugging = true;
    }

    #elif defined(__APPLE__) || defined(__MACH__)
    // --- macOS Yöntemi ---
    int mib[4]; // Management Information Base
    struct kinfo_proc info;
    size_t size = sizeof(info);

    // info yapısını sıfırla
    info.kp_proc.p_flag = 0;

    // Sorgumuzu hazırlıyoruz: Kernel'den, process listesinden,
    // kendi process ID'mize ait bilgileri istiyoruz.
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid(); // Mevcut process'in ID'sini al

    // sysctl çağrısını yap ve process bilgilerini al
    if (sysctl(mib, 4, &info, &size, NULL, 0) == 0) {
        // Eğer process'in flag'leri arasında "P_TRACED" (izleniyor) varsa,
        // bu bir debugger'a bağlı olduğunu gösterir.
        if ((info.kp_proc.p_flag & P_TRACED) != 0) {
            isDebugging = true;
        }
    }

    #elif defined(__linux__)
    // --- Linux Yöntemi ---
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        isDebugging = true;
    }
    #endif // Platform seçimi sonu

    if (isDebugging) {
        std::cerr << "Debugger tespit edildi! Uygulama sonlandiriliyor." << std::endl;
        exit(EXIT_FAILURE); // Programı anında sonlandır
    }
}


// --- RASP: Tamper Detection / Checksum ---
std::string SecurityUtils::calculateSHA256(const std::string& filePath) {
    std::string command = "shasum -a 256 \"" + filePath + "\"";
    std::string result = "";
    std::array<char, 128> buffer;

    // Komutu çalıştır ve çıktısını oku
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        return "popen_failed";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result.substr(0, 64);
}

void SecurityUtils::verifyAppIntegrity(const char* appPath) {
    const std::string originalHash = "4c3f9b56a023bc1b4165f25c0b8008b77b987fa01e547f0d01aecb0ea839d858";

    std::string currentHash = calculateSHA256(appPath);

    if (originalHash != currentHash) {
        std::cerr << "Uygulama butunlugu bozulmus! Program sonlandiriliyor." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// --- Kod Sertleştirme: String Gizleme ---
std::string SecurityUtils::getObfuscatedString(const std::vector<char>& obfuscatedChars) {
    std::string temp = "";
    for (char c : obfuscatedChars) {
        temp += (c ^ 0xAA);
    }
    return temp;
}

// --- Kod Sertleştirme: Kontrol Akışı Gizleme (Opaque Predicate) ---
bool SecurityUtils::isAlwaysTrue() {
    volatile int x = 10;
    return (x * x) >= 0;
}