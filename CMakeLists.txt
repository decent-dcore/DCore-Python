if (POLICY CMP0025)
    # Compiler id for Apple Clang is now AppleClang
    cmake_policy(SET CMP0025 NEW)
endif()

if (POLICY CMP0048)
    # The ``project()`` command manages VERSION variables.
    cmake_policy(SET CMP0048 NEW)
endif()

cmake_minimum_required( VERSION 3.12.0 FATAL_ERROR )
project( DCore-Python VERSION 1.5.0 LANGUAGES CXX )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.4 )
        message( FATAL_ERROR "GCC version must be at least 7.4" )
    endif()
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0 )
        message( FATAL_ERROR "Clang version must be at least 7.0" )
    endif()
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.10 )
        message( FATAL_ERROR "MSVC version must be at least 19.10" )
    endif()
endif()

# Compile in C++14 mode
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED YES )
set( CMAKE_EXPORT_COMPILE_COMMANDS YES )

if( "${VCPKG_TARGET_TRIPLET}" MATCHES "windows-static" )
    foreach( flag_var FLAGS FLAGS_DEBUG FLAGS_RELEASE FLAGS_MINSIZEREL FLAGS_RELWITHDEBINFO )
        set( cxx_flag_var "CMAKE_CXX_${flag_var}" )
        if(${cxx_flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${cxx_flag_var} "${${cxx_flag_var}}")
        endif()
    endforeach(flag_var)
endif()

if( UNIX )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
    set( Boost_USE_STATIC_LIBS OFF CACHE BOOL "Use shared Boost libraries" )
endif()

add_subdirectory( src )
