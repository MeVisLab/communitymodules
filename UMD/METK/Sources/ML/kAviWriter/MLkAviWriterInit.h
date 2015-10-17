//----------------------------------------------------------------------------------
//
// Copyright 2001-2004, MeVis gGmbH
// ALL RIGHTS RESERVED
//
// THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
// INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
// DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
// PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
//
//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLkAviWriterInit.h
// \author  okonrad
// \date    2005-07-21
//
*/
// Last changed by $Author: okonrad $
// $Id: MLkAviWriterInit.h 27276 2005-07-28 11:59:40Z okonrad $
// $Source$
//----------------------------------------------------------------------------------


#ifndef __MLkAviWriterInit_H
#define __MLkAviWriterInit_H


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int MLkAviWriterInit (void);

ML_END_NAMESPACE

#endif // __MLkAviWriterInit_H


