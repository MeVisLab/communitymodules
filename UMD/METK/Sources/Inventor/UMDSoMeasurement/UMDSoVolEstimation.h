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

// $Id: UMDSoVolEstimation.h,v 1.8 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoVolEstimation.h,v $

/*!
// \file    UMDSoVolEstimation.h
// \author  Christian Tietjen
// \date    11/2001
*/


#ifndef _UMDSoVolEstimation
#define _UMDSoVolEstimation


#include "UMDMeasurementInit.h"
#include "UMDSoMeasureTool.h"

class SoCalculator;
class SoDragger;
class SoExtText2;
class SoLocateHighlight;
class SoTransform;
class SoTransformerDragger;


//! UMDSoVolEstimation ist ein Tool zur approximativen Messung
//! eines Volumens.
//! Anders als bei den anderen Vermessungstools wird das
//! berechnete Volumen hier nur angezeigt, wenn das unitFlag
//! gesetzt ist.
//! Diese Klasse wird eigentlich nur zum interagieren des
//! Hüllkörpers in der Measurement-Applikation benutzt.
//! Der Hüllkörper zur Beschreibung des Volumens läßt sich
//! beliebig per Tastatur, Dragger oder Mausklick im Raum
//! verschieben.
//! Mit einem SoTransformerDragger lassen sich alle wichtigen
//! transformationen (außer Scherung) auf dem Hüllkörper
//! ausführen
//! Rückgabe: das Volumen des Volumens
//! Basisklasse: \c SoMeasureTool
//! Die Interaktionsmoeglichkeiten sind im Header von 
//! \c SoMeasureTool beschrieben
class SO_MEASUREMENT_CLASS_SPEC UMDSoVolEstimation : public UMDSoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoVolEstimation);
  

public:
  
  //! Constructor
  UMDSoVolEstimation();
  
  //! must be called first to initialize the class in OpenInventor
  static void initClass();
  
  //! Volumen ausgeben
  SoSFFloat absVolume;
  //! Volumen des Ellipsoiden von Applikation berechnet
  //! Angabe immer in cmm
  SoSFFloat externVolume;
  
  // ######## Fields ##############
  
  //! Volumen wird extern berechnet
  SoSFBool volExternFlag;

  //! Rotation des Hüllkörpers
  SoSFRotation rotation;
  //! Skalierung des Hüllkörpers
  SoSFVec3f scaleFactor;

  //! Art des Hüllkörpers
  SoSFEnum rasterFunction;


protected:

  //! Destructor
  virtual ~UMDSoVolEstimation();


private: 
  
  //! additional initializations
  void initVolEstimation();
  
  //! Feld-Sensoren initialisieren
  void initVolEstimationFieldSensors();
  
  //! haengt Knoten in Graphen
  void createVolEstimation();
  
  //! creates the SoTransformerDragger
  void createDragger();

  //! gibt einen Gruppenknoten zurueck, der den 
  //! Hüllkörper enthaelt
  void createEllipsoid();

  //! Rueckgabe eines Separators mit Text
  void createText();
  
  //! Setzt den darzustellenden Text
  void getTextString();
  
  //! \c startPos has been changed
  void pointChanged();

  //! Welches Objekt wurde getroffen
  SbBool getPickedObjectMouseLeft(const SoPickedPoint* pickedPoint);
  
  //! die Funktionen zum Bewegen der Pfeilspitzen bei Tastatur-Events
  SbBool setArrowByKeyboard(float x, float y, float z, Taste taste);

  //! Der Callback für den Dragger
  static void valueChangedCB(void* userData, SoDragger* dragger);
  
  //! the scale factor was changed
  static void scaleExternChangedCB(void* userData, SoSensor* s);
  //! the rotation was changed
  static void rotationExternChangedCB(void* userData, SoSensor* s);
  //! the raster function was changed
  static void rasterFunctionChangedCB(void* userData, SoSensor* s);

  //! calcs the intern volume if needed
  SoCalculator *_volumeCalc;

  //! Highlight-Knoten zur Hervorhebung der Interaktionsschnittstellen
  SoLocateHighlight *_sphereHighlight;

  //! the picked point
  SoSFVec3f *_pickedPoint;


  //! Dragger to perform the tarnsformations
  SoTransformerDragger *_transformDragger;
  //! setzt die Transformierungen um
  SoTransform *_transform;

  //! enthält die Maßzahl
  SoExtText2 *_volText;
  //! verschiebt die Maßzahl
  SoTransform *_textTrafo2;
  //! speichert das intern berechnete Volumen
  SoSFFloat _internVolume;

  //! zeigt einen der Hüllkörper an
  SoSwitch *_funcSwitch;
  //! Auswahl der Hüllkörper
  enum SoVolEst_function {Sphere, Box, Cylinder};

  //! \c volExternFlag was changed
  SoFieldSensor* _volExternFlagSens;
  //! \c externVolume was changed
  SoFieldSensor* _externVolumeSens;

  //! \c rasterFunction was changed
  SoFieldSensor* _rasterFunctionSens;

  //! \c rotation was changed
  SoFieldSensor* _externRotSens;
  //! \c scaleFactor was changed
  SoFieldSensor* _externScaleSens;

  //! the values were changed intern
  bool _internChanged;
};

#endif
