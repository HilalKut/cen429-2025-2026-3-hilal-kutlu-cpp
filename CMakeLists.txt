cmake_minimum_required(VERSION 3.14)
project(DigitalJournalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage -O0 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

set(CMAKE_CXX_STANDARD 14)

# SQLite3 ve GTest paketlerini bul
find_package(SQLite3 REQUIRED)
find_package(GTest REQUIRED)

# Header yollarını ekle
include_directories(
    src
    src/models/header 
    src/digitaljournal/header 
    src/crypto/header 
    src/security/header 
    src/ui/header
    ${SQLite3_INCLUDE_DIRS}
)

# Ortak kaynak dosyaları
set(MY_SOURCES
    src/digitaljournal/src/digitaljournal.cpp 
    src/digitaljournal/src/DatabaseManager.cpp
    src/crypto/src/CryptoUtils.cpp
    src/security/src/SecurityUtils.cpp
    src/ui/src/UIUtils.cpp
)

# 1. Ana Uygulama
add_executable(DigitalJournalApp ${MY_SOURCES} src/digitaljournal/src/main.cpp)
# MacBook uyumluluğu için değişken üzerinden linkleme yapıyoruz
target_link_libraries(DigitalJournalApp ${SQLite3_LIBRARIES})

# 2. Testler
add_executable(DigitalJournalTests ${MY_SOURCES} tests/DigitalJournalTests.cpp)
target_link_libraries(DigitalJournalTests GTest::gtest_main ${SQLite3_LIBRARIES})

