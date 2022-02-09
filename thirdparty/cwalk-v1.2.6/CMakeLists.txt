# CWalk had an annoying CMake file that didn't allow you to disable installing,
# so we do this instead. This is version 1.2.6.

add_library(cwalk STATIC
  "include/cwalk/cwalk.h"
  "src/cwalk.c"
)

target_include_directories(cwalk PUBLIC "include")
target_include_directories(cwalk PRIVATE "include/cwalk")
