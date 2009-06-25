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
// |                Computes the angle                   |
// |                                                     |
// +-----------------------------------------------------+

// $Id: UMDSoAngleLines.h,v 1.7 2003/02/19 16:36:42 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoAngleLines.h,v $

/*! The Inventor node class UMDSoAngleLines derived from SoSeparator and UMDSoMeasureTool, implementing two lines for angle measurements.
// \file    UMDSoAngleLines.h
// \author  Henry Sonnet / Christian Tietjen
// \date    09/2002
*/


#ifndef _UMDSoAngleLines
#define _UMDSoAngleLines


#include "UMDMeasurementInit.h"
#include "UMDSoMeasureTool.h"

class UMDSoCake;
class SoCalculator;
class SoComposeRotation;
class SoComposeRotationFromTo;
class SoDecomposeRotation;
//class SoDragger;
//class SoDragger3D;
class SoIndexedFaceSet;
class SoLocateHighlight;
class SoRotation;
class SoExtText2;
class SoTransform;
class SoVertexProperty;



//! The Inventor node class UMDSoAngleLines derived from SoSeparator and UMDSoMeasureTool implementing two lines for angle measurements;
//! beide Linien besitzen die gleiche Anfangsposition;
//! an dieser Anfangsposition befindet sich eine Kugel,
//! bei deren Picken ein Dragger erscheint, der beide
//! Linien gleichzeitig bewegt.
//! An den Endpunkten der beiden Linien befindet sich
//! jeweils ein Kegel, bei dessen Picken ein \c SoDragger3D
//! erscheint, der das entsprechende Linienende bewegt.
//! Eine der beiden Linien ist in der Mitte
//! geteilt, diese Luecke dient der Plazierung des Winkels;
//!
//! Rückgabe: eingeschlossener Winkel der beiden Linien
//! Basisklasse: \c SoMeasureTool
//!
//! Die Interaktionsmoeglichkeiten sind im Header von
//! \c SoMeasureTool beschrieben
class SO_MEASUREMENT_CLASS_SPEC UMDSoAngleLines : public UMDSoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoAngleLines);


public:

  //! Konstruktor
  UMDSoAngleLines();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  //! zweiter und dritter Eckpunkt des Werkzeugs
  SoSFVec3f endPos, connectPos;

  //! Wert, bei dessen Unterschreiten Linie geschlossen dargestellt wird
  SoSFFloat minDistance;

  //! der Winkels zwischen den beiden Linien
  SoSFFloat angleDegree, angleArc;

protected:

  //! Destruktor
  virtual ~UMDSoAngleLines();


private:

  //! gibt einen Gruppenknoten zurueck, der die
  //! Pfeilenden sowie eine Kugel am gemeinsamen Startpunkt enthaelt;
  void createInteractionSep();

  //! haengt Knoten in Graphen
  void createAngleLines();

  //! Rueckgabe eines Separators mit Text
  void createText();

  //! Erzeugen der Rotationsengines etc
  void createRotationCalcs();

  //! creates the orientation areas
  SoSeparator* createAreasSep();

  //! verschiebt den Text an die richtige Position
  void getTextTrafo();

  //! Setzt den darzustellenden Text
  void getTextString();

  //! additional initializations
  void initAngleLines();

  //! Feld-Sensoren initialisieren
  void initAngleLinesFieldSensors();

  //! Welches Objekt wurde getroffen
  SbBool getPickedObjectMouseLeft(const SoPickedPoint *pickedPoint);

  //! Flaechen zur besseren Orientierung
  void getAreas();

  //! die Funktionen zum Bewegen der Pfeilspitzen bei Tastatur-Events
  SbBool setArrowByKeyboard(float x, float y, float z, Taste taste);

  //! \c endPos changed by user
  static void endChangedCB(void *userData, SoSensor* s);
  //! \c connectPos changed by user
  static void connectChangedCB(void *userData, SoSensor* s);

  //! called if one of the vertices was changed
  void pointChanged();

  // ##### Die Instanz-Variablen #######

  //! calculates the parameters for the four arrows and the angle
  SoCalculator *_arrowCalc, *_angleCalc;

  //! mit Hilfe von SoComposeRotationFromTo Winkel berechnen lassen, erspart Ausnahmeregelungen
  SoComposeRotationFromTo *_computeAngleCRFT;
  //! and vice versa
  SoDecomposeRotation *_computeAngle;

  //! alles zur Rotation der Winkelebebene
  SoComposeRotationFromTo *_kreisInEbeneCRFT, *_kreisDrehenCRFT;
  //! intermediate rotation
  SoRotation *_rotTemp;

  //! the measure text
  SoExtText2 *_angleText;
  //! moves the cube needed for the text-LOD
  SoTransform *_cubeTrafo;

  //! shows the areas if activ
  SoSwitch *_showAreasSwitch;
  //! vertex property for \c _areas
  SoVertexProperty* _areaProps;
  //! the areas
  SoIndexedFaceSet *_areas;
  //! the tilecircle between the angles
  UMDSoCake* _teilkreis;

  //! Highlight-Knoten zur Hervorhebung der Interaktionsschnittstellen
  SoLocateHighlight *_arrowStartHighlight, *_arrowEndHighlight, *_sphereHighlight;

  //! Die Feld-Sensoren:
  SoFieldSensor* _endPosSens, *_connectPosSens;
};

#endif
