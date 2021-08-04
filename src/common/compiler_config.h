#ifndef GRIDGENERATOR_COMPILER_CONFIG_H
#define GRIDGENERATOR_COMPILER_CONFIG_H
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__) && !defined(__PGI) && !defined(_CRAYC) && !defined(_SX)
#define GCC_COMPILER
#elif defined(__clang__)
#define CLANG_COMPILER
#else
#error Unsupported compiler
#endif

#endif // GRIDGENERATOR_COMPILER_CONFIG_H
