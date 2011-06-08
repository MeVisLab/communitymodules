//----------------------------------------------------------------------------------
//! Project global and OS specific declarations
/*!
// \file    UMDQHull3DSystem.h
// \author  Jens Haase
// \date    2005-02-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __UMDQHULL3DSYSTEM_H__
#define __UMDQHULL3DSYSTEM_H__


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef UMDQHULL3D_EXPORTS
    // Export library symbols.
    #define UMDQHULL3D_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define UMDQHULL3D_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
    #ifdef UMDQHULL3D_EXPORTS
      // Use the UMDQHULL3D_EXPORT macro to export classes and functions
      #define UMDQHULL3D_EXPORT __declspec(dllexport)
    #else
      // If included by external modules, exported symbols are declared as import symbols
      #define UMDQHULL3D_EXPORT __declspec(dllimport)
    #endif
  #else
    // No export declarations are necessary for non-Windows systems
    #define UMDQHULL3D_EXPORT
  #endif
#endif

#ifdef WIN32
#include <vector>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;
#endif


#endif
// __UMDQHULL3DSYSTEM_H__
