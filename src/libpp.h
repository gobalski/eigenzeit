/* LIB PRIMS PICO libpp.h */
#pragma once
#include <stdio.h>

#ifdef PROD
  #define LOG_DEBUG(...) ((void)0)
#else
  #define LOG_DEBUG(...) fprintf(stdout, __VA_ARGS__)
#endif
