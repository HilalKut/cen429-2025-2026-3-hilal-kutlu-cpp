# DigitalJournal kütüphanesi
add_library(digitaljournal SHARED
    digitaljournal/src/digitaljournal.cpp
    digitaljournal/header/digitaljournal.h
)
target_include_directories(digitaljournal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/digitaljournal/header
)

# Utility kütüphanesi (varsa)
# add_library(utility STATIC
#     utility/src/utility.cpp
#     utility/header/utility.h
# )
# target_include_directories(utility PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/utility/header
# )

# Ana uygulama (EXE)
add_executable(DigitalJournalApp main.cpp
    # Buraya ana uygulamanızın .cpp dosyalarını ekleyin, örneğin main.cpp
    # Örneğin, src/main.cpp adında bir dosyanız varsa:
    # main.cpp
)
target_link_libraries(DigitalJournalApp PRIVATE digitaljournal) # Utility varsa onu da ekleyin
target_include_directories(DigitalJournalApp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/digitaljournal/header
    # ${CMAKE_CURRENT_SOURCE_DIR}/utility/header # Utility varsa
)