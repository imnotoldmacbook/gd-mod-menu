cmake_minimum_required(VERSION 3.5...3.30)
set(CMAKE_CXX_STANDARD 17)

project(gd-mod-menu)

# ============================================================
# Import library generation from GD DLLs (at configure time)
# ============================================================
set(COCOS2D_DLL ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/libcocos2d.dll)
set(EXTENSIONS_DLL ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/libExtensions.dll)

set(COCOS2D_LIB ${CMAKE_BINARY_DIR}/libraries/cocos-headers/libcocos2d.lib)
set(EXTENSIONS_LIB ${CMAKE_BINARY_DIR}/libraries/cocos-headers/libExtensions.lib)

# Generate import libraries at CMake configure time (since DLLs are in repo)
if(MSVC)
    find_program(DUMPBIN_EXE dumpbin.exe
        PATHS "$ENV{VCToolsInstallDir}/bin/Hostx86/x86"
        "$ENV{VCToolsInstallDir}/bin/HostX64/x86"
        NO_DEFAULT_PATH
    )
    find_program(LIB_EXE lib.exe
        PATHS "$ENV{VCToolsInstallDir}/bin/Hostx86/x86"
        "$ENV{VCToolsInstallDir}/bin/HostX64/x86"
        NO_DEFAULT_PATH
    )

    if(DUMPBIN_EXE AND LIB_EXE AND EXISTS ${COCOS2D_DLL} AND EXISTS ${EXTENSIONS_DLL})
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libraries/cocos-headers)

        # Generate libcocos2d.lib
        execute_process(
            COMMAND ${DUMPBIN_EXE} /EXPORTS ${COCOS2D_DLL}
            OUTPUT_VARIABLE COCOS2D_EXPORTS
            RESULT_VARIABLE DUMPBIN_RESULT
        )
        if(NOT DUMPBIN_RESULT)
            file(WRITE ${CMAKE_BINARY_DIR}/libraries/cocos-headers/libcocos2d.def
                "LIBRARY libcocos2d\nEXPORTS\n")
            # Parse exports - lines like "    1    0 00012345 FunctionName = @ILT+xxx(???FunctionName)"
            string(REGEX MATCHALL "[A-Za-z_?@][A-Za-z0-9_@]*" EXPORT_NAMES "${COCOS2D_EXPORTS}")
            foreach(name ${EXPORT_NAMES})
                # Filter out ordinals and internal names
                if(NOT name MATCHES "^[0-9]+$" AND NOT name MATCHES "^__.*" AND name MATCHES "[A-Za-z]")
                    file(APPEND ${CMAKE_BINARY_DIR}/libraries/cocos-headers/libcocos2d.def "${name}\n")
                endif()
            endforeach()
            execute_process(
                COMMAND ${LIB_EXE} /MACHINE:X86 /DEF:${CMAKE_BINARY_DIR}/libraries/cocos-headers/libcocos2d.def /OUT:${COCOS2D_LIB}
                RESULT_VARIABLE LIB_RESULT
            )
            if(NOT LIB_RESULT)
                message(STATUS "Generated ${COCOS2D_LIB}")
            else()
                message(WARNING "Failed to generate ${COCOS2D_LIB}")
            endif()
        endif()

        # Generate libExtensions.lib
        execute_process(
            COMMAND ${DUMPBIN_EXE} /EXPORTS ${EXTENSIONS_DLL}
            OUTPUT_VARIABLE EXT_EXPORTS
            RESULT_VARIABLE DUMPBIN_RESULT2
        )
        if(NOT DUMPBIN_RESULT2)
            file(WRITE ${CMAKE_BINARY_DIR}/libraries/cocos-headers/libExtensions.def
                "LIBRARY libExtensions\nEXPORTS\n")
            string(REGEX MATCHALL "[A-Za-z_?@][A-Za-z0-9_@]*" EXPORT_NAMES "${EXT_EXPORTS}")
            foreach(name ${EXPORT_NAMES})
                if(NOT name MATCHES "^[0-9]+$" AND NOT name MATCHES "^__.*" AND name MATCHES "[A-Za-z]")
                    file(APPEND ${CMAKE_BINARY_DIR}/libraries/cocos-headers/libExtensions.def "${name}\n")
                endif()
            endforeach()
            execute_process(
                COMMAND ${LIB_EXE} /MACHINE:X86 /DEF:${CMAKE_BINARY_DIR}/libraries/cocos-headers/libExtensions.def /OUT:${EXTENSIONS_LIB}
                RESULT_VARIABLE LIB_RESULT2
            )
            if(NOT LIB_RESULT2)
                message(STATUS "Generated ${EXTENSIONS_LIB}")
            else()
                message(WARNING "Failed to generate ${EXTENSIONS_LIB}")
            endif()
        endif()
    else()
        message(WARNING "dumpbin.exe or lib.exe not found, or DLLs missing - import libraries will not be generated")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
    find_program(DLLTOOL dlltool)
    if(DLLTOOL AND EXISTS ${COCOS2D_DLL} AND EXISTS ${EXTENSIONS_DLL})
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libraries/cocos-headers)
        execute_process(
            COMMAND ${DLLTOOL} -d ${COCOS2D_DLL} -l ${COCOS2D_LIB}
            RESULT_VARIABLE RESULT
        )
        if(NOT RESULT)
            message(STATUS "Generated ${COCOS2D_LIB}")
        endif()
        execute_process(
            COMMAND ${DLLTOOL} -d ${EXTENSIONS_DLL} -l ${EXTENSIONS_LIB}
            RESULT_VARIABLE RESULT2
        )
        if(NOT RESULT2)
            message(STATUS "Generated ${EXTENSIONS_LIB}")
        endif()
    else()
        message(WARNING "dlltool not found or DLLs missing")
    endif()
endif()

# ============================================================
# Main target
# ============================================================
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

# Link against generated import libraries
target_link_libraries(${PROJECT_NAME}
  minhook
  cocos2d
  ${COCOS2D_LIB}
  ${EXTENSIONS_LIB}
)

# MinGW-specific linking
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "MinGW")
  target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif()