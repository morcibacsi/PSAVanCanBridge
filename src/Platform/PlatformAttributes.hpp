#pragma once

#ifdef PLATFORM_NATIVE
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif
#else
#include <esp_attr.h>
#endif

