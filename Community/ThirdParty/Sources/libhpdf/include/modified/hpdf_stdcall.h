//----------------------------------------------------------------------------------
//! Build  header file for libhpdf (aka libHaru) project for MeVisLab.
/*!
// \file    hpdf_stdcall.h
// \author  Axel Newe
// \date    2016-01-11
//
// This header defines OS specific settings and is derived from and is needed
// to make the original library code compile when the project is generated
// by MeVisLab.
*/
//----------------------------------------------------------------------------------

#ifndef WIN32
 #ifndef __stdcall
  #define __stdcall __attribute__((stdcall))
 #endif
#endif