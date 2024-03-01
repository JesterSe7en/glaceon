#ifndef GLACEON_GLACEON_BASE_H_
#define GLACEON_GLACEON_BASE_H_

#ifdef _WIN64
#ifdef GLACEON_EXPORTS
#define GLACEON_API __declspec(dllexport)
#else
#define GLACEON_API __declspec(dllimport)
#endif
#elif __linux__
#ifdef GLACEON_EXPORTS
#define GLACEON_API __attribute__((visibility("default")))
#else
#define GLACEON_API
#endif
#else
#error "Unsupported platform"
#endif

#endif  // GLACEON_GLACEON_BASE_H_
