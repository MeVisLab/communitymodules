#include "vpField.h"
#include "kBasics.h"
#ifndef min
#define min(a,b) a>b?b:a
#endif
#ifndef max
#define max(a,b) a<b?b:a
#endif

#include <iostream>
using namespace std;

//CTOR----------------------------------------------------------------------------------------------------
/*! Standardkonstruktur */
//--------------------------------------------------------------------------------------------------------
CvpField::CvpField( )
{
  m_vfData = NULL;
  m_flUsedStandardCtor = true;
  m_flIsFirstValue = true;
}


//CTOR----------------------------------------------------------------------------------------------------
/*! Konstruktor benoetigt die Groesse /p iSize des Feldes. Diese ist im weiteren Verlauf nicht flexibel. */
//--------------------------------------------------------------------------------------------------------
CvpField::CvpField(unsigned int iSize)
{
  m_vfData             = NULL;
  m_flUsedStandardCtor = false;
  try
  {
    if ( iSize < 1 ) 
      throw "vpField::initial field size can not be zero!";
    
    m_vfData = new float[iSize];
    
    if ( m_vfData == NULL )
      throw "vpField::memory allocation failed!";
    else
    {
      m_iSize = iSize;
      fillWith( 0.0 );
      m_fMinValue = m_fMaxValue = 0;
      m_iMinPos   = m_iMaxPos   = m_iSize - 1;
    }
  }
  catch( char * str )
  {
    cout << "ERROR: " << str << endl;
  }
  //m_flUsedStandardCtor = false;
  m_flIsFirstValue = true;


}

//DTOR----------------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
CvpField::~CvpField()
{
  if (m_flUsedStandardCtor) return;
  try
  {
    delete[] m_vfData;
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
  }
}

//FILL_WITH-----------------------------------------------------------------------------------------------
/*!Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int CvpField::fillWith(float fValue)
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    for ( int i = 0; i < m_iSize; i++ ) 
      m_vfData[i] = fValue;
  }
  catch( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  } 
  m_fMinValue = m_fMaxValue = fValue;
  m_iMinPos   = m_iMaxPos   = m_iSize - 1;

  return 1;
}


//SET_VALUE_AT--------------------------------------------------------------------------------------------
/*!Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int CvpField::setValueAt(unsigned int iPos, float fValue)
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    if (iPos >= ( unsigned int ) m_iSize) throw "vpField:: (set) out of field bounds!";
    if (m_flIsFirstValue)
    {
      m_fMinValue = m_fMaxValue = fValue;                       //ACHTUNG: dirty hack
      m_iMinPos = m_iMaxPos = iPos;                             //später: min/max global überprüfen
      m_flIsFirstValue = false;
    }
    m_vfData[iPos] = fValue;
    
    if ( fValue > m_fMaxValue ) 
    {
      m_fMaxValue = fValue;
      m_iMaxPos   = iPos;
    }

    if ( fValue < m_fMinValue ) 
    {
      m_fMinValue = fValue;
      m_iMinPos   = iPos;
    }
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  _scanMinMax();
  return 1;
}

//SET_VALUE_AT--------------------------------------------------------------------------------------------
/*! Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
float CvpField::getValueAt ( unsigned int iPos ) const
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    if ( iPos >= ( unsigned int ) m_iSize ) 
      throw "vpField:: (get) out of field bounds!";
    return m_vfData[iPos];
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
}
//!<Gibt den Feldwert an Feldposition \p iPos zurueck


//NORMALIZE-----------------------------------------------------------------------------------------------
/*! Ein Feld mit homogenen Werten wird auf \c 0 gesetzt. Schlaegt die Operation fehl, wird \c 0 
    zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------

int CvpField::normalize ( void )
{
  if (m_flUsedStandardCtor) return 0;
  _scanMinMax();
  float fRange = m_fMaxValue - m_fMinValue;
  if (fRange == 0) fRange = 1;
  try
  {
    for (int i = 0; i < m_iSize; i++)
      m_vfData[i] = (m_vfData[i] - m_fMinValue) / fRange;
  }
  catch ( char *str )
  {
    cout<< "ERROR: " << str << endl;
    return 0;
  }
  m_fMinValue = 0;
  m_fMaxValue = 1.0;
  return 1;
  
}

//GET_MIN_VALUE-------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
float CvpField::getMinValue ( void ) const
{
  return m_fMinValue;
}

//GET_MAX_VALUE-------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
float CvpField::getMaxValue ( void ) const
{
  return m_fMaxValue;
}

//GET_MIN_POS---------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
int CvpField::getMinPos ( void ) const
{
  return m_iMinPos;
}

//GET_MAX_POS---------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
int CvpField::getMaxPos ( void ) const
{
  return m_iMaxPos;
}

//INVERT--------------------------------------------------------------------------------------------------
/*! Auch Felder die nicht auf den Bereich von \c 0 bis \c 1 skaliert sind koennen invertiert werden. 
    Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int CvpField::invert ( void )
{
	if (m_flUsedStandardCtor) return 0;
  float fRange = m_fMaxValue - m_fMinValue;
	try
  {
    for ( int i = 0; i < m_iSize; i++ )
      m_vfData[i] = fRange - m_vfData[i] + 2 * m_fMinValue;
  } 
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  int iTemp = m_iMinPos;
  m_iMinPos = m_iMaxPos;
  m_iMaxPos = iTemp;
  return 1;
}

//ADD-----------------------------------------------------------------------------------------------------
/*! Es gibt kein eigene Funktion fuer Subtraktion. Diese kann durch Addition negativer Werten durchgefuehrt 
    werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int CvpField::add ( float fValue )
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    for ( int i = 0; i < m_iSize; i++ )
      m_vfData[i] += fValue;
  } 
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  m_fMinValue += fValue;
  m_fMaxValue += fValue;
  return 1;
}

//MULTIPLY------------------------------------------------------------------------------------------------
/*! Es gibt kein eigene Funktion fuer Division. Diese kann durch Multiplikation inverser Werten durchgefuehrt 
    werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int CvpField::multiply ( float fValue )
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    for ( int i = 0; i < m_iSize; i++ )
      m_vfData[i] *= fValue;
  } 
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  
  m_fMinValue *= fValue;
  m_fMaxValue *= fValue;
  
  if ( fValue < 0 )
  {
    int iTemp = m_iMinPos;
    m_iMinPos = m_iMaxPos;
    m_iMaxPos = iTemp;

    float fTemp = m_fMinValue;
    m_fMinValue = m_fMaxValue;
    m_fMaxValue = fTemp;    
  }

  if ( fValue == 0 )
  {
      m_fMinValue = m_fMaxValue = 0;
      m_iMinPos   = m_iMaxPos   = m_iSize - 1;
  }

  return 1;
}

//ADD-----------------------------------------------------------------------------------------------------
/*! Es koennen nur Felder gleicher Groesse addiert werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int CvpField::add ( const CvpField &oField )
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    m_fMinValue = m_fMaxValue = getValueAt(0) * oField.getValueAt(0);
    m_iMinPos   = m_iMaxPos   = 0;
    if ( m_iSize != oField.getSize() ) throw "vpField::field size not equal!";
    for ( int i = 0; i < m_iSize; i++ )
      setValueAt(i, getValueAt(i) + oField.getValueAt(i) );
  } 
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  return 1;
}

//ADD-----------------------------------------------------------------------------------------------------
/*! Es koennen nur Felder gleicher Groesse multipliziert werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int CvpField::multiply ( const CvpField &oField )
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    m_fMinValue = m_fMaxValue = getValueAt(0) * oField.getValueAt(0);
    m_iMinPos   = m_iMaxPos   = 0;
    if ( m_iSize != oField.getSize() ) throw "vpField::field size not equal!";
    for ( int i = 0; i < m_iSize; i++ )
      setValueAt(i, getValueAt(i) * oField.getValueAt(i) );
  } 
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  return 1;
}

//MERGE---------------------------------------------------------------------------------------------------
/*! Es wird der jeweils groesste Wert der beiden Felder als neuer Feldwert verwendet. Es koennen nur Felder 
    gleicher Groesse gemischt werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int CvpField::merge ( const CvpField &oField )
{
  if (m_flUsedStandardCtor) return 0;
  try
  {
    if ( m_iSize != oField.getSize() ) throw "vpField::field size not equal!";
    for ( int i = 0; i < m_iSize; i++ )
      setValueAt(i, max ( getValueAt(i), oField.getValueAt(i) ) );
  } 
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  return 1;
}

//GET_SIZE------------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
int CvpField::getSize ( void ) const
{
  return m_iSize;
}

//SET_SIZE------------------------------------------------------------------------------------------------
/*! Kann nur einmal nach Verwendung des Standard-Ctor ( z.b. bei dem Erzeugen eines CvpField - Arrays mit
    \c new )verwendet werden um die Feldgroesse zu setzen.*/
