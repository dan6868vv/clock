#cmake_minimum_required(VERSION 3.28)
#project(Arrow_Display)
#
#set(CMAKE_CXX_STANDARD 17)
#
#add_executable(Arrow_Display main.cpp)
cmake_minimum_required(VERSION 3.10)
project(Arrow_Display)

set(CMAKE_CXX_STANDARD 17)

# Указываем путь к вашей установке raylib
set(RAYLIB_PATH "D:/_root/Programs/_Lib_c++/raylib/raylib")

add_executable(Arrow_Display main.cpp)

# Добавляем пути для заголовочных файлов (чтобы #include работали)
target_include_directories(Arrow_Display PRIVATE
        "${RAYLIB_PATH}/src"
        "D:/_root/Programs/_Lib_c++/raylib/w64devkit/x86_64-w64-mingw32/include"
)

# Ищем библиотеку raylib
# Обычно она лежит в одном из этих мест
find_library(RAYLIB_LIBRARY
        NAMES raylib libraylib
        PATHS
        "${RAYLIB_PATH}/src"
        "${RAYLIB_PATH}/lib"
        "D:/_root/Programs/_Lib_c++/raylib/w64devkit/x86_64-w64-mingw32/lib"
        REQUIRED
)

if(RAYLIB_LIBRARY)
    message(STATUS "Found raylib: ${RAYLIB_LIBRARY}")
    target_link_libraries(Arrow_Display ${RAYLIB_LIBRARY})
else()
    message(FATAL_ERROR "Raylib library not found!")
endif()

# На Windows нужны эти библиотеки
target_link_libraries(Arrow_Display winmm gdi32 opengl32)