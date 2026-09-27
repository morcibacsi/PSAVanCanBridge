if(IDF_TARGET STREQUAL "esp32c6")
    idf_component_register(
        SRCS "src/LpCoreVanTx.cpp"
        INCLUDE_DIRS "include"
        PRIV_INCLUDE_DIRS "private_include"
        REQUIRES esp_driver_gpio esp_hw_support
        PRIV_REQUIRES esp_timer freertos ulp)

    # Use a component-specific application name so this component cannot
    # collide with another ULP application in the consuming project.
    set(ulp_app_name lp_core_van_ulp)
    set(ulp_sources "ulp/main.c")
    set(ulp_exp_dep_srcs "src/LpCoreVanTx.cpp")
    ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
else()
    # Header-only registration lets unsupported targets discover the component
    # and include LpCoreVanTx.hpp without pulling in C6-only dependencies.
    idf_component_register(INCLUDE_DIRS "include")
endif()
