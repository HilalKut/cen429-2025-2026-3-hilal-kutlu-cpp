cmake_minimum_required(VERSION 3.14)
project(DigitalJournalProject)

set(CMAKE_CXX_STANDARD 14) # std::make_unique için gerekli

# SQLite3 ve GTest paketlerini bul
find_package(SQLite3 REQUIRED)
find_package(GTest REQUIRED)

include_directories(src/models/header src/digitaljournal/header src/crypto/header src/security/header src/ui/header)

# Kaynak dosyaları
add_executable(DigitalJournalTests 
    tests/DigitalJournalTests.cpp 
    src/digitaljournal/src/digitaljournal.cpp 
    src/digitaljournal/src/DatabaseManager.cpp
    src/crypto/src/CryptoUtils.cpp
    src/security/src/SecureUtils.cpp
    src/ui/src/UIUtils.cpp
)

target_link_libraries(DigitalJournalTests GTest::gtest_main SQLite3::SQLite3)