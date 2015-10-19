//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    METKObjXMLWriterInit.h
// \author  Konrad Mühler
// \date    2006-12-12
//
*/
//----------------------------------------------------------------------------------


#ifndef __METKObjXMLWriterInit_H
#define __METKObjXMLWriterInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
void METKObjXMLWriterInit (void);

ML_END_NAMESPACE

#endif // __METKObjXMLWriterInit_H


