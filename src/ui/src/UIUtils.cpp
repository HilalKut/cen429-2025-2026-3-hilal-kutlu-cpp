/* src/ui/src/UIUtils.h */

#include "UIUtils.h"
#include "colors.h" // Renkleri de dahil edelim
#include <iostream>
#include <string>
#include <sstream>
#include <limits>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader(const std::string& title) {
    std::cout << "========================================" << std::endl;
    std::cout << "    " << title << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
}

int getValidatedInput(int min, int max) {
    std::string line;
    int choice;
    while (true) {
        std::cout << CYAN << ">> Seciminiz: " << RESET;
        std::getline(std::cin, line);
        
        std::stringstream ss(line);
        if (ss >> choice && ss.eof()) {
            if (choice >= min && choice <= max) {
                return choice;
            }
        }
        std::cout << RED << "Gecersiz giris. Lutfen " << min << " ile " << max << " arasinda bir sayi girin." << RESET << std::endl;
    }
}