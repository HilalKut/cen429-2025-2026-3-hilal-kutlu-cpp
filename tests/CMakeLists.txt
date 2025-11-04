# Google Test'i projeye dahil et (FetchContent ile)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# --- Test Hedeflerini Oluşturma ---

# Test için çalıştırılabilir bir hedef oluştur.
# EXCLUDE_FROM_ALL parametresi, normal bir "build" sırasında derlenmesini engeller,
# ancak testler çalıştırılacağı zaman derlenmesini sağlar.
add_executable(
    digitaljournal_tests 
    digitaljournal/digitaljournal_test.cpp
)

# Test programını, test ettiği 'digitaljournal' kütüphanesine ve Google Test'e bağla.
target_link_libraries(
    digitaljournal_tests 
    PRIVATE 
    digitaljournal 
    gtest_main
)

# Test programının, gerekli başlık dosyalarını bulabilmesi için yolları ekledim.
target_include_directories(
    digitaljournal_tests 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/digitaljournal/header
    ${googletest_SOURCE_DIR}/googletest/include
)

# --- Testleri CTest'e Tanımlama ---

# Testleri etkinleştir. Bu, CTest'in çalışması için gereklidir.
enable_testing()

# Bu komut, CTest'e testin nasıl çalıştırılacağını söyler.
add_test(
    NAME DigitalJournalApp.UnitTests 
    COMMAND digitaljournal_tests
)