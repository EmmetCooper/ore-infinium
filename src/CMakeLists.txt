set(SRC
activechunk.cpp
block.cpp
camera.cpp
chunk.cpp
debug.cpp
entity.cpp
fluidcell.cpp
fluids.cpp
fluidrenderer.cpp
game.cpp
image.cpp
lightrenderer.cpp
main.cpp
player.cpp
shader.cpp
sprite.cpp
spritesheetrenderer.cpp
particlerenderer.cpp
physicsdebugrenderer.cpp
texture.cpp
tilerenderer.cpp
torch.cpp
world.cpp
skyrenderer.cpp
time.cpp
item.cpp
quickbarinventory.cpp
timer.cpp
spatialhash.cpp
vegetation.cpp
object.cpp
#quadtreerenderer.cpp
tool.cpp

client/client.cpp
client/gui/chatdialog.cpp
client/gui/debugmenu.cpp
client/gui/debugsettings.cpp
client/gui/gui.cpp
client/gui/mainmenu.cpp
client/gui/optionsdialog.cpp
client/gui/quickbarmenu.cpp

client/gui/core/Shell.cpp
client/gui/core/ShellFileInterface.cpp
client/gui/core/ShellRenderInterfaceOpenGL.cpp
client/gui/core/SystemInterfaceSDL2.cpp

client/gui/decoratorspritesheet.cpp
client/gui/decoratorspritesheetinstancer.cpp

server/server.cpp
server/contactlistener.cpp

settings/settings.cpp

network/packet.cpp
${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc

unittest.cpp

${CMAKE_SOURCE_DIR}/utils/GLEW/glew.c

squircle.cpp

)

include(FindPkgConfig)

add_executable(ore-infinium ${SRC})

find_program(PROTOC_EXEC NAME protoc
PATHS
${CMAKE_INSTALL_PREFIX}/bin
${CMAKE_INSTALL_PREFIX}/protobuf/src
/usr/bin/
/usr/local/bin
/opt/bin
)


if (NOT PROTOC_EXEC)
    message(FATAL_ERROR
    "ERROR: We couldn't launch launch protoc. We need the 'protoc' binary for at build time to generate network shit. If this message is in error, check the output of 'which protoc' maybe the search paths need fixed in this file."
    )
endif (NOT PROTOC_EXEC)

message("FOUND PROTOC BINARY! (good), path is: " + ${PROTOC_EXEC})
message("RUNNING PROTOC BINARY WITH COMMAND: " + ${PROTOC_EXEC}  -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto)

# ----------------------- Generate protoc (somefile.pb.cc and somefile.pb.h) from a given somefile.proto file and build against that. the 'protoc' binary has to exist, but apparently distros have 2 packages mostly, libprotobuf and protoc, both of which are needed  -----------------------------------------
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc
  COMMAND ${PROTOC_EXEC}  -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/packet.proto
#;protobuf
)

add_custom_target(
  do_protoc
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/packet.pb.cc
# ;protobuf
)

add_dependencies(ore-infinium do_protoc)

include_directories(
# ----------
${CMAKE_SOURCE_DIR}/utils/glm/
${CMAKE_SOURCE_DIR}/utils/GLEW/

${CMAKE_INSTALL_PREFIX}/utils/GLEW/
#-----------

${CMAKE_SOURCE_DIR}
${WEBP_INCLUDE_DIR}
${SDL2_INCLUDE_DIR}
${SDLIMAGE_INCLUDE_DIR}
${FREETYPE_INCLUDE_DIRS}
${YAMLCPP_INCLUDE_DIR}
${LIBROCKET_INCLUDE_DIRS}
${ENET_INCLUDE_DIRS}
${PROTOBUF_INCLUDE_DIR}
${CHIPMUNK_INCLUDE_DIR}
${LIBNOISE_INCLUDE_DIR}
${Qt5Quick_INCLUDES}
)

target_link_libraries(
ore-infinium
${SDL2_LIBRARY}
${SDL2MAIN_LIBRARY}
${WEBP_LIBRARIES}
${SDLIMAGE_LIBRARY}
${ZLIB_LIBRARY}
${FREETYPE_LIBRARY}
${OPENGL_LIBRARIES}
${LIBROCKET_LIBRARIES}
${LIBROCKET_DEBUGGER_LIBRARY}
${LIBROCKET_CONTROLS_LIBRARY}
${YAMLCPP_LIBRARY}
${ENET_LIBRARIES}
${PROTOBUF_LIBRARY}
${CHIPMUNK_LIBRARIES}
${LIBNOISE_LIBRARY}
${Qt5Quick_LIBRARIES}
)

# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${ore_infinium_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ore_infinium_VERSION_MINOR}")
include(CPack)

#add_subdirectory(${CMAKE_SOURCE_DIR}/src/)

# Install target
install(DIRECTORY ../textures/ DESTINATION "textures") #FILES_MATCHING PATTERN "*")
install(DIRECTORY ../font/ DESTINATION "font") #FILES_MATCHING PATTERN "*")
install(DIRECTORY ./client/ DESTINATION "client") #FILES_MATCHING PATTERN "*")

install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.frag" )
install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.vert" )
install(DIRECTORY ./ DESTINATION bin FILES_MATCHING PATTERN "*.gs" )
install(TARGETS "ore-infinium" DESTINATION bin)

