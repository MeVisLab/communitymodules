
#ifndef __MLImageUtilitiesSystem_H
#define __MLImageUtilitiesSystem_H

//! Defines export symbols for classes, so they can be used in other DLLs.

#ifdef WIN32
#ifdef MLIMAGEUTILITIES_EXPORTS
#define MLIMAGEUTILITIES_EXPORT __declspec(dllexport)
#else
#define MLIMAGEUTILITIES_EXPORT __declspec(dllimport)
#endif

#else
#define MLIMAGEUTILITIES_EXPORT
#endif


#endif // __MLImageUtilitiesSystem_H
