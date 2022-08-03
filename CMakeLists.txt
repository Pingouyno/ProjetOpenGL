cmake_minimum_required(VERSION 3.17)

project(ProjetOpenGL)

file(GLOB SOURCES_DIR "sources/*")
#file(GLOB HEADERS_DIR "headers/*.h")

add_executable(${PROJECT_NAME} main.cpp ${SOURCES_DIR})

#target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/freetype2 /usr/local/freetype2)

target_link_libraries(${PROJECT_NAME} GL dl glfw IrrKlang)
