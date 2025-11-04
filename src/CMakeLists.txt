# DigitalJournal kütüphanesi (DLL için SHARED)
add_library(digitaljournal SHARED
    digitaljournal/src/digitaljournal.cpp
    digitaljournal/header/digitaljournal.h
    
    digitaljournal/src/DatabaseManager.cpp
    digitaljournal/header/DatabaseManager.h

    security/src/SecurityUtils.cpp
    security/header/SecurityUtils.h

    security/src/SecureCommunicator.cpp
    security/header/SecureCommunicator.h

    ui/header/colors.h

    ui/src/UIUtils.cpp
    ui/header/UIUtils.h

)

# Kütüphanenin başlık dosyalarını (header) nerede bulacağını belirtiyoruz.
target_include_directories(digitaljournal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/digitaljournal/header
    ${CMAKE_CURRENT_SOURCE_DIR}/security/header  
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/header

    ${CMAKE_BINARY_DIR}/_deps/googletest-src/googletest/include
    ${CMAKE_BINARY_DIR}/_deps/googletest-src/googlemock/include 
    ${CMAKE_CURRENT_SOURCE_DIR}  
)

# SQLite3 kütüphanesini bul
find_package(SQLite3 REQUIRED)

# Kütüphanenin başlık dosyalarını belirt
target_include_directories(digitaljournal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/digitaljournal/header
    ${CMAKE_CURRENT_SOURCE_DIR}/security/header
)

# SQLite3 kütüphanesini digitaljournal kütüphanemize bağla
target_link_libraries(digitaljournal PRIVATE SQLite::SQLite3)

# Ana uygulama (EXE)
add_executable(DigitalJournalApp main.cpp)

# Uygulamayı (DigitalJournalApp) oluşturduğumuz kütüphaneye (digitaljournal) bağla
target_link_libraries(DigitalJournalApp PRIVATE digitaljournal)