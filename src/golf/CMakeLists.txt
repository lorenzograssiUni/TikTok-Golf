cmake_minimum_required(VERSION 3.10)

project(golf C)

set(CMAKE_C_STANDARD 99)

add_executable(golf
    main.c
    game.c
    draw.c
    ui.c
    golf.c
    socket_listener.c
)

target_include_directories(golf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_link_libraries(golf
    glfw
    opengl32
)
