//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    ${__TARGETFILE__}
// \author  ${vars.author}
// \date    ${__DATE__}
*/
//----------------------------------------------------------------------------------


#pragma once


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int ${vars.cppProject.fullProject}Init();

ML_END_NAMESPACE
