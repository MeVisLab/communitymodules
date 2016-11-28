//----------------------------------------------------------------------------------
//! The ML module class GraphToFibers.
/*!
// \file    mlGraphToFibers.h
// \author  Axel Newe
// \date    2016-03-31
//
// 
*/
//----------------------------------------------------------------------------------


#pragma once


// Local includes
#include "GraphToFibersSystem.h"

// ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>
#include <mlGraph.h>
#include <mlVesselNode.h>
#include <mlVesselEdge.h>
#include <mlSkeleton.h>
#include <mlFiberSet.h>
#include <mlFiberSetDefines.h>

// Project includes
#include <IndexPairList.h>


ML_START_NAMESPACE


//! 
class GRAPHTOFIBERS_EXPORT GraphToFibers : public Module
{
public:

  //! Constructor.
  GraphToFibers();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

  //! Updates the internal module state after loading or cloning the module,
  //! and enables notification handling again.
  virtual void activateAttachments();

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! 
  BaseField*  _inGraphFld;
  //! 
  BaseField*  _outNodePositionsFld;
  BaseField*  _outEdgePositionsFld;
  BaseField*  _outEdgeConnectionsFld;
  BaseField*  _outFibersFld;
  //@}


  //! The input vessel graph
  Graph             _inGraph;

  //! The XMarkerList output for point positions of point clouds
  ml::XMarkerList _outNodePositions;

  //! The XMarkerList output for node positions of line sets
  ml::XMarkerList _outEdgePositions;

  //! The IndexPairList output for connections of line sets
  ml::IndexPairList _outEdgeConnections;

  //! The fiberset containers used for visualizing the line sets
  FiberSetContainer _outFiberSetContainer;

  //! Update the output fibers
  void _updateOutput();
  void _createFibers();

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(GraphToFibers)
};


ML_END_NAMESPACE