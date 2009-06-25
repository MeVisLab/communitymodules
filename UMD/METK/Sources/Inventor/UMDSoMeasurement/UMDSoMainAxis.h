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
// |        Main Axis Computation of a Point Sets        |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoMainAxis.h,v 1.15 2003/09/05 16:14:44 milo Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoMainAxis.h,v $

/*! The Inventor node class wrapping/interfacing the MainAxis class, derived from SoAutoMeasureTool.
// \file    UMDSoMainAxis.h
// \author  Christian Tietjen
// \date    09/2002
*/

#ifndef _UMDSoMainAxis
#define _UMDSoMainAxis


#include "UMDMeasurementInit.h"
#include "UMDSoAutoMeasureTool.h"

class UMDSoDistanceLine;
class SoFieldSensor;
class SoIndexedLineSet;

//! The Inventor node class wrapping/interfacing the MainAxis class, derived from SoAutoMeasureTool.
class SO_MEASUREMENT_CLASS_SPEC UMDSoMainAxis : public UMDSoAutoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoMainAxis);


public:

  //! Constructor
  UMDSoMainAxis();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  // ######## Fields ##############

  //! the three inertia axis
  SoSFVec3f xAxis, yAxis, zAxis;

  //! returns the three extensions of the object-orientated bounding box and the maximum diameter
  SoSFFloat xDiameter, yDiameter, zDiameter, maxDiameter;

  //! what could that be?
  SoSFVec3f baryCenter, midPoint;

  //! the entity, in which the measure of the diameters is shown
  SoSFEnum unit;

  //! decides whether the OBB is shown or not
  SoSFBool displayBoundingBox;
  //! the linewidth of the OBB
  SoSFFloat lineWidth;

  //! decides whether the main axis are shown or not
  SoSFBool displayMainAxis;
  //! only the largest value is shown
  SoSFBool largestValueOnly;
  //! decides whether the origin is in the barycenter or in the midpoint
  SoSFEnum axisOrigin;

protected:

  //! Destructor
  virtual ~UMDSoMainAxis();


private:

  //! the enum for \c axisOrigin
  enum MainAxis_AxisOrigin {//ILAB4
                            barycenter=0,
                            midpoint  =1,
                            //ILAB5
                            ORIGIN_BARYCENTER=barycenter,
                            ORIGIN_MIDPOINT  =midpoint
                            };

  //! indicates the first member processing to set the \c color
  bool _firstInit;

  //! the sensor for \c displayBoundingBox
  SoFieldSensor* _displayBoundingBoxSensor;
  //! the sensor for \c displayMainAxis
  SoFieldSensor* _displayMainAxisSensor;
  //! the sensor for \c largestValueOnly
  SoFieldSensor* _largestValueOnlySensor;
  //! the sensor for \c displayBoundingBox
  SoFieldSensor* _axisOriginSensor;
  //! stretches a vector with the direction of \c vector to the length \c length
  SbVec3f stretchVector(const SbVec3f vector, const float length);

  //! inserts a distance line at the given \c midpoint and the length and direction of \c axis
  UMDSoDistanceLine*   insertDistanceLine(SbVec3f midpoint, SbVec3f axis);
  //! inserts a BB at the given \c midpoint and the given dimensions
  SoIndexedLineSet* insertBoundingBox(SbVec3f midPoint, SbVec3f xAxis, SbVec3f yAxis, SbVec3f zAxis);

  //! called by \c calculate, performs the main axis computation and returns whether it was successful
  bool _calculate();
  //! called after one of the sensor has been touched or a calculation was successful
  void display();

};

#endif
