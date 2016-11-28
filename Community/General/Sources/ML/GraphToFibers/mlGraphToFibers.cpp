//----------------------------------------------------------------------------------
//! The ML module class GraphToFibers.
/*!
// \file    mlGraphToFibers.cpp
// \author  Axel Newe
// \date    2016-03-31
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlGraphToFibers.h"

// Project includes
#include <shared/MLU3D_MarkerListTools.h>
#include <MLPDF_Tools.h>

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(GraphToFibers, Module);

//----------------------------------------------------------------------------------

GraphToFibers::GraphToFibers() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  (_inGraphFld   = addBase("inGraph"))->setBaseValueAndAddAllowedType(&_inGraph);
  (_outNodePositionsFld = addBase("outNodePositions"))->setBaseValueAndAddAllowedType(&_outNodePositions);
  (_outEdgePositionsFld = addBase("outEdgePositions"))->setBaseValueAndAddAllowedType(&_outEdgePositions);
  (_outEdgeConnectionsFld = addBase("outEdgeConnections"))->setBaseValueAndAddAllowedType(&_outEdgeConnections);
  (_outFibersFld = addBase("outFibers"))->setBaseValueAndAddAllowedType(&_outFiberSetContainer);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate page data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

}

//----------------------------------------------------------------------------------

void GraphToFibers::handleNotification(Field* field)
{
  if (field == _inGraphFld)
  {
    ml::Base *inValue = _inGraphFld->getBaseValue();

    if (inValue)
    {
      if (ML_BASE_IS_A(inValue, ml::Graph))
      {
        ml::Graph* inGraph = ((ml::Graph*)inValue);
        _inGraph = *inGraph;
      }
    }
    else
    {
      _inGraph.clearGraph();
    }

    _updateOutput();

  }

}

//----------------------------------------------------------------------------------

void GraphToFibers::activateAttachments()
{
  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  Module::activateAttachments();
}

//----------------------------------------------------------------------------------

void GraphToFibers::_updateOutput()
{
  _outNodePositions.clearList();
  _outNodePositions.selectItemAt(-1);
  _outEdgePositions.clearList();
  _outEdgePositions.selectItemAt(-1);
  _outEdgeConnections.clearList();
  _outEdgeConnections.selectItemAt(-1);

  int nodeCount = 0;
  int edgeCount = 0;

  int  currentEdgePositionNum = 0;

  // Work on a local copy of the graph, because the smoothing will change it
  Graph* vesselGraph = NULL;
  ML_CHECK_NEW(vesselGraph, Graph(_inGraph));

  vesselGraph->purifyGraph();
  //vesselGraph->setRootIdToAllChildren();

  vesselGraph->closeSkeletonGaps();

  // smooth the graph
  const int numSmoothingPasses = 3;
  const float smoothingFactor = 0.7;
  //const Graph::EdgeIterator endEdge();
  for (Graph::EdgeIterator iter = vesselGraph->beginEdge(); iter != vesselGraph->endEdge(); ++iter)
  {
    static_cast<VesselEdge*>(*iter)->smooth(numSmoothingPasses, smoothingFactor, smoothingFactor);
  }

  // Iterate over all root nodes
  for (Graph::ConstNodeIterator iter = vesselGraph->beginRoot(); iter != vesselGraph->endRoot(); iter++)
  {

  }

  // Iterate over all nodes
  for (Graph::ConstNodeIterator i = vesselGraph->beginNode(); i != vesselGraph->endNode(); i++)
  {
    const VesselNode* thisNode = *i;
    const Vector3 thisNodePos = thisNode->getPos();

    //thisNode->getDepNode(); // Get dependent node via edge with index i.
    //thisNode->edges();
    //thisNode->getId();
    //thisNode->getLabel();

    // Add node markers
    XMarker thisNodeMarker(thisNodePos);
    std::string nodeName = "Node #" + mlPDF::intToString(nodeCount);
    thisNodeMarker.setName(nodeName.c_str());
    thisNodeMarker.type = 0;
    _outNodePositions.appendItem(thisNodeMarker);

    // Add edge markers & connections
    const size_t thisNodeEdgesNum = thisNode->getEdgeNum(); // Get Number of edges dependent to the node.
    for (size_t e = 0; e < thisNodeEdgesNum; e++)
    {
      const VesselEdge* thisNodeEdge = thisNode->getDepEdge(e); // Get the pointer of edge with index i. 

      if (thisNodeEdge)
      {
        bool newBranch = true;

        const size_t numSkeletons = thisNodeEdge->numSkeletons();

        for (size_t s = 0; s < numSkeletons; s++)
        {
          const Skeleton* thisSkeleton = thisNodeEdge->skeleton(s);

          Vector3 thisSkeletonPos = thisSkeleton->getPos(); 
          XMarker thisVesselEdgeMarker(thisSkeletonPos);
          thisVesselEdgeMarker.type = 0; // edgeCount;
          std::string edgeName = "Centerlines";
          //std::string edgeName = "Edge #" + mlPDF::intToString(edgeCount);
          thisVesselEdgeMarker.setName(edgeName.c_str());
          _outEdgePositions.appendItem(thisVesselEdgeMarker);

          /*
          if (s < numSkeletons - 1)
          {
            IndexPair thisVesselEdgeConnection((int)s, (int)s + 1);
            thisVesselEdgeConnection.type = edgeCount;
            _outEdgeConnections.appendItem(thisVesselEdgeConnection);
          }
          */

          if (!newBranch)
          {
            IndexPair thisVesselEdgeConnection(currentEdgePositionNum - 1, currentEdgePositionNum);
            thisVesselEdgeConnection.type = 0; 
            _outEdgeConnections.appendItem(thisVesselEdgeConnection);
          }

          currentEdgePositionNum++;
          newBranch = false;

        }

        edgeCount++;

      }

    }

    nodeCount++;
  }

  ML_DELETE(vesselGraph);

  _outNodePositionsFld->touch();
  _outEdgePositionsFld->touch();
  _outEdgeConnectionsFld->touch();
  _createFibers();
}

//----------------------------------------------------------------------------------

void GraphToFibers::_createFibers()
{
  mlU3D::U3DMarkerListTools::fillFiberSetContainerFromPositionsAndConnections(_outFiberSetContainer, _outEdgePositions, _outEdgeConnections);

  FiberSetContainer::FiberSet* selectedFiberSet = _outFiberSetContainer.getFiberSet(0);

  if (selectedFiberSet)
  {
    selectedFiberSet->setColor(Vector3(1));
  }

  _outFibersFld->touch();
}


ML_END_NAMESPACE