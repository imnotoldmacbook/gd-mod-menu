cmake_minimum_required(VERSION 3.5...3.30)
set(CMAKE_CXX_STANDARD 17)

project(gd-mod-menu)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Force 32-bit compilation
set(CMAKE_SIZEOF_VOID_P 4)

# Compiler-specific flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "MinGW")
  # MinGW-w64 32-bit
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
  add_definitions(-D_WIN32 -DWIN32 -D_CRT_SECURE_NO_WARNINGS)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Clang targeting MSVC (Windows native)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target i386-pc-windows-msvc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target i386-pc-windows-msvc")
  add_definitions("--target=i386-pc-windows-msvc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC 32-bit
  add_definitions(-D_WIN32 -DWIN32 -D_CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  libraries/minhook/include
  libraries/gd.h/include
  libraries/gd.h/
  src/
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/cocos-headers)

target_link_libraries(${PROJECT_NAME} minhook cocos2d)

# MinGW-specific linking
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "MinGW")
  target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif()