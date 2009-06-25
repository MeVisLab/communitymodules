// Copyright (c) 2009, Fraunhofer MEVIS Institute for Medical Image Computing
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Fraunhofer MEVIS nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR THE COPYRIGHT HOLDER BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------------
//! ML module class to load data for the Coronary Artery Tracking competition 2008.
/*!
// \file    mlLoadCAT08Data.h
// \author  Ola Friman
// \date    2009-05-20
//
// Loads data for the Coronary Artery Tracking competition 2008
*/
//----------------------------------------------------------------------------------


#ifndef __mlLoadCAT08Data_H
#define __mlLoadCAT08Data_H


// Local includes
#include "MLLoadCAT08DataSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#include "mlXMarkerList.h"
#if ML_GRAPH_IS_AVAILABLE
  #include "mlGraph.h"
  #include "mlVesselNode.h"
  #include "mlVesselEdge.h"
  #include "mlSkeleton.h"
#endif

ML_START_NAMESPACE


//! ML module class to load data for the Coronary Artery Tracking competition 2008.
class MLLOADCAT08DATA_EXPORT LoadCAT08Data : public BaseOp
{
public:

  //! Constructor.
  LoadCAT08Data ();

  //! Destructor
  ~LoadCAT08Data();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! Name and path to the .mhd file.
  StringField *_fileStringFld;

  //! Output base fields
  BaseField* _outputAFld;
  BaseField* _outputBFld;
  BaseField* _outputSFld;
  BaseField* _outputEFld;
  BaseField* _outputCenterLineFld;
  #if ML_GRAPH_IS_AVAILABLE
    BaseField* _outputGraphFld;
  #endif

  //! Data structures for A,B,S,E points, centerlines and an ML Graph
  XMarkerList _AList;
  XMarkerList _BList;
  XMarkerList _SList;
  XMarkerList _EList;
  XMarkerList _centerLineList;
  #if ML_GRAPH_IS_AVAILABLE
    Graph   _outputGraph;
  #endif

  //! Clear all data structures.
  void _clearAll();


  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(LoadCAT08Data)
};


ML_END_NAMESPACE

#endif // __mlLoadCAT08Data_H

