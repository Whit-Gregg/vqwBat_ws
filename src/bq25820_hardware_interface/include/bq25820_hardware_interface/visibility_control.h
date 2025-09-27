#ifndef BQ25820_HARDWARE_INTERFACE__VISIBILITY_CONTROL_H_
#define BQ25820_HARDWARE_INTERFACE__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define BQ25820_HARDWARE_INTERFACE_EXPORT __attribute__ ((dllexport))
    #define BQ25820_HARDWARE_INTERFACE_IMPORT __attribute__ ((dllimport))
  #else
    #define BQ25820_HARDWARE_INTERFACE_EXPORT __declspec(dllexport)
    #define BQ25820_HARDWARE_INTERFACE_IMPORT __declspec(dllimport)
  #endif
  #ifdef BQ25820_HARDWARE_INTERFACE_BUILDING_LIBRARY
    #define BQ25820_HARDWARE_INTERFACE_PUBLIC BQ25820_HARDWARE_INTERFACE_EXPORT
  #else
    #define BQ25820_HARDWARE_INTERFACE_PUBLIC BQ25820_HARDWARE_INTERFACE_IMPORT
  #endif
  #define BQ25820_HARDWARE_INTERFACE_PUBLIC_TYPE BQ25820_HARDWARE_INTERFACE_PUBLIC
  #define BQ25820_HARDWARE_INTERFACE_LOCAL
#else
  #define BQ25820_HARDWARE_INTERFACE_EXPORT __attribute__ ((visibility("default")))
  #define BQ25820_HARDWARE_INTERFACE_IMPORT
  #if __GNUC__ >= 4
    #define BQ25820_HARDWARE_INTERFACE_PUBLIC __attribute__ ((visibility("default")))
    #define BQ25820_HARDWARE_INTERFACE_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define BQ25820_HARDWARE_INTERFACE_PUBLIC
    #define BQ25820_HARDWARE_INTERFACE_LOCAL
  #endif
  #define BQ25820_HARDWARE_INTERFACE_PUBLIC_TYPE
#endif

#endif  // BQ25820_HARDWARE_INTERFACE__VISIBILITY_CONTROL_H_
