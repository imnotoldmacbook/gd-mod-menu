cmake_minimum_required(VERSION 3.5...3.30)

project(cocos-headers CXX)

# Generate import libraries from DLLs using lib.exe (MSVC) or dlltool (MinGW)
# This allows linking against GD's libcocos2d.dll and libExtensions.dll

function(generate_import_lib output_lib input_dll)
    if(MSVC)
        # Use lib.exe from Visual Studio
        find_program(LIB_EXE lib.exe
            PATHS "$ENV{VCToolsInstallDir}/bin/Hostx86/x86"
            "$ENV{VCToolsInstallDir}/bin/HostX64/x86"
            "$ENV{WindowsSdkDir}/bin"
            NO_DEFAULT_PATH
        )
        if(LIB_EXE)
            add_custom_command(
                OUTPUT ${output_lib}
                COMMAND ${LIB_EXE} /MACHINE:X86 /DEF:${input_dll} /OUT:${output_lib}
                DEPENDS ${input_dll}
                COMMENT "Generating import library ${output_lib} from ${input_dll}"
            )
        else()
            message(WARNING "lib.exe not found, cannot generate import library for ${input_dll}")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
        # Use dlltool from MinGW
        find_program(DLLTOOL dlltool)
        if(DLLTOOL)
            add_custom_command(
                OUTPUT ${output_lib}
                COMMAND ${DLLTOOL} -d ${input_dll} -l ${output_lib}
                DEPENDS ${input_dll}
                COMMENT "Generating import library ${output_lib} from ${input_dll}"
            )
        else()
            message(WARNING "dlltool not found, cannot generate import library for ${input_dll}")
        endif()
    endif()
endfunction()

# Generate import libraries
set(COCOS2D_DLL ${CMAKE_CURRENT_LIST_DIR}/libcocos2d.dll)
set(EXTENSIONS_DLL ${CMAKE_CURRENT_LIST_DIR}/libExtensions.dll)

set(COCOS2D_LIB ${CMAKE_CURRENT_BINARY_DIR}/libcocos2d.lib)
set(EXTENSIONS_LIB ${CMAKE_CURRENT_BINARY_DIR}/libExtensions.lib)

generate_import_lib(${COCOS2D_LIB} ${COCOS2D_DLL})
generate_import_lib(${EXTENSIONS_LIB} ${EXTENSIONS_DLL})

# Create imported targets for the generated libraries
add_library(cocos2d_imported SHARED IMPORTED)
set_target_properties(cocos2d_imported PROPERTIES
    IMPORTED_IMPLIB ${COCOS2D_LIB}
    IMPORTED_LOCATION ${COCOS2D_DLL}
)

add_library(extensions_imported SHARED IMPORTED)
set_target_properties(extensions_imported PROPERTIES
    IMPORTED_IMPLIB ${EXTENSIONS_LIB}
    IMPORTED_LOCATION ${EXTENSIONS_DLL}
)

# Main interface library with includes
add_library(cocos2d INTERFACE)

target_include_directories(cocos2d INTERFACE
  cocos2dx
  cocos2dx/include
  cocos2dx/kazmath/include
  cocos2dx/platform/win32
  cocos2dx/platform/third_party/win32
  cocos2dx/platform/third_party/win32/OGLES
  extensions
)

# Link the imported libraries
target_link_libraries(cocos2d INTERFACE cocos2d_imported extensions_imported)

# Ensure import libs are built before main target
add_custom_target(generate_import_libs DEPENDS ${COCOS2D_LIB} ${EXTENSIONS_LIB})
add_dependencies(cocos2d generate_import_libs)