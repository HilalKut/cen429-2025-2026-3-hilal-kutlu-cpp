#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <string>

void clearScreen();
void printHeader(const std::string& title);
int getValidatedInput(int min, int max);

#endif // UI_UTILS_H