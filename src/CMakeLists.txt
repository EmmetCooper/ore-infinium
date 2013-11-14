
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS packet.proto)

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
quickview.cpp

client/client.cpp
client/graph.cpp
client/FboInSGRenderer.cpp
client/gui/optionsdialogbackend.cpp

server/server.cpp
server/contactlistener.cpp

settings/settings.cpp

network/packet.cpp
${PROTO_SRCS}

unittest.cpp

${CMAKE_SOURCE_DIR}/utils/GLEW/glew.c

)

include(FindPkgConfig)

add_executable(ore-infinium ${SRC})


include_directories(
# ----------
${CMAKE_SOURCE_DIR}/utils/glm/
${CMAKE_SOURCE_DIR}/utils/GLEW/
${CMAKE_CURRENT_BINARY_DIR}/ #for moc

${CMAKE_INSTALL_PREFIX}/utils/GLEW/
#-----------

${CMAKE_SOURCE_DIR}
${SDL2_INCLUDE_DIR}
${YAMLCPP_INCLUDE_DIR}
${ENET_INCLUDE_DIRS}
${PROTOBUF_INCLUDE_DIR}
${CHIPMUNK_INCLUDE_DIR}
${Qt5Quick_INCLUDES}
)

target_link_libraries(
ore-infinium
${SDL2_LIBRARY}
${SDL2MAIN_LIBRARY}
${OPENGL_LIBRARIES}
${YAMLCPP_LIBRARY}
${ENET_LIBRARIES}
${PROTOBUF_LIBRARY}
${CHIPMUNK_LIBRARIES}
${Qt5Quick_LIBRARIES}
)

message("QT5QUICK LIBS: " + ${Qt5Quick_LIBRARIES})

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

