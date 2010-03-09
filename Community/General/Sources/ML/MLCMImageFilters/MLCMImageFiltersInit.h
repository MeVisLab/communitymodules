//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    EMCExpressionParserInit.h
// \author  Theo van Walsum
*/
//----------------------------------------------------------------------------------


#ifndef __MLCMImageFilters_H
#define __MLCMImageFilters_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type 
//! system of the ML.
int MLCMImageFiltersInit (void);

ML_END_NAMESPACE

#endif // __MLCMImageFiltersInit_H


