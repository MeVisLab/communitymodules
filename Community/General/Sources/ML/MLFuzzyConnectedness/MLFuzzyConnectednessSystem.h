// **InsertLicense** code
//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    MLFuzzyConnectednessSystem.h
// \author  Markus Hüllebrand
// \date    2009-06-12

  This file is part of the FuzzyConnectedness module for MeVisLab.

  Copyright (c) 2001-2009, Fraunhofer MEVIS, Bremen, Germany
  ALL RIGHTS RESERVED

  The FuzzyConnectedness module for MeVisLab is free software:
  you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The FuzzyConnectedness module for MeVisLab is distributed in
  the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
  for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
//----------------------------------------------------------------------------------


#ifndef __MLFuzzyConnectednessSystem_H
#define __MLFuzzyConnectednessSystem_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef MLFUZZYCONNECTEDNESS_EXPORTS
    // Export library symbols.
    #define MLFUZZYCONNECTEDNESS_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define MLFUZZYCONNECTEDNESS_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else
  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32
  #ifdef MLFUZZYCONNECTEDNESS_EXPORTS
  // Use the MLFUZZYCONNECTEDNESS_EXPORT macro to export classes and functions
  #define MLFUZZYCONNECTEDNESS_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLFUZZYCONNECTEDNESS_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define MLFUZZYCONNECTEDNESS_EXPORT
  #endif
#endif

#endif // __MLFuzzyConnectednessSystem_H


