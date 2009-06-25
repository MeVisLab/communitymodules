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

// $Id: UMDSoAutoMeasureTool.h,v 1.7 2003/09/23 17:24:33 milo Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoAutoMeasureTool.h,v $

/*! The inventor node class UMDSoAutoMeasureTool which is the base class for all automatic measurement tools.
// \file    UMDSoAutoMeasureTool.h
// \author  Christian Tietjen
// \date    09/2002
*/


#ifndef _UMDSoAutoMeasureTool
#define _UMDSoAutoMeasureTool

#include "UMDMeasurementInit.h"

class SoFieldSensor;

//! This is the base class for all automatic measurement tools.
class SO_MEASUREMENT_CLASS_SPEC UMDSoAutoMeasureTool : public SoSeparator {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoAutoMeasureTool);


public:

  //! Constructor
  UMDSoAutoMeasureTool();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  // ######## Fields ##############

  //! is true if the calculated results are valid
  SoSFBool resultsValid;

  //! object, for which the calculations will be done
#ifdef ILAB5
  SoSFNode inObject1Node;
#else
  SoSFNode inputObject;
#endif

  //! the color of the tool(s), which describe the results
  SoSFColor color;
  //! decides whether the \c inputObject will be displayed or not
  SoSFBool displayInput;
  //! decides whether the unit of the tool measure will be displayed or not
  SoSFBool displayToolMeasure;
  SoSFBool displayUnit;

  //! decides whether the name of the tool measure will be displayed or not
  SoSFBool displayName;
  //! the name of the tool
  SoSFString toolName;

  //! display the tool only if \c resultsValid is true
  SoSFBool displayToolOnlyOnValid;

  //! indicates that \c inputObject contains a scene graph which contains every point only one time
  //! this adjustment will save computation time
  //! Attention!: this feature is only given by special pointsets! shapes contain every point six times,
  //! because the points belong to several triangles
  //SoSFBool isUnique;

  //! forces the computation, returns true if computation was successful
  bool calculate();


protected:

  //! Destructor
  virtual ~UMDSoAutoMeasureTool();

  //! the available units, must be synchronized with \c MeasureTool_Unit from the class \c SoMeasureTool
  enum AutoMeasureTool_Unit {deg, mm, cmm, cm, m, ml, l};

  //! containing the measure tools and the input objects. if there are more input objects, they must be collected in \c _inputSep
  SoSeparator *_toolSep, *_inputSep1, *_inputSep2;
  //! connected with \c displayInput
  SoSwitch* _displayInputSwitch;

  //! the sensor for \c displayToolOnlyOnValid
  SoFieldSensor* _displayToolOnlyOnValidSensor;
  //! the sensor for \c displayInput
  SoFieldSensor* _displayInputSensor;
  //! the sensor for \c inputObject
  SoFieldSensor* _inputObjectSensor;

  //! called if inputObject has been changed, calls the non-static method \c inputChanged
  static void  inputChangedCB(void *userData, SoSensor*);
  //! called if the ouput should be changed, calls the non-static method \c outputChanged
  static void outputChangedCB(void *userData, SoSensor*);
  //! called by \c inputChangedCB
  void  inputChanged();
  //! called by \c outputChangedCB
  void outputChanged();
  //! generates a float array out of an scene graph
  void getPointSet(SoGroup* inputGroup, float* &pointSet, long &size);

  //! this method will be called if the output scene must be redrawn, must be implemented by the deriving classes
  virtual void display() {};
  //! called by the public method \c calculate, perform the computation
  virtual bool _calculate() {return false;};

};

#endif
