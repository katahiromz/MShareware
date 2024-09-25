# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.5)

# project name, version and languages
project(MyApp VERSION 0.0 LANGUAGES C CXX RC)

# statically linking (without additional DLLs)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (MSVC)
    # replace "/MD" with "/MT" (building without runtime DLLs)
    set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    foreach(CompilerFlags ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
    endforeach()
endif()

# UTF-8 support
if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

# Dialog app or not?
option(MYAPP_IS_DIALOG "MyApp is a dialog app?" ON)
if(MYAPP_IS_DIALOG)
    add_definitions(-DMYAPP_IS_DIALOG=1)
endif()

##############################################################################

# MyApp.exe
add_executable(MyApp WIN32 MyApp.cpp MyApp_res.rc mt19937ar.c
    MSha256.cpp
    MShareware.cpp
    MShareware.rc)
target_compile_definitions(MyApp PRIVATE -DUNICODE -D_UNICODE) # Unicode support
target_link_libraries(MyApp PRIVATE comctl32 comdlg32 shell32 shlwapi)
if(MSVC)
    target_link_options(MyApp PRIVATE /MANIFEST:NO) # No additional manifest
endif()


##############################################################################
