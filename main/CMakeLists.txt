idf_component_register(
        SRCS
        "main.cpp"
        "HD108_driver.cpp"
        "segment_frame_generator.cpp"
        "init_wifi.cpp"
        "http_server.cpp"
        "sunrise.cpp"
        INCLUDE_DIRS
        "")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
