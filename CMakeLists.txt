cmake_minimum_required(VERSION 3.10)
project(CEN429_DigitalJournalApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # DLL için gerekli

# Bu satırları ekleyelim veya güncelleyelim:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Çalıştırılabilir dosyalar (exe) buraya gitsin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Kütüphaneler (dll, so) buraya gitsin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Arşivler (a) buraya gitsin

# src ve tests alt dizinlerini dahil et
add_subdirectory(src)