
#ifndef __MLImageUtilitiesSystem_H
#define __MLImageUtilitiesSystem_H

//! Defines export symbols for classes, so they can be used in other DLLs.
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLIMAGEUTILITIES_EXPORTS
    // Export library symbols.
    #define MLIMAGEUTILITIES_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLIMAGEUTILITIES_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLIMAGEUTILITIES_EXPORTS
  #define MLIMAGEUTILITIES_EXPORT __declspec(dllexport)
  #else
  #define MLIMAGEUTILITIES_EXPORT __declspec(dllimport)
  #endif

  #else
  #define MLIMAGEUTILITIES_EXPORT
  #endif
#endif

#endif // __MLImageUtilitiesSystem_H
