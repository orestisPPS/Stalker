# Centralized module helpers for header-only Stalker

include(CMakeParseArguments)

# Helper: check if a module is selected via STALKER_MODULES list
function(stalker_modules_is_selected NAME OUT_VAR)
    set(_list ${STALKER_MODULES})
    set(_enabled 0)
    if(NOT _list)
        # Default to all modules if list is empty
        set(_enabled 1)
    else()
        list(FIND _list ${NAME} _idx)
        if(_idx GREATER -1)
            set(_enabled 1)
        endif()
        # Also allow special value "all"
        list(FIND _list all _idx_all)
        if(_idx_all GREATER -1)
            set(_enabled 1)
        endif()
    endif()
    set(${OUT_VAR} ${_enabled} PARENT_SCOPE)
endfunction()

function(stalker_module_selected NAME OUT_VAR)
    stalker_modules_is_selected(${NAME} ${OUT_VAR})
endfunction()

# Define canonical dependency map variables for modules.
# Usage: stalker_define_dependency_map()
function(stalker_modules_define_deps)
    # Clear existing (idempotent)
    set(_STALKER_MODULE_DEPS_Core "" PARENT_SCOPE)
    set(_STALKER_MODULE_DEPS_Memory "Core" PARENT_SCOPE)
    set(_STALKER_MODULE_DEPS_Utility "Core" PARENT_SCOPE)
    set(_STALKER_MODULE_DEPS_Containers "Core" PARENT_SCOPE)
    set(_STALKER_MODULE_DEPS_Mathematics "Memory" PARENT_SCOPE)
    set(_STALKER_MODULE_DEPS_Threading "Memory" PARENT_SCOPE)
    set(_STALKER_ALL_MODULES "Core;Memory;Utility;Containers;Mathematics;Threading" PARENT_SCOPE)
endfunction()

function(stalker_define_dependency_map)
    stalker_modules_define_deps()
endfunction()

# Expand requested module list with transitive dependencies.
# INPUT: REQUESTED list (may be empty -> all), dependency vars _STALKER_MODULE_DEPS_<Module>
# OUTPUT: OUT_VAR expanded unique ordered list.
function(stalker_modules_expand REQUESTED OUT_VAR)
    if(NOT DEFINED _STALKER_ALL_MODULES)
        message(FATAL_ERROR "stalker_expand_modules: dependency map not defined; call stalker_define_dependency_map() first")
    endif()
    set(_requested "${REQUESTED}")
    if(_requested STREQUAL "")
        set(_effective ${_STALKER_ALL_MODULES})
    else()
        set(_effective)
        foreach(m IN LISTS _requested)
            list(APPEND _effective ${m})
            # Walk dependency chain
            set(_cursor ${m})
            while(NOT _cursor STREQUAL "")
                if(DEFINED _STALKER_MODULE_DEPS_${_cursor} AND NOT _STALKER_MODULE_DEPS_${_cursor} STREQUAL "")
                    set(_dep ${_STALKER_MODULE_DEPS_${_cursor}})
                    list(FIND _effective ${_dep} _exists)
                    if(_exists EQUAL -1)
                        list(APPEND _effective ${_dep})
                    endif()
                    set(_cursor ${_dep})
                else()
                    set(_cursor "")
                endif()
            endwhile()
        endforeach()
        list(REMOVE_DUPLICATES _effective)
    endif()
    set(${OUT_VAR} "${_effective}" PARENT_SCOPE)
endfunction()

function(stalker_expand_modules REQUESTED OUT_VAR)
    stalker_modules_expand("${REQUESTED}" ${OUT_VAR})
endfunction()

# Realize (create) expanded module targets (excluding Core which should already exist).
function(stalker_modules_realize MODULE_LIST)
    foreach(m IN LISTS MODULE_LIST)
        if(m STREQUAL "Core")
            continue()
        endif()
        if(DEFINED _STALKER_MODULE_DEPS_${m} AND NOT _STALKER_MODULE_DEPS_${m} STREQUAL "")
            set(_dep_mod ${_STALKER_MODULE_DEPS_${m}})
            if(_dep_mod STREQUAL "Core")
                set(_dep_tgt StalkerCore)
            else()
                set(_dep_tgt "Stalker${_dep_mod}")
            endif()
            stalker_define_header_module(NAME ${m} DEPENDS ${_dep_tgt})
        else()
            stalker_define_header_module(NAME ${m})
        endif()
    endforeach()
endfunction()

function(stalker_realize_modules MODULE_LIST)
    stalker_modules_realize("${MODULE_LIST}")
endfunction()

# Define a header-only module target with standard include/install and dependency wiring
# Usage: stalker_define_header_module(NAME <Name> [DEPENDS <t1;...>] [INSTALL_SUBDIR <rel/path>])
function(stalker_module_define_header)
    set(options)
    set(oneValueArgs NAME INSTALL_SUBDIR)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(SDM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if(NOT SDM_NAME)
        message(FATAL_ERROR "stalker_module_define_header: NAME is required")
    endif()

    set(_tgt Stalker${SDM_NAME})
    if(TARGET ${_tgt})
        return()
    endif()

    add_library(${_tgt} INTERFACE)

    # Standard include directories from project root
    target_include_directories(${_tgt} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # Wire dependencies if any
    if(SDM_DEPENDS)
        target_link_libraries(${_tgt} INTERFACE ${SDM_DEPENDS})
    endif()

    # Install headers for this module if an install subdir is provided
    if(SDM_INSTALL_SUBDIR)
        # Install given relative include subdirectory under include/
        install(
            DIRECTORY ${PROJECT_SOURCE_DIR}/include/${SDM_INSTALL_SUBDIR}
            DESTINATION include
        )
    else()
        # Default: install specific module dir under include/Stalker/<NAME>
        install(
            DIRECTORY ${PROJECT_SOURCE_DIR}/include/Stalker/${SDM_NAME}
            DESTINATION include/Stalker
        )
    endif()

    # Export this target for package consumers
    install(TARGETS ${_tgt}
        EXPORT StalkerTargets
        INCLUDES DESTINATION include
    )
endfunction()

function(stalker_define_header_module)
    stalker_module_define_header(${ARGN})
endfunction()
