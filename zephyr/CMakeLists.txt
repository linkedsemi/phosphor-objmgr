if(CONFIG_OPENBMC_PHOSPHOR_OBJMGR)
    zephyr_library_named(phosphor-objmgr)
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

    zephyr_library_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/src)
    zephyr_export_include_directories(${ZEPHYR_CURRENT_MODULE_DIR}/libmapper)

    zephyr_library_link_libraries(boost)
    zephyr_library_link_libraries(tinyxml2)
    zephyr_library_link_libraries(sd-bus)
    zephyr_library_link_libraries(sdbusplus)
    zephyr_library_link_libraries(phosphor-dbus-interfaces)
endif()
