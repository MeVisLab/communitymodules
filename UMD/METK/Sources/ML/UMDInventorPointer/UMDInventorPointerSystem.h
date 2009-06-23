//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    UMDInventorPointerSystem.h
// \author  Konrad
// \date    2006-02-02
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDInventorPointerSystem_H
#define __UMDInventorPointerSystem_H


// DLL export macro definition
#ifdef WIN32
#ifdef UMDINVENTORPOINTER_EXPORTS
// Use the UMDINVENTORPOINTER_EXPORT macro to export classes and functions
#define UMDINVENTORPOINTER_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDINVENTORPOINTER_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDINVENTORPOINTER_EXPORT
#endif


#endif // __UMDInventorPointerSystem_H


