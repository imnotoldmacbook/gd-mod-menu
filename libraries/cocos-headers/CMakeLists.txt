cmake_minimum_required(VERSION 3.5...3.30)

project(cocos-headers CXX)

# Generate import libraries from DLLs
# For MSVC: dumpbin -> .def -> lib.exe
# For MinGW: dlltool directly from DLL

function(generate_import_lib output_lib input_dll)
    get_filename_component(dll_name ${input_dll} NAME_WE)
    set(def_file ${CMAKE_CURRENT_BINARY_DIR}/${dll_name}.def)

    if(MSVC)
        # Step 1: Find dumpbin and lib.exe
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

        if(DUMPBIN_EXE AND LIB_EXE)
            # Step 2: Generate .def file from DLL exports
            add_custom_command(
                OUTPUT ${def_file}
                COMMAND ${DUMPBIN_EXE} /EXPORTS ${input_dll}
                    | findstr /R /C:"^[[:space:]]*[0-9A-F]*[[:space:]]*[0-9A-F]*[[:space:]]*[0-9A-F]*[[:space:]]*[A-Za-z_?@]"
                COMMAND echo "LIBRARY ${dll_name}" > ${def_file}
                COMMAND echo "EXPORTS" >> ${def_file}
                COMMAND ${DUMPBIN_EXE} /EXPORTS ${input_dll}
                    | findstr /R /C:"^[[:space:]]*[0-9A-F]*[[:space:]]*[0-9A-F]*[[:space:]]*[0-9A-F]*[[:space:]]*[A-Za-z_?@]"
                    | awk "{print \\$4}" >> ${def_file}
                DEPENDS ${input_dll}
                COMMENT "Generating .def file for ${input_dll}"
            )

            # Step 3: Generate import library from .def
            add_custom_command(
                OUTPUT ${output_lib}
                COMMAND ${LIB_EXE} /MACHINE:X86 /DEF:${def_file} /OUT:${output_lib}
                DEPENDS ${def_file}
                COMMENT "Generating import library ${output_lib} from ${def_file}"
            )
        else()
            message(WARNING "dumpbin.exe or lib.exe not found, cannot generate import library for ${input_dll}")
        endif()

    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
        # Use dlltool from MinGW - can generate directly from DLL
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