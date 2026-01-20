#pragma once

#include <cstdio>
#include <cstdlib>

#ifdef DEBUG

    #define DBG_ASSERT(cond)                                           \
        do {                                                           \
            if (!(cond)) {                                             \
                std::fprintf(stderr,                                   \
                    "ASSERT FAILED: %s\n  %s:%d\n",                    \
                    #cond, __FILE__, __LINE__);                        \
                std::abort();                                          \
            }                                                          \
        } while (0)

    #define DBG_LOG(fmt, ...)                                           \
        do {                                                           \
            std::fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__);  \
        } while (0)

#else

    #define DBG_ASSERT(cond) ((void)0)
    #define DBG_LOG(fmt, ...) ((void)0)

#endif
