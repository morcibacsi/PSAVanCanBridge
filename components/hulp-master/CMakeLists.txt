set(include_dirs
    "src"
)

set(srcs
    "src/hulp.c"
    "src/hulp_touch.c"
    "src/hulp_uart.c"
    "src/hulp_regwr.c"
    "src/hulp_debug.c"
)

# ESP-IDF 6 removed the legacy touch-sensor register headers used by HULP's
# optional touch helper. The VAN ULP implementation does not use that helper.
include($ENV{IDF_PATH}/tools/cmake/version.cmake)
if("${IDF_VERSION_MAJOR}" VERSION_GREATER_EQUAL "6")
    list(REMOVE_ITEM srcs "src/hulp_touch.c")
endif()

set(requires
    ulp
    driver
    soc
    hal
)

idf_component_register(
    INCLUDE_DIRS "${include_dirs}"
    SRCS "${srcs}"
    REQUIRES "${requires}"
)
