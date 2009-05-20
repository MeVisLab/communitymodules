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
//! The ML module class LoadCAT08Data.
/*!
// \file    mlLoadCAT08Data.cpp
// \author  Ola Friman
// \date    2009-05-20
//
// Loads data for the Coronary Artery Tracking competition 2008
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlLoadCAT08Data.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(LoadCAT08Data, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
LoadCAT08Data::LoadCAT08Data ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("LoadCAT08Data::LoadCAT08Data ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();

  // Add fields to the module and set their values.
  _fileStringFld = fields.addString("fileName");
  _fileStringFld->setStringValue("");

  _outputAFld = fields.addBase("outputA");
  _outputAFld->setBaseValue(&_AList);

  _outputBFld = fields.addBase("outputB");
  _outputBFld->setBaseValue(&_BList);

  _outputSFld = fields.addBase("outputS");
  _outputSFld->setBaseValue(&_SList);

  _outputEFld = fields.addBase("outputE");
  _outputEFld->setBaseValue(&_EList);

  _outputCenterLineFld = fields.addBase("outputCenterLine");
  _outputCenterLineFld->setBaseValue(&_centerLineList);

  // Add base output for vessel graph
  _outputGraphFld = fields.addBase("outputGraph");
  _outputGraphFld->setBaseValue(&_outputGraph);


  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Destructor.
//----------------------------------------------------------------------------------
LoadCAT08Data::~LoadCAT08Data () {
  _clearAll();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void LoadCAT08Data::handleNotification (Field *field)
{
  ML_TRACE_IN("LoadCAT08Data::handleNotification ()");

  // Clear all data structures
  _clearAll(); 

  // Get filename
  std::string filename = _fileStringFld->getStringValue();

  // Search filename+path for the last "/"
  int k = 0;
  for(k=(int)filename.length()-1; k>=0; --k) {
    if(filename[k] == '/') {
      break;
    }
  }
  // If no '/' is found something is wrong and k will equal to 0.
  if(k==0) {
    mlDebug("Filename path could not be extracted.")
      return;
  }

  // Extract the path name
  std::string pathname = filename.substr(0,k+1);

  std::ostringstream dataFileName;
  std::ifstream file_to_read;

  // Loop over the 4 defined vessels
  for(int vesselNbr = 0; vesselNbr<4; ++vesselNbr) {

    // Get A-points
    dataFileName << pathname << "vessel" << vesselNbr << "/" << "pointA.txt";
    file_to_read.open(dataFileName.str().c_str());
    if(!file_to_read) {
      mlDebug("Failed to open " << dataFileName.str())
    }

    if(file_to_read) {
      double x,y,z;
      file_to_read >> x >> y >> z;
      vec3 pos(x,y,z);
      _AList.appendItem(XMarker(vec6(pos,0,0,0), vec3(0)));
    }
    dataFileName.str("");     
    file_to_read.close();

    // Get B-points
    dataFileName << pathname << "vessel" << vesselNbr << "/" << "pointB.txt";
    file_to_read.open(dataFileName.str().c_str());
    if(file_to_read) {
      double x,y,z;
      file_to_read >> x >> y >> z;
      vec3 pos(x,y,z);
      _BList.appendItem(XMarker(vec6(pos,0,0,0), vec3(0)));
    }
    dataFileName.str("");     // This is a strange way to clear the dataFileName, but dataFileName.clear() does not work
    file_to_read.close();

    // Get S-points
    dataFileName << pathname << "vessel" << vesselNbr << "/" << "pointS.txt";
    file_to_read.open(dataFileName.str().c_str());
    if(file_to_read) {
      double x,y,z;
      file_to_read >> x >> y >> z;
      vec3 pos(x,y,z);
      _SList.appendItem(XMarker(vec6(pos,0,0,0), vec3(0)));
    }
    dataFileName.str("");     // This is a strange way to clear the dataFileName, but dataFileName.clear() does not work
    file_to_read.close();

    // Get E-points
    dataFileName << pathname << "vessel" << vesselNbr << "/" << "pointE.txt";
    file_to_read.open(dataFileName.str().c_str());
    if(file_to_read) {
      double x,y,z;
      file_to_read >> x >> y >> z;
      vec3 pos(x,y,z);
      _EList.appendItem(XMarker(vec6(pos,0,0,0), vec3(0)));
    }
    dataFileName.str("");     // This is a strange way to clear the dataFileName, but dataFileName.clear() does not work
    file_to_read.close();

    // Get vessel centerline as XMarkers and Vessel Graph
    dataFileName << pathname << "vessel" << vesselNbr << "/" << "reference.txt";
    file_to_read.open(dataFileName.str().c_str());
    if(file_to_read) {
      int pointNbr = 0;
      // Create a new vessel graph edge
      VesselEdge *graphEdge = new VesselEdge;
      VesselNode *rootNode = NULL;
      do {

        // Extract point and radius
        double x,y,z,r,foo;
        file_to_read >> x >> y >> z >> r >> foo;
        vec3 pos(x,y,z);

        // Add to XMarker list
        _centerLineList.appendItem(XMarker(vec6(pos,0,0,0), vec3(0)));

        // If this is the first point, add new root to the Vessel Graph
        if(0 == pointNbr) {
          rootNode = new VesselNode(pos);               // Create new node
          _outputGraph.addNode(rootNode);                // Add it to the graph
          GraphRoot *gRoot = new GraphRoot(rootNode);    // Make the new node a root
          _outputGraph.addRoot(gRoot);
        }
        // Create a skeleton and add it to the edge
        graphEdge->addSkeleton(Skeleton(pos, r, r));

        // Increment
        ++pointNbr;
      }while(!file_to_read.eof());

      // Finish generating the vessel graph by adding the edge and end node to the graph
      VesselNode *endNode = new VesselNode(graphEdge->backSkeleton()->pos);
      _outputGraph.addNode(endNode);
      // Connect the vessel edge
      graphEdge->setPred(rootNode);
      graphEdge->setSucc(endNode);
      // Add the vessel edge to the graph
      _outputGraph.attachIdEdge(graphEdge,rootNode->getId(),endNode->getId());

    }
    dataFileName.str("");     // This is a strange way to clear the dataFileName, but dataFileName.clear() does not work
    file_to_read.close();
    file_to_read.clear();     // Another strange thing, clear() must be called when we read the file to eof, otherwise we cannot reopen file_to_read later.
  }

  _outputAFld->notifyAttachments();
  _outputBFld->notifyAttachments();
  _outputSFld->notifyAttachments();
  _outputEFld->notifyAttachments();
  _outputCenterLineFld->notifyAttachments();
  _outputGraphFld->notifyAttachments();
}

void LoadCAT08Data::_clearAll() 
{
  // Clear current XMarkerLists and Vesel Graph
  _AList.clear();
  _BList.clear();
  _SList.clear();
  _EList.clear();
  _centerLineList.clear();
  _outputGraph.clearGraph();
}



ML_END_NAMESPACE

