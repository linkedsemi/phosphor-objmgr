if(CONFIG_OPENBMC_PHOSPHOR_OBJMGR)
    zephyr_library()
    zephyr_library_sources(
        # ${ZEPHYR_CURRENT_MODULE_DIR}/fail-monitor/main.cpp
        # ${ZEPHYR_CURRENT_MODULE_DIR}/fail-monitor/monitor.cpp
        # ${ZEPHYR_CURRENT_MODULE_DIR}/libmapper/app.c
        ${ZEPHYR_CURRENT_MODULE_DIR}/libmapper/mapper.c
        ${ZEPHYR_CURRENT_MODULE_DIR}/src/associations.cpp
        ${ZEPHYR_CURRENT_MODULE_DIR}/src/handler.cpp
        ${ZEPHYR_CURRENT_MODULE_DIR}/src/processing.cpp
        ${ZEPHYR_CURRENT_MODULE_DIR}/src/main.cpp
    )
    # zephyr_library_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/src)
    zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/src)
    zephyr_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/libmapper)
    # zephyr_library_compile_definitions(_GNU_SOURCE)

endif()
