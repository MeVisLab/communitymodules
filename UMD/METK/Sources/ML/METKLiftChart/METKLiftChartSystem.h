//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    METKLiftChartSystem.h
// \author  Christian Tietjen
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKLIFTCHARTSYSTEM_H
#define __METKLIFTCHARTSYSTEM_H


// DLL export macro definition
#if defined (ML_LIBRARY_EXPORT_ATTRIBUTE) || defined (ML_LIBRARY_IMPORT_ATTRIBUTE)
  // Make use of the new style and platform independent import/export definitions.
  #ifdef METKLIFTCHART_EXPORTS
    // Export library symbols.
    #define METKLIFTCHART_EXPORT   ML_LIBRARY_EXPORT_ATTRIBUTE
  #else
    // If included by external modules, exported symbols are declared as import symbols
    #define METKLIFTCHART_EXPORT   ML_LIBRARY_IMPORT_ATTRIBUTE
  #endif
#else

  // Only for backward compatibility to versions < MeVisLab2.2.
  #ifdef WIN32


  #ifdef METKLIFTCHART_EXPORTS
  // Use the UMDINVENTORPOINTER_EXPORT macro to export classes and functions
  #define METKLIFTCHART_EXPORT __declspec(dllexport)
  #else
  // If included by external modules, exported symbols are declared as import symbols
  #define METKLIFTCHART_EXPORT __declspec(dllimport)
  #endif

  #else
  // No export declarations are necessary for non-Windows systems
  #define METKLIFTCHART_EXPORT
  #endif
#endif

#ifdef WIN32
  // wenn die warnings innerhalb des push/pop deaktiviert werden, werden sie vom pop wieder aktiviert
  #pragma warning( disable : 4514 ) // Nichtreferenzierte Inline-Funktion wurde entfernt
  #pragma warning( disable : 4505 ) // Nichtreferenzierte lokale Funktion wurde entfernt
  #pragma warning( disable : 4097 ) // Erwarteter pragma-Parameter sollte 'restore' oder 'off' sein
  #pragma warning( disable : 4786 ) // Bezeichner wurde auf 'Nummer' Zeichen in den Fehlerinformationen verkürzt
  #pragma warning( disable : 4710 ) // ... ist keine Inline-Funktion

  #pragma warning( disable : 4511 ) // Kopierkonstruktor konnte nicht generiert werden
  #pragma warning( disable : 4512 ) // Zuweisungsoperator konnte nicht generiert werden

  // Disable ugly warnings of windows headers.
  #pragma warning ( push, 3 )
#endif

#include <mlOpenGL.h>

//#include <windows.h>
#include <string>
//#include <FLOAT.H>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#ifdef WIN32
  #pragma warning ( pop )
#endif


#endif // __METKLIFTCHARTSYSTEM_H


