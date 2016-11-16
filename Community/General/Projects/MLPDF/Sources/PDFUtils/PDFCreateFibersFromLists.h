//----------------------------------------------------------------------------------
//! The ML module class PDFCreateFibersFromLists.
/*!
// \file    PDFCreateFibersFromLists.h
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-29
//
// Create fibers from positions list and connections list.
*/
//----------------------------------------------------------------------------------


#pragma once


#ifndef _PDFCreateFibersFromLists_H
#define _PDFCreateFibersFromLists_H


// Local includes
#include "MLPDFSystem.h"
#include "../shared/MLPDF_Tools.h"

// Project includes
#include <IndexPairList.h>
#include <ColoredMarkerList.h>

// ML includes
#include <mlXMarkerList.h>
#include <mlFiberSet.h>


ML_START_NAMESPACE


//////////////////////////////////////////////////////////////////////////

class MLPDF_EXPORT PDFCreateFibersFromLists : public Module
{
  //! Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(PDFCreateFibersFromLists)

public:

  //! Constructor.
  PDFCreateFibersFromLists();

protected:

  //! Destructor.
  virtual ~PDFCreateFibersFromLists();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field* field);

private:

  /* FIELDS */

  // Input fields
  BaseField*     _inLinePositionsFld;
  BaseField*     _inLineConnectionsFld;

  // Fiber output field
  BaseField*     _outFibersFld;

  //! Fields for SoVertexProperty and SoIndexedLineSet
  StringField*   _vertexListFld;
  StringField*   _coordIndexListFld;


  //! The XMarkerList input for node positions of line sets
  ml::XMarkerList _inLinePositions;

  //! The StringList input for connections of line sets
  ml::IndexPairList _inLineConnections;

  //! The fiberset container used for visualizing the line sets
  FiberSetContainer _outFiberSetContainer;

  /* METHODS */

  void _createFibers();


//////////////////////////////////////////////


};

ML_END_NAMESPACE

#endif // _PDFCreateFibersFromLists_H