//
//  MLOsiriXBridgeSystem.h
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 18.01.08.
//  This code is in the public domain.
//

#ifndef __MLOsiriXBridgeSystem_H
#define __MLOsiriXBridgeSystem_H

// DLL export macro definition
#ifdef MLOSIRIXBRIDGE_EXPORTS
  // Use the MLOSIRIXBRIDGE_EXPORT macro to export classes and functions
  #define MLOSIRIXBRIDGE_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols
  #define MLOSIRIXBRIDGE_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif

#endif // __MLOsiriXBridgeSystem_H
