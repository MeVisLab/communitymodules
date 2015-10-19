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

// $Id: UMDSoRuler.h,v 1.9 2003/02/19 16:37:15 okonrad Exp $
// $Source: /projects/repositories/mevis/Libraries/std/Inventor/SoMeasurement/UMDSoRuler.h,v $

/*! Inventor node UMDSoRuler is a ruler which can be moved by selections/interactions.
// \file    UMDSoRuler.h
// \author  Henry Sonnet / Christian Tietjen
// \date    10/2001
*/


#ifndef _UMDSoRuler
#define _UMDSoRuler

#include "UMDMeasurementInit.h"
#include "UMDSoMeasureTool.h"

class SoExtText2;
class SoLocateHighlight;
class SoTranslate1Dragger;
class SoTranslation;


//! Inventor node UMDSoRuler is a ruler which can be moved by selections/interactions.
//! Am Anfangs- bzw. Endpunkt des Lineals befindet sich
//! jeweils eine Kugel, bei deren Betaetigung ein Dragger erscheint,
//! mit dessen Hilfe das Lineal in der Laenge veraendert werden kann.
//! Beim Auswählen der Verbindungslinie erscheint ein Dragger zum
//! verschieben des Nullpunktes.
//!
//! Rückgabe: Länge des Lineals
//! Basisklasse: \c SoMeasureTool
//!
//! Die Interaktionsmoeglichkeiten sind im Header von
//! \c SoMeasureTool beschrieben
class SO_MEASUREMENT_CLASS_SPEC UMDSoRuler : public UMDSoMeasureTool {
  //! macro that defines extended methods
  SO_NODE_HEADER(UMDSoRuler);


public:

  //! Constructor
  UMDSoRuler();

  //! must be called first to initialize the class in OpenInventor
  static void initClass();

  //! the second vertex
  SoSFVec3f endPos;

  //! Wert, bei dessen Unterschreiten Linie geschlossen dargestellt
  //! wird
  SoSFFloat minDistance;

  //! Länge des Lineals
  SoSFFloat lineLength;


protected:

  //! Destructor
  virtual ~UMDSoRuler();


private:

  //! Initialisierung der Variablen und Szenen-Graphen mit Knoten
  //! bestuecken
  void initRuler();
  //! Initialisierung der Variablen und Szenen-Graphen mit Knoten
  //! bestuecken
  void createRuler();

  //! Feld-Sensoren initialisieren
  void initRulerFieldSensors();

  //! erstellt einen LevelOfDetail-Knoten, der für die
  //! Detailierung der Strokes zuständig ist
  void createLevelOfStrokes();

  //! gibt einen Gruppen-Knoten zurueck,
  //! der den Text fuer die Masseinheit enthaelt
  void createUnitGroup();

  //! creates the interaction-part at \c startPos
  //SoSeparator* createDragStartSep();
  //! creates the interaction-part at \c endPos
  //SoSeparator* createDragEndSep();
  //! creates the interaction-part to move the strokes
  SoSeparator* createDragMoveScaleSep();

  //! Skalenstriche werden bestimmt und positioniert
  void getStrokes();

  //! gibt eine Separator zurueck, der die Ziffern
  //! des Lineals enthaelt
  SoAnnotation* getDigit(float digit);

  //! Bestimmen der neuen Positionen
  void getUnitPos();

  //! Setzt den darzustellenden Text
  void getTextString();

  //! Welches Objekt wurde getroffen
  SbBool getPickedObjectMouseLeft(const SoPickedPoint* pickedPoint);

  //! die Funktionen zum Bewegen der Pfeilspitzen bei Tastatur-Events
  SbBool setArrowByKeyboard(float x, float y, float z, Taste taste);

  //! Aktualisierungen veranlassen
  void pointChanged();

  //! Callback for the Dragger at \c startPos
  //static void draggerStartCB(void* userData, SoDragger* dragger);
  //! Callback for the Dragger at \c endPos
  //static void draggerEndCB(void* userData, SoDragger* dragger);
  //! Callback for the Dragger for moving the strokes
  static void draggerMoveScaleCB(void* userData, SoDragger* dragger);

  //! callback for \c endPos
  static void endChangedCB(void *userData, SoSensor* s);
  //! callback for \c minDistance
  static void minDistanceChangedCB(void *userData, SoSensor* s);


  //! null position of the strokes
  float _strokeCenterPos;
  //! null position of the strokes
  SoTranslation *_firstStroke;
  //! groups with several level of details
  SoSeparator *_strokeGroup0, *_strokeGroup1, *_strokeGroup2;
  //! groups with several level of details
  SoSeparator *_strokeGroup3, *_strokeGroup4, *_strokeGroup5, *_strokeGroup6;
  //! contains the strokes
  SoSeparator *_strokesSep;
  //! \c _oneStrokeSep contains a big stroke with a digit, \c _oneSmallStrokeSep contains a small stroke without a digit
  SoSeparator *_oneStrokeSep, *_oneSmallStrokeSep;
  //! contains the digits
  SoSeparator *_digitsSep;
  //! contains the measure text
  SoExtText2 *_unitText;
  //! shows the unit if desired
  SoSwitch *_unitSwitch;
  //! true if endPos was dragged
  bool _endDraggerDragged;

  //! moves the vertices
  SoTransform *_sphereTrafoStart, *_sphereTrafoEnd;

  //! dragger to move the strokes
  SoTranslate1Dragger *_dragMoveScale;
  bool _changedByDragger;

  //! Highlight-Knoten zur Hervorhebung der Interaktionsschnittstellen
  SoLocateHighlight *_sphereStartHighlight, *_sphereEndHighlight, *_lineHighlight;

  //! calculates line length, diameter, endpoints
  SoCalculator *_lineCalc;
  //! composes rotation for the strokes and the stroke-dragger
  SoComposeRotationFromTo *_rotLine, *_rotDragger;
  //! needed to define an own dragger (avtivePart, inactivePart)
  SoSeparator *_reglerAktiv, *_reglerInaktiv;

  //! \c endPos was changed
  SoFieldSensor* _endPosSens;
  //! \c minDistance was changed
  SoFieldSensor* _minDistanceSens;
};

#endif
