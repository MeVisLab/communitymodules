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

// $Id: UMDSoDistanceLine.h,v 1.9 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoDistanceLine.h,v $

/*! Inventor node class UMDSoDistanceLine is a line for distance measurements.
// \file    UMDSoDistanceLine.h
// \author  Henry Sonnet / Christian Tietjen
// \date    10/2001
*/

#ifndef _UMDSoDistanceLine
#define _UMDSoDistanceLine


#include "UMDMeasurementInit.h"
#include "UMDSoMeasureTool.h"

class SoCalculator;
class SoLocateHighlight;
class SoExtText2;
class SoTransform;


//! Inventor node class UMDSoDistanceLine is a line for distance measurements;
//! Die Linie besteht, wenn sie eine bestimmte Laenge ueber-
//! schreitet, aus zwei getrennten Zylindern, zwischen denen
//! sich die Abstandszahl sowie auf Wunsch eine Einheit
//! befindet;
//! an den beiden Enden befindet sich jeweils ein Kegel,
//! der beim Ueberfahren mit der Maus aufleuchtet;
//! betaetigt man diesen Kegel, erscheint ein 3D-Dragger
//! (der Klasse \c SoDragger3D) mit dessen Hilfe man das jeweilige
//! Ende der Distance Line verschieben kann;
//!
//! Rückgabe: Länge der Distanz
//! Basisklasse: \c SoMeasureTool
//!
//! Die Interaktionsmoeglichkeiten sind im Header von
//! \c SoMeasureTool beschrieben
class SO_MEASUREMENT_CLASS_SPEC UMDSoDistanceLine : public UMDSoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoDistanceLine);


public:

  //! Constructor
  UMDSoDistanceLine();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  //! the second vertex
  SoSFVec3f endPos;

  //! Wert, bei dessen Unterschreiten die Linie
  //! geschlossen dargestellt wird
  SoSFFloat minDistance;

  //! Länge der Distanzlinie
  SoSFFloat lineLength;

  //! Schaltet Beschriftung ein/aus
  SoSFBool enableText;

  //! Skaliert Dicken des Pfeils
  SoSFFloat scaleLine;


protected:

  //! Destructor
  virtual ~UMDSoDistanceLine();


private:

  //! haengt Knoten in Graphen
  void createDistLine();

  //! gibt einen Gruppenknoten zurueck, der die beiden
  //! Pfeilenden enthaelt
  void createArrows();

  //! Rueckgabe eines Separators mit Text
  void createText();

  //! Positionsaenderungen
  void getTextTrafo();

  //! Setzt den darzustellenden Text
  void getTextString();

  //! additional initializations
  void initDistanceLine();

  //! Feld-Sensoren initialisieren
  void initDistLineFieldSensors();

  //! Welches Objekt wurde getroffen
  SbBool getPickedObjectMouseLeft(const SoPickedPoint *pickedPoint);

  //! called if one of the vertices was changed
  void pointChanged();

  //! die Funktionen zum Bewegen der Pfeilspitzen bei Tastatur-Events
  SbBool setArrowByKeyboard(float x, float y, float z, Taste taste);

  //! \c endPos changed by user
  static void endChangedCB(void *userData, SoSensor* s);

  //! \c enableText changed by user
  static void enableTextChangedCB(void *userData, SoSensor* s);

  //! \c scaleLine changed by user
  static void scaleLineChangedCB(void *userData, SoSensor* s);



  //! Highlight-Knoten zur Hervorhebung der Interaktionsschnittstellen
  SoLocateHighlight *_arrowStartHighlight, *_arrowEndHighlight;

  //! calculates the parameters for the two arrows
  SoCalculator *_arrowCalc;

  //! contains the measure text
  SoExtText2 *_distText;
  //! true if endPos was dragged
  bool _endDraggerDragged;

  //! Die Feld-Sensoren:
  SoFieldSensor *_endPosSens, *_enableTextSens, *_scaleLineSens;

};

#endif
