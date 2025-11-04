enable_testing()

# Google Test entegrasyonu (CMake'in FetchContent modülü ile)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1 # İstediğiniz versiyonu seçebilirsiniz
)
FetchContent_MakeAvailable(googletest)

# DigitalJournal testleri
add_executable(digitaljournal_tests digitaljournal/digitaljournal_test.cpp)
target_link_libraries(digitaljournal_tests PRIVATE digitaljournal gtest_main) # gtest_main, gtest ve ana fonksiyonu içerir
target_include_directories(digitaljournal_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/digitaljournal/header
    ${gtest_SOURCE_DIR}/include # Google Test başlık dosyaları için
)

# SecurityUtils testleri
add_executable(security_tests security/SecurityUtils_test.cpp ../src/security/src/SecurityUtils.cpp)
target_link_libraries(security_tests PRIVATE gtest_main)
target_include_directories(security_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/security/header
)
add_test(NAME security_unit_tests COMMAND security_tests)