#ifndef LAB_FIELD_H
#define LAB_FIELD_H

//! Class for fields for 2D-Labeling and standard operators which can be applied to them.
//! \file labField.h

/*! \class labField
\brief Class for fields for 2D-Labeling and standard operators which can be applied to them.
\author Konrad Muehler
\date 04-2008
*/


// Local includes
#include "METK2DLabelingSystem.h"

#include <vector>
#include <map>
#include "kBasics.h"

class METK2DLABELING_EXPORT labField {
public:
  labField              ( );                                              //!<Konstruktor
  labField              ( unsigned int iSizeX, unsigned int iSizeY);      //!<Konstruktor
  ~labField             ( void );                                         //!<Destruktur
  int fillWith          ( float fValue );                 //!<Fuellt das Feld mit dem Wert von \p fValue
  int set               ( labField* oField);
  int setValueAt        ( unsigned int iPos, float fValue );        //!<Setzt an der Feldposition \p iPos den Wert von \p fValue
  int setValueAt        ( unsigned int iX, unsigned int iY, float fValue ); //!<Setzt an der Feldposition \p iX \p iY den Wert von \p fValue
  float getValueAt      ( unsigned int iPos ) const;                      //!<Gibt den Feldwert an Feldposition \p iPos zurueck
  float getValueAt      ( unsigned int iX, unsigned int iY ) const;       //!<Gibt den Feldwert an Feldposition \p iX \p iY zurueck
  int normalize         ( void );                                         //!<Skaliert alle Feldwerte im Bereich \c 0 bis \c 1
  float getMinValue       ( void ) const;                                   //!<gibt den minimalen Feldwert zurueck
  float getMaxValue       ( void ) const;                                   //!<gibt den maximale Feldwert zurueck
  int getMinPos         ( void ) const;                                   //!<gibt Feldposition fuer minimalen Feldwert zurueck
  int getMaxPos         ( void ) const;                                   //!<gibt Feldposition fuer maximalen Feldwert zurueck
  void scanMinThreshold ( float lowBound, int &minPos, float &minValue );
  void scanMaxThreshold ( float upBound, int &maxPos, float &maxValue );
  int invert            ( void );                                         //!<invertiert das Feld.
  int add               ( float fValue );                                 //!<addiert \p fValue zu allen Feldwerten
  int multiply          ( float fValue );                                 //!<multipliziert alle Feldwerte mit \p fValue;
  int add               ( const labField *oField );                       //!<addiert die Feldwerte von \p oField zu allen korrespondierenden Feldwerten
  int multiply          ( const labField *oField );                       //!<multipliziert alle Feldwerte von \p oField mit allen korrespondierenden Feldwerten
  int merge             ( const labField &oField );                       //!<mischt alle Feldwerte von \p oField mit allen korrespondierenden Feldwerten
  int getSize           ( void ) const;                                   //!<gibt die Groesse des Feldes zurueck
  int getSizeX          ( void ) const;                                   //!<gibt die Groesse des Feldes zurueck
  int getSizeY          ( void ) const;                                   //!<gibt die Groesse des Feldes zurueck
  int setSize           ( int iSize );                                    //!<ermoeglicht bedingtes Setzen der Feldgroesse
  bool isValid          ( void ) const;                                   //!<gibt an ob vpField valide und somit verwendbar ist
  int binarize          ( float fThreshold );                             //!<binarisiert das Feld
  int cutOff            ( float fThreshold, float valueToSet );
  int fillManhattan   ( unsigned int iX, unsigned int iY);        //!<Füllt ein Feld mit der ManhattanDistanz ab einem gegebenen Punkt
  int fillManhattanTextBoxCorrected( unsigned int iX, unsigned int iY, unsigned int iWidth, unsigned int iHeight);
  float* get_fData();

private:
  float   *_fData;
  unsigned int _iSize, _iSizeX, _iSizeY;
  float   _minValue, _maxValue;
  float _minThreshold;
  int   _minPos, _maxPos;
  bool  _usedStandardCtor;
  bool  _isFirstValue;
  void  _scanMinMax ( void );
};

#endif