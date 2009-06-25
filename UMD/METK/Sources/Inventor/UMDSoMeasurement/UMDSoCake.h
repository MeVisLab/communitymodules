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


// $Id: UMDSoCake.h,v 1.5 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoCake.h,v $

/*! Inventor node class to generate a cylinder, from which a part is resected.
// \file    UMDSoCake.h
// \author  Christian Tietjen
// \date    11/2001
*/


#ifndef _UMDSoCake
#define _UMDSoCake

#include "UMDMeasurementInit.h"

class SoCalculator;
class SoFieldSensor;
class SoIndexedFaceSet;
class SoVertexProperty;

//! Inventor node class to generate a cylinder, from which a part is resected.
class SO_MEASUREMENT_CLASS_SPEC UMDSoCake : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoCake);


public:

  //! Constructor
  UMDSoCake();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  //! the angle in arc resp. degree
  SoSFFloat angleArc, angleDegree;
  //! the measures of the cake
  SoSFFloat radius, height;


protected:

  //! Destructor
  virtual ~UMDSoCake();


private:

  //! Feld-Sensoren initialisieren
  void initCakeFieldSensors();

  //! additional initializations
  void initUMDSoCake();

  //! creates the cake tree
  void initCakeTree();

  //! refreshes the cake after changes
  void cakeValueChanged();

  //! true if the difference between the two values is smaller then 0.0001
  bool areEqual(const float& float1, const float& float2);

  //! \c radius, \c height or \c angleArc has been changed
  static void valueChangedCB(void* userData, SoSensor* s);
  //! \c angleDegree has been changed
  static void degreeValueChangedCB(void* userData, SoSensor* s);

  //! the root knot
  SoSeparator* _root;

  //! the face set
  SoIndexedFaceSet *_flaeche;
  //! the vertex property
  SoVertexProperty *_vertexProp;
  //! the coordinates
  SoMFVec3f _coords;

  //! the field sensors
  SoFieldSensor* _radiusSens, *_heightSens, *_angleArcSens, *_angleDegreeSens;
};

#endif
