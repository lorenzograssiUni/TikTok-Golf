add_executable(golf
    main.c
    game.c
    draw.c
    ui.c
    golf.c
    socket_listener.c
)

target_link_libraries(golf PRIVATE
    common
    fast_obj
    glad
    glfw
    lightmapper
    mattiasgustavsson_libs
    miniz
    parson
    sokol
    stb
    xatlas
)

if(WIN32)
    target_link_libraries(golf PRIVATE ws2_32)
endif()