cmake_minimum_required(VERSION 3.10)
project(CEN429_DigitalJournalApp LANGUAGES CXX)

# --- Code Coverage Ayarları ---
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    message(STATUS "Code coverage build enabled for Clang")
    # -fprofile-instr-generate ve -fcoverage-mapping bayrakları,
    # modern Clang'in kapsama verisi üretmesi için gereklidir.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -O0 -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 

# Bu satırları ekleyelim veya güncelleyelim:
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # Çalıştırılabilir dosyalar (exe) buraya gitsin
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Kütüphaneler (dll, so) buraya gitsin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # Arşivler (a) buraya gitsin

# --- Testleri Etkinleştirme ---
# Bu komut, CMake'in testleri keşfetmesini ve ctest için
# gerekli yapılandırma dosyalarını oluşturmasını sağlar.
enable_testing()
add_subdirectory(tests)
# src ve tests alt dizinlerini dahil et
add_subdirectory(src)