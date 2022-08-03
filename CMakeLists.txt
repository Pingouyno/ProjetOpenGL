cmake_minimum_required(VERSION 3.17)

project(ProjetOpenGL)

file(GLOB SOURCES_DIR "sources/*")
#file(GLOB HEADERS_DIR "headers/*.h")

#add_executable(${PROJECT_NAME} main.cpp ${SOURCES_DIR})

add_executable( ${PROJECT_NAME} 
main.cpp 
./sources/PlaySound.cpp
./sources/glad.c
./sources/stb.cpp
./sources/VBO.cpp
./sources/VAO.cpp
./sources/EBO.cpp
./sources/shaderClass.cpp
./sources/GlobalArrays.cpp
./sources/Texture.cpp
./sources/Shape.cpp 
./sources/Quad.cpp 
./sources/Quad2D.cpp 
./sources/Cube.cpp
./sources/TextManager.cpp
./sources/Camera.cpp
./sources/Overlay.cpp
./sources/World.cpp
./sources/EventManager.cpp
)

#target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/freetype2 /usr/local/freetype2)


target_link_libraries(${PROJECT_NAME} GL dl glfw IrrKlang)