//--------------------------------------------------------------------------------------------------------
int CvpField::setSize ( int iSize )
{
  if (!m_flUsedStandardCtor) return 0;
  m_flUsedStandardCtor = false;
  try
  {
    if ( iSize < 1 ) 
      throw "vpField::initial field size can not be zero!";
    
    m_vfData = new float[iSize];
    
    if ( m_vfData == NULL )
      throw "vpField::memory allocation failed!";
    else
    {
      m_iSize = iSize;
      fillWith( 0.0 );
      m_fMinValue = m_fMaxValue = 0;
      m_iMinPos   = m_iMaxPos   = m_iSize - 1;
    }
  }
  catch( char * str )
  {
    cout << "ERROR: " << str << endl;
  }
  return 1;

}

//IS_VALID------------------------------------------------------------------------------------------------
/*! Dies ist der Fall wenn der \c Ctor(iSize) verwendet wurde oder \c setSize nach Verwendung des Standard
    \c Ctors genutzt wurde. */
//--------------------------------------------------------------------------------------------------------
bool CvpField::isValid ( void ) const
{
  return !m_flUsedStandardCtor;
}

void CvpField::_scanMinMax ( void )
{
  m_fMinValue = m_fMaxValue = getValueAt(0);
  m_iMinPos   = m_iMaxPos   = 0;
  for ( int i = 0; i < m_iSize; i++ )
  {
    if (getValueAt(i) < m_fMinValue)
    {
      m_fMinValue = getValueAt(i);
      m_iMinPos = i;
    }
    if (getValueAt(i) > m_fMaxValue)
    {
      m_fMaxValue = getValueAt(i);
      m_iMaxPos = i;
    }
  }
}
string CvpField::toString(){
	string ret="";
	if (m_flUsedStandardCtor) return 0;
	for ( int i = 0; i < m_iSize; i++ ){
		ret+=kBasics::FloatToString(getValueAt(i))+";";
	}
	return ret;
}
int CvpField::binarize( float fThreshold )
{
  if (m_flUsedStandardCtor) return 0;
  for ( int i = 0; i < m_iSize; i++ )
  {
    if ( getValueAt(i) >= fThreshold ) setValueAt(i,1.0); else setValueAt(i,0.0);
  }
  return 1; 
}
