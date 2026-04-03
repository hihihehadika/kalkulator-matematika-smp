cmake_minimum_required(VERSION 3.16)
project(KalkulatorMatematikaSMP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 Auto-MOC: Otomatis generate meta-object code untuk signal/slot
set(CMAKE_AUTOMOC ON)

# Cari Qt6 Modules yang dibutuhkan
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Charts Qml)

# Kumpulkan semua file sumber secara rekursif
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.h
)

# Buat executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Charts
    Qt6::Qml
)

# Include directories — agar #include "core/Theme.h" bekerja
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Windows: Sembunyikan console window saat release
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
