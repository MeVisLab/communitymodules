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
// |  Converts any image into a pointset                 |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoThresholdToPointSet.h,v 1.7 2004/08/02 11:32:20 florian Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoThresholdToPointSet.h,v $

/*! Inventor node UMDSoThresholdToPointSet generating a pointset from a given image.
// \file    UMDSoThresholdToPointSet.h
// \author  Christian Tietjen
// \date    09/2002
*/
#ifndef _UMDSoThresholdToPointSet
#define _UMDSoThresholdToPointSet

//! size of the buffer for fast caching of the point set
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10000
#endif


#include "UMDMeasurementInit.h"
#include <SoSFMLImage.h>

class SoFieldSensor;
class SoPointSet;
class SoVertexProperty;


//! Inventor node UMDSoThresholdToPointSet generating a pointset from a given image.
//! For each voxel between a given threshold a point will be created.
//! The set can be bordered by a threshold.
class SO_MEASUREMENT_CLASS_SPEC UMDSoThresholdToPointSet : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoThresholdToPointSet);


public:

  //! Constructor
  UMDSoThresholdToPointSet();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  // ######## Fields ##############

  //! min and max for threshold-interval
  SoSFFloat minValue, maxValue;

  //! use voxel color for each point or no color
  SoSFBool setColor;

  //! input image
#ifdef ILAB5
  SoSFMLImage inImage;
#else
  SoSFMLImage image;
#endif

  //! direct access on the extracted point set
  //! is empty if \c rootInput contains no valid scenegraph
 SoVertexProperty* vertexProperty;

  //! forces the computation
  void calculate();


protected:

  //! Destructor
  virtual ~UMDSoThresholdToPointSet();

private:

  //! internal representation of the point set
  SoPointSet* _pointSet;
  //! listen whether the input has been changed
  SoFieldSensor *_minValueSens, *_maxValueSens, *_imageSens, *_setColorSens;

  //! callback of field-sensor
  static void _fieldChangedCB(void* userData, SoSensor* s);

};

#endif
