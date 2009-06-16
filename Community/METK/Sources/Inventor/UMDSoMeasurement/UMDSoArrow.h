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


// $Id: UMDSoArrow.h,v 1.6 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoArrow.h,v $

/*!
// \file    UMDSoArrow.h
// \author  Christian Tietjen
// \date    10/2001
*/

#ifndef _UMDSoArrow
#define _UMDSoArrow

#include "UMDMeasurementInit.h"

class SoCone;
class SoCalculator;
class SoCylinder;
class SoDrawStyle;
class SoFieldSensor;

//! generates an arrow
//! the arrow has a position and a reference point, from where he points at
class SO_MEASUREMENT_CLASS_SPEC UMDSoArrow : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoArrow);


public:

  //! Constructor
  UMDSoArrow();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();
 
  // ######## Fields ##############
  //! Position der Pfeilspitze
  SoSFVec3f position;
  //! von wo der Pfeil gedreht wird (wie beim Wetterhahn)
  SoSFVec3f rotationRef;
  //! length of the arrow - default value is 10
  SoSFFloat length;
  //! beneath this length teh arrow has no shaft
  SoSFFloat minDistance;
  //! display an arrowhead (true by default) or a cylinder
  SoSFBool displayHead;

  //! scale factor to scale size of cones and cylinder
  SoSFFloat scale;

protected:

  //! Destructor
  virtual ~UMDSoArrow();


private: 

  //! im nachhinein überflüssig, wird jetzt aber zum aufrechterhalten der Hierarchie
  //! beim Überprüfen der PickAction benötigt
  SoSeparator *_root;
  //! calculates the diameter of the shaft and the rotation of the arrow
  SoCalculator *_calcStiel, *_rotationCalc;
  //! called if \c displayHead has been changed
  SoFieldSensor* _displayHeadSens;

  //! called if \c scale has been changed
  SoFieldSensor* _scaleSens;


  SoTransform* _versatzSpitze;

  //! the arrowhead
  SoCone* _spitzeSpitz;
  //! the none-arrowhead
  SoCylinder* _spitzeStumpf;

  // ##### Die Funktionen #####
  //! additional initialization
  void initUMDSoArrow();

  //! callback for \c displayHead
  static void displayHeadChangedCB(void* userData, SoSensor* s);

  //! callback for \c scale
  static void scaleChangedCB(void* userData, SoSensor* s);
};

#endif
