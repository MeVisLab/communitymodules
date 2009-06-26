//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METKPickingSystem.h
// \author  Christian Tietjen
// \date    2006-05-16
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKPICKING_SYSTEM_H
#define __METKPICKING_SYSTEM_H


// DLL export macro definition
#ifdef WIN32
#ifdef METKPICKING_EXPORTS
// Use the METKMESSAGING_EXPORT macro to export classes and functions
#define METKPICKING_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define METKPICKING_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define METKPICKING_EXPORT
#endif


#endif // __METKPICKING_SYSTEM_H


