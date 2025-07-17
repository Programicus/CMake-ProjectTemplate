#pragma once

#define assert(expr) ASSERT(expr, "VKBootstrap assert: {}", expr)
#include <VkBootstrap.h>
#undef assert