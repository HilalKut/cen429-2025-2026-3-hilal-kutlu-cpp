// tests/TestMain.cpp

#include "gtest/gtest.h"

// Tüm testleri çalıştırmak için GTest'in varsayılan main fonksiyonunu kullanır.
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}