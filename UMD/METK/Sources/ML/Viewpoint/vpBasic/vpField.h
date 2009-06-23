#ifndef VP_FIELD_H
#define VP_FIELD_H
#include <string>
/*! \class CvpField
    \brief Eine Klasse fuer Felder und darauf anwendbaren Standardoperationen.
    \author Mathias Neugebauer
    \version 0.1
    \date 04-2007

    Die bei den Felder handelt es sich zur Zeit um \c float - Felder, da ich auf Kriegsfuss mit \c templates stehe...
    aber eines Tage vielleicht, wer weiss. 
*/

#ifdef WIN32
  #ifndef VPBASIC_EXPORTS
    #define __VPBASIC_IMPORTEXPORT __declspec(dllimport)
  #else
    #define __VPBASIC_IMPORTEXPORT __declspec(dllexport) 
  #endif
#else
  #define __VPBASIC_IMPORTEXPORT
#endif

class __VPBASIC_IMPORTEXPORT CvpField{
  public:
    CvpField              ( );                                              //!<Konstruktor
    CvpField              ( unsigned int iSize );                           //!<Konstruktor
    ~CvpField             ( void );                                         //!<Destruktur
    int fillWith          ( float fValue );                                 //!<Fuellt das Feld mit dem Wert von \p fValue
    int setValueAt        ( unsigned int iPos, float fValue );              //!<Setzt an der Feldposition \p iPos den Wert von \p fValue
    float getValueAt      ( unsigned int iPos ) const;                      //!<Gibt den Feldwert an Feldposition \p iPos zurueck
    int normalize         ( void );                                         //!<Skaliert alle Feldwerte im Bereich \c 0 bis \c 1
    float getMinValue     ( void ) const;                                   //!<gibt den minimalen Feldwert zurueck
    float getMaxValue     ( void ) const;                                   //!<gibt den maximale Feldwert zurueck
    int getMinPos         ( void ) const;                                   //!<gibt Feldposition fuer minimalen Feldwert zurueck
    int getMaxPos         ( void ) const;                                   //!<gibt Feldposition fuer maximalen Feldwert zurueck
    int invert            ( void );                                         //!<invertiert das Feld.
    int add               ( float fValue );                                 //!<addiert \p fValue zu allen Feldwerten
    int multiply          ( float fValue );                                 //!<multipliziert alle Feldwerte mit \p fValue;
    int add               ( const CvpField &oField );                       //!<addiert die Feldwerte von \p oField zu allen korrespondierenden Feldwerten
    int multiply          ( const CvpField &oField );                       //!<multipliziert alle Feldwerte von \p oField mit allen korrespondierenden Feldwerten
    int merge             ( const CvpField &oField );                       //!<mischt alle Feldwerte von \p oField mit allen korrespondierenden Feldwerten
    int getSize           ( void ) const;                                   //!<gibt die Groesse des Feldes zurueck
    int setSize           ( int iSize );                                    //!<ermoeglicht bedingtes Setzen der Feldgroesse 
    bool isValid          ( void ) const;                                   //!<gibt an ob vpField valide und somit verwendbar ist
    int binarize          ( float fThreshold );                             //!<binarisiert das Feld
	std::string toString  ();												//!<gibt die Werte als String zurueck

  private:
    float     *m_vfData;
    int       m_iSize;
    float     m_fMinValue, m_fMaxValue;
    int       m_iMinPos, m_iMaxPos;
    bool      m_flUsedStandardCtor;
    bool      m_flIsFirstValue;
    void      _scanMinMax ( void );
};




#endif