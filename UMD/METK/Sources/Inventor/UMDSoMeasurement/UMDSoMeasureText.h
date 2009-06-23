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
// |      Computes the distance between two points       |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoMeasureText.h,v 1.2 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoMeasureText.h,v $

/*!
// \file    UMDSoMeasureText.h
// \author  Christian Tietjen
// \date    09/2002
*/

#ifndef _UMDSoMeasureText
#define _UMDSoMeasureText


#include "UMDMeasurementInit.h"

class SoEventCallback;
class SoFieldSensor;
class SoSwitch;
class SoTransform;


class SO_MEASUREMENT_CLASS_SPEC UMDSoMeasureText : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoMeasureText);
  

public:
  
  //! Constructor
  UMDSoMeasureText();
  
  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  //! text is coupled into 3D scene
  SoSFBool isCoupled;

  //! content of the text
  SoSFString string;

  //! color of the text background
  SoSFColor color;

protected:

  //! Destructor
  virtual ~UMDSoMeasureText();


private:
  
  SoSwitch *_switch2D3D;

  unsigned long _lastClick;

  //! switches between 2D and 3D translation mode
  SoFieldSensor *_isCoupledSensor;
  static void isCoupledCB(void* userData, SoSensor*);
  void toggleTranslationMode();

  //! Callback for mousebutton-event
  static void mousePressedCB(void* userData, SoEventCallback* eventCB);

};

#endif
