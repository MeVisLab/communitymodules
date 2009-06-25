/*
 * Copyright 2001, MeVis gGmbH
 * ALL RIGHTS RESERVED
 *
 * THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
 * INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
 * DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
 * PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
 *
 */



// +-----------------------------------------------------+
// |                                                     |
// |  Efficient Distance Computation between Point Sets  |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoMinimalDistance.h,v 1.9 2003/09/05 16:14:44 milo Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoMinimalDistance.h,v $

/*! Inventor node UMDSoMinimalDistance computing the shortest euclidean distance between two pointsets.
// \file    UMDSoMinimalDistance.h
// \author  Christian Tietjen
// \date    09/2002
*/

#ifndef _UMDSoMinimalDistance
#define _UMDSoMinimalDistance

#include "UMDMeasurementInit.h"
#include "UMDSoAutoMeasureTool.h"


class UMDSoDistanceLine;
class SoFieldSensor;
class SoSwitch;

//! Inventor node UMDSoMinimalDistance computing the shortest euclidean distance between two pointsets.
//! Based on: "Efficient Distance Computation between Non-Convex Objects" by Sean Quinlan
//! see: graphics.stanford.edu/courses/cs468-01-winter/papers/q-edcbnco-94.ps
//! This class integrates the Non-Inventor class \c MinimalDistance into ILab
class SO_MEASUREMENT_CLASS_SPEC UMDSoMinimalDistance : public UMDSoAutoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoMinimalDistance);


public:

  //! Constructor
  UMDSoMinimalDistance();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  // ######## Fields ##############

  //! starts the distance computation
  SoSFTrigger computeMinimalDistance;
  //! if this is true, the computation starts automatically if a valid input is given
  SoSFBool autoCompute;

  //! contain the second object
#ifdef ILAB5
  SoSFNode inObject2Node;
#else
  SoSFNode inputObject2;
#endif

  //! the selected point in \c inputObject
  SoSFVec3f startPos;
  //! the selected point in \c inputObject2
  SoSFVec3f endPos;
  //! the computed distance
  SoSFFloat distance;

  //! the unit of the distance measure
  SoSFEnum unit;

  SoSFFloat scaleLine;

  //! Wert, bei dessen Unterschreiten die Linie
  //! geschlossen dargestellt wird
  SoSFFloat minDistance;


  //! Decides whether the computing parameters should be generated automatically or not.
  //! By default it is FALSE, because the computing method ist not optimal.
  SoSFBool autoParam;
  //! This parameter describes, how many points max. can be put into one subsphere,
  //! but the designed value can be lower.
  //! The default value is 2000.
  SoSFInt32 autoEntries;
  //! Spheres can max. conclude ... points
  SoSFInt32 manEntries;
  //! The partition amounts ...
  SoSFInt32 manPartition;

  //! dumps the console with additional infos
  SoSFBool verbose;
  //! decides whether the transparent spheres should be displayed
  SoSFBool displaySpheres;
  //! The sizes of the point sets
  SoSFInt32 sizeOfPointSet1, sizeOfPointSet2;

  //! describes the maintainable error between 0% and 100%. The default value is 0%.
  SoSFFloat error;

protected:

  //! Destructor
  virtual ~UMDSoMinimalDistance();


private:

  //! \c _spheres is just for debugging aims
  SoSeparator *_spheres;

  //! the distance line
  UMDSoDistanceLine *_distanceLine;

  //! raise an alarm on input node change
  SoFieldSensor *_inputSens2;
  //! the callback method for \c _inputSens2
  static void inputCB(void *userData, SoSensor*);

  //! starts distance computation
  SoFieldSensor *_computeMinimalDistanceSens;
  //! the callback method for \c _computeMinimalDistanceSens
  static void computeMinimalDistanceCB(void *userData, SoSensor*);
  //! computes the distance instead of \c _calculate to allow for the \c computeMinimalDistance trigger
  bool _computeMinimalDistance();

  //! notifies if \c displaySpheres was changed
  SoFieldSensor *_displaySpheresSensor;
  //! contain the two point sets
  float *_pointSet1, *_pointSet2;
  //! size of the two point sets
  long _size1, _size2;

  // the derived method from \c SoAutoMeasureTool
  bool _calculate();
  // the derived method from \c SoAutoMeasureTool
  void display();

};

#endif
