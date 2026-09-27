FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
list(FILTER app_sources EXCLUDE REGEX "/Platform/Native/")
idf_component_register(
    SRCS ${app_sources}
    PRIV_REQUIRES lp_core_van
    #REQUIRES bt
)

littlefs_create_partition_image(littlefs flash_data FLASH_IN_PROJECT)

if(CONFIG_IDF_TARGET_ESP32C6 AND PYTHON MATCHES "platformio")
    # PlatformIO only supports an application-level LP program. Keep the
    # standalone component untouched, but compile its LP source through the
    # root ulp/main.c shim and select the library's existing PlatformIO ABI.
    set(ulp_app_name ulp_main)
    set(ulp_sources ${CMAKE_CURRENT_SOURCE_DIR}/../ulp/main.c)
    set(ulp_exp_dep_srcs ${app_sources})
    ulp_embed_binary(${ulp_app_name} ${ulp_sources} ${ulp_exp_dep_srcs})

    target_compile_definitions(__idf_lp_core_van PRIVATE LP_CORE_VAN_PLATFORMIO)
    target_include_directories(__idf_lp_core_van PRIVATE
        ${CMAKE_BINARY_DIR}/esp-idf/src/ulp_main)

    # PlatformIO compiles component sources itself and cannot schedule the
    # component-local ulp_embed_binary output. Exclude only that generated
    # assembly source; LpCoreVanTx.cpp remains owned by the component.
    get_target_property(lp_core_van_sources __idf_lp_core_van SOURCES)
    set(lp_core_van_platformio_sources)
    foreach(lp_core_van_source IN LISTS lp_core_van_sources)
        get_filename_component(lp_core_van_source_name ${lp_core_van_source} NAME)
        if(NOT lp_core_van_source_name STREQUAL lp_core_van_ulp.bin.S)
            list(APPEND lp_core_van_platformio_sources ${lp_core_van_source})
        endif()
    endforeach()
    set_property(TARGET __idf_lp_core_van PROPERTY SOURCES ${lp_core_van_platformio_sources})

    # The same component-local ULP artifact also contributes a linker option
    # and dependency that PlatformIO cannot produce. The application-level
    # ulp_main artifact above replaces both.
    get_target_property(lp_core_van_link_options __idf_lp_core_van INTERFACE_LINK_OPTIONS)
    set(lp_core_van_platformio_link_options)
    foreach(lp_core_van_link_option IN LISTS lp_core_van_link_options)
        if(NOT lp_core_van_link_option MATCHES lp_core_van_ulp)
            list(APPEND lp_core_van_platformio_link_options ${lp_core_van_link_option})
        endif()
    endforeach()
    set_property(TARGET __idf_lp_core_van PROPERTY INTERFACE_LINK_OPTIONS
        ${lp_core_van_platformio_link_options})

    idf_build_get_property(lp_core_van_link_dependencies __LINK_DEPENDS)
    set(lp_core_van_platformio_link_dependencies)
    foreach(lp_core_van_link_dependency IN LISTS lp_core_van_link_dependencies)
        if(NOT lp_core_van_link_dependency MATCHES lp_core_van_ulp)
            list(APPEND lp_core_van_platformio_link_dependencies ${lp_core_van_link_dependency})
        endif()
    endforeach()
    idf_build_set_property(__LINK_DEPENDS ${lp_core_van_platformio_link_dependencies})
endif()
