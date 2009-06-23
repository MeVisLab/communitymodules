//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDViewerTriggerSystem.h
// \author  Konrad Mühler
// \date    2005-04-23
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDVIEWERTRIGGERSYSTEM_H__
#define __UMDVIEWERTRIGGERSYSTEM_H__


// DLL export macro definition
#ifdef WIN32
#ifdef UMDVIEWERTRIGGER_EXPORTS
// Use the UMDVIEWERTRIGGER_EXPORT macro to export classes and functions
#define UMDVIEWERTRIGGER_EXPORT __declspec(dllexport)
#else
// If included by external modules, exported symbols are declared as import symbols
#define UMDVIEWERTRIGGER_EXPORT __declspec(dllimport)
#endif

#else
// No export declarations are necessary for non-Windows systems
#define UMDVIEWERTRIGGER_EXPORT
#endif


#endif
// __UMDVIEWERTRIGGERSYSTEM_H__

