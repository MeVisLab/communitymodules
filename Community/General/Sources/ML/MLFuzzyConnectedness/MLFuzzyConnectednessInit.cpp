// **InsertLicense** code
//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    MLFuzzyConnectednessInit.cpp
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


// Local includes
#include "MLFuzzyConnectednessSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include "mlLibraryInitMacros.h"

// Include all module headers.
#include "mlFuzzyConnectedness.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int MLFuzzyConnectednessInit ()
{
  ML_TRACE_IN("MLFuzzyConnectednessInit ()")

  FuzzyConnectedness::initClass();
  // Add initClass calls from all other modules here...

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(MLFuzzyConnectednessInit)

