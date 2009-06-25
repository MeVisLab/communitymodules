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
// |       Computes the angle between two objects        |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoAngleToObjects.h,v 1.10 2003/09/23 17:24:33 milo Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoAngleToObjects.h,v $

/*! Computes the angle between two objects on the basis of the main axis computation.
// \file    UMDSoAngleToObjects.h
// \author  Christian Tietjen
// \date    09/2002
*/

#ifndef _UMDSoAngleToObjects
#define _UMDSoAngleToObjects

#include "UMDMeasurementInit.h"
#include "UMDSoAutoMeasureTool.h"


class UMDSoAngleLines;
class SoFieldSensor;

//! Computes the angle between two objects on the basis of the main axis computation.                |
//! The result is the angle between the two longest main axis of the two objects.                      |
class SO_MEASUREMENT_CLASS_SPEC UMDSoAngleToObjects : public UMDSoAutoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoAngleToObjects);


public:

  //! Constructor
  UMDSoAngleToObjects();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  // ######## Fields ##############

  //! the second input object
#ifdef ILAB5
  SoSFNode inObject2Node;
#else
  SoSFNode inputObject2;
#endif

  //! the angle between the axes
  SoSFFloat angleDegree;

  //! the three points defining the angle
  SoSFVec3f startPos, endPos, connectPos;


protected:

  //! Destructor
  virtual ~UMDSoAngleToObjects();


private:

  //! these group contains the second object
  SoSeparator *_inputSep2;
  //! the angle lines
  UMDSoAngleLines *_angleLines;

  //! raise an alarm on input node change
  SoFieldSensor *_inputSens2;
  //! the callback method for \c _inputSens2
  static void inputCB(void *userData, SoSensor*);

  //! the derived method from \c SoAutoMeasureTool
  bool _calculate();
  //! the derived method from \c SoAutoMeasureTool
  void display();
  //! delivers the largest main axis if \c node contains a valid point set
  bool getLargestMainAxis(SoGroup *node, SbVec3f &vec1, SbVec3f &vec2);
};

#endif
