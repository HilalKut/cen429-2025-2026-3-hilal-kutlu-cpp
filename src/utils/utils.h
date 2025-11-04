#ifndef UTILS_H
#define UTILS_H

#include <string>

// Terminal ekranını temizler
void clearScreen();

// Başlık biçiminde metin yazdırır
void printHeader(const std::string& title);

// Belirtilen aralıkta kullanıcıdan geçerli bir sayı alır
int getValidatedInput(int min, int max);

#endif
