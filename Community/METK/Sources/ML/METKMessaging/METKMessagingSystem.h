//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METKMessagingSystem.h
// \author  Christian Tietjen
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKMessagingSystem_H
#define __METKMessagingSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef METKMESSAGING_EXPORTS
// Use the METKMESSAGING_EXPORT macro to export classes and functions
#define METKMESSAGING_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define METKMESSAGING_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define METKMESSAGING_EXPORT
#endif


#endif // __METKMessagingSystem_H


