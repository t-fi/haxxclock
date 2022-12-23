idf_component_register(SRCS "hello_world_main.cpp" "HD108_driver.cpp" INCLUDE_DIRS "")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
