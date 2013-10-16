cmake_minimum_required(VERSION 2.8.8)

option(BUILD_THIRD_PARTY_LIBRARIES "Set to ON to automatically build included 3rd-party libraries" OFF)

# To force which compiler to use, e.g. for me for clang instead of gcc, pass CC and CCX=/usr/bin/clang++ cmake .. ; ...

project(ore-infinium)

include(FeatureSummary)

# Enable debug symbols by default
#
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
#'-msse', '-Wconversion', '-Wno-variadic-macros', '-Wuninitialized', '-Wcast-align', '-Wredundant-decls', '-Wno-unknown-pragmas', '-funsigned-char', '-fdata-sections', '-ffunction-sections', '-pedantic', '-Wextra', '-Weffc++', '-Wno-long-long', '-Wold-style-cast', '-Wstrict-overflow', '-Woverloaded-virtual', '-Wsign-promo', '-Wabi', '-Wmissing-declarations', '-Wall'
    set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -g3")
# -Wall -Wno-unused-variable -Wno-unused-function -Woverloaded-virtual -g3 -ggdb -ftrapv -fverbose-asm -Wextra -Wfloat-equal -Winit-self -Wuninitialized -Werror-implicit-function-declaration -Wundef -Wshadow  -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wswitch-default -Wconversion -Wno-switch -Wunreachable-code -Wformat=2 -fstack-check -Wdouble-promotion
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -g3 -ggdb -fno-exceptions")

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -O0 -g3 -Wmismatched-tags -Wswitch -Wuninitialized -fdiagnostics-show-category=name -fdiagnostics-show-template-tree -Wextra-tokens -Wextra -Wvolatile-register-var -Wunused-label -Wswitch -Wstrict-overflow=1 -Wsign-compare -Wreturn-type -Wreorder -Wpointer-sign -Wparentheses -Wnonnull -Wcomment -Wno-unused-parameter -fno-exceptions")
# -fsanitize=integer -fsanitize=undefined")

#set (SANITIZER_FLAGS  " -fsanitize=integer -fsanitize=undefined")

#set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS})
endif() 
else(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations -Woverloaded-virtual")
    set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -fexpensive-optimizations")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Set version information in a config.h file
set(ore_infinium_VERSION_MAJOR 0)
set(ore_infinium_VERSION_MINOR 1)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib64 ${CMAKE_INSTALL_PREFIX}/lib)


  # Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)
  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
#  set(CMAKE_INCLUDE_CURRENT_DIR ON)


####### # Executables fail to build with Qt 5 in the default configuration
#######  # without -fPIE. We add that here.
#######  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

include(FindProtobuf)

find_package(SDL2)

find_package(Qt5Quick)

find_package(OpenGL)

find_package(SDL_image)

find_package(YamlCpp)

find_package(Chipmunk)

find_package(enet)
find_package(Protobuf)
find_package(LibNoise)

find_package(ZLIB)

set_package_properties(SDL2 PROPERTIES DESCRIPTION "SDL2" URL "http://hg.libsdl.org/SDL/")
set_package_properties(SDL2 PROPERTIES TYPE REQUIRED PURPOSE "Used for setting up an OpenGL 3.0+ context as well as for input.")

set_package_properties(OpenGL PROPERTIES DESCRIPTION "OpenGL >= 3.0 is required. Version check is done at runtime" URL "-")
set_package_properties(OpenGL PROPERTIES TYPE REQUIRED PURPOSE "OpenGL is used for all rendering, without it you won't see anything :)")

set_package_properties(SDL_image PROPERTIES DESCRIPTION "Cross-platform open source image loading library" URL "")
set_package_properties(SDL_image PROPERTIES TYPE REQUIRED PURPOSE "For loading textures from disk")

set_package_properties(YamlCpp PROPERTIES DESCRIPTION "YAML CPP < 0.5, >= 0.30" URL "http://code.google.com/p/yaml-cpp/")
set_package_properties(YamlCpp PROPERTIES TYPE REQUIRED PURPOSE "For config file and sprite sheet reading")

set_package_properties(Chipmunk PROPERTIES DESCRIPTION "Chipmunk open source 2D physics engine" URL "https://github.com/slembcke/Chipmunk-Physics")
set_package_properties(Chipmunk PROPERTIES TYPE REQUIRED PURPOSE "I use it for everything collision-wise in the game.")

set_package_properties(enet PROPERTIES DESCRIPTION "enet reliable UDP networking library" URL "http://enet.bespin.org/index.html")
set_package_properties(enet PROPERTIES TYPE REQUIRED PURPOSE "For networking, obviously.")

set_package_properties(Protobuf PROPERTIES DESCRIPTION "Google's protobuf Protocol Buffers implementation" URL "http://code.google.com/p/protobuf/")
set_package_properties(Protobuf PROPERTIES TYPE REQUIRED PURPOSE "For serializing and deserializing data in a cross-architecture/platform manner. Used for network communication as well as saving game files and such.")

set_package_properties(LibNoise PROPERTIES DESCRIPTION "libnoise" URL "libnoise")
set_package_properties(LibNoise PROPERTIES TYPE REQUIRED PURPOSE "For easily generating brownian noise for world tilemap/terrain generation")

set_package_properties(ZLIB PROPERTIES DESCRIPTION "zlib" URL "zlib")
set_package_properties(ZLIB PROPERTIES TYPE REQUIRED PURPOSE "For compression for network communication as well as file loading/saving")

feature_summary(WHAT ALL)
#FATAL_ON_MISSING_REQUIRED_PACKAGES

#   DEPENDS ${proj_DEPENDENCIES}


include_directories(${CMAKE_SOURCE_DIR})

configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include_directories("${PROJECT_BINARY_DIR}")

#message("PROJ BIN DITR: " + ${CMAKE_SOURCE_DIR} + "/src/network/protobuf-src")
## protoc -I=protobuf-src --cpp_out=protobuf-compiled protobuf-src/packet.proto
##kexecute_process("protoc -I=" + ${PROJECT_BINARY_DIR} + " --cpp out=")
#
#set (NETWORKPATH ${CMAKE_SOURCE_DIR}/src/network)
#set (PROTOBUF_SRC_GEN ${NETWORKPATH}/protobuf-src)
#set (PROTOBUF_COMPILED_GEN ${NETWORKPATH}/protobuf-compiled)
#
#set(PROTOBUF_CMD_ARGS "protoc -I=" "${PROTOBUF_SRC_GEN} --cpp_out=${PROTOBUF_COMPILED_GEN}" " ${PROTOBUF_SRC_GEN}/packet.proto")
#
#message(${PROTOBUF_CMD_ARGS})
#execute_process(COMMAND "${PROTOBUF_CMD_ARGS}" RESULT_VARIABLE result)
#message("GENERATED PROTOBUF FILES, result: " ${result})
#
#add_custom_command(
#OUTPUT foo
#COMMAND protoc
#DEPENDS foo.in)
#)
#
#add_custom_target(process_protoc DEPENDS protoc)
#add_dependencies(main-target process_protoc)

add_subdirectory(src)
