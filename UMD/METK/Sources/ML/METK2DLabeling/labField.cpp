#include "labField.h"

//#define min(a,b) a>b?b:a
//#define max(a,b) a<b?b:a

#include <iostream>
#ifndef WIN32
  #include <float.h>
#endif


using namespace std;

//CTOR----------------------------------------------------------------------------------------------------
/*! Standardkonstruktur */
//--------------------------------------------------------------------------------------------------------
labField::labField( )
{
  _fData = NULL;
  _usedStandardCtor = true;
  /*_isFirstValue = true;*/
}


//CTOR----------------------------------------------------------------------------------------------------
/*! Konstruktor benoetigt die Groesse /p iSize des Feldes. Diese ist im weiteren Verlauf nicht flexibel. */
//--------------------------------------------------------------------------------------------------------
labField::labField(unsigned int iSizeX, unsigned int iSizeY)
{
  _fData             = NULL;
  _usedStandardCtor = false;
  try
  {
    if ( iSizeX < 1 || iSizeY<1 )
      throw "labField::initial field size can not be zero!";

    _fData = new float[iSizeX*iSizeY];

    if ( _fData == NULL )
      throw "labField::memory allocation failed!";
    else
    {
      _iSizeX = iSizeX; _iSizeY = iSizeY;
      _iSize = iSizeX*iSizeY;
      fillWith( 0.0 );
      _minValue = _maxValue = 0;
      _minPos   = _maxPos   = _iSize - 1;
    }
  }
  catch( char * str )
  {
    cout << "ERROR: " << str << endl;
  }
  //_usedStandardCtor = false;
  _isFirstValue = true;


}


int labField::set( labField* oField )
{
  if (_usedStandardCtor) return 0;
  if (!oField) return 0;
  try
  {
    _minValue = _maxValue = getValueAt(0) * oField->getValueAt(0);
    _minPos   = _maxPos   = 0;
    if ( _iSize != oField->getSize() ) throw "labField::field size not equal!";
    float* _tempData = oField->get_fData();
    for ( unsigned int i = 0; i < _iSize; i++ )
    {
      //setValueAt(i, oField->getValueAt(i) );
      //Write directly from pointer to pointer
      _fData[i] = _tempData[i];
    }
    _scanMinMax();
  }
  catch (char *str)
  {
    std::cout << "ERROR: " << str << std::endl;
    return 0;
  }
  return 1;
}

labField::~labField()
{
  if (_usedStandardCtor) return;
  try
  {
    delete[] _fData;
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
  }
}

//FILL_WITH-----------------------------------------------------------------------------------------------
/*!Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int labField::fillWith(float fValue)
{
  if (_usedStandardCtor) return 0;
  try
  {
    for ( unsigned int i = 0; i < _iSize; i++ )
      _fData[i] = fValue;
  }
  catch( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  _minValue = _maxValue = fValue;
  _minPos   = _maxPos   = _iSize - 1;

  return 1;
}


//SET_VALUE_AT--------------------------------------------------------------------------------------------
/*!Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int labField::setValueAt(unsigned int iPos, float fValue)
{
  if (_usedStandardCtor) return 0;
  try
  {
    if (iPos >= ( unsigned int ) _iSize) throw "labField:: (set) out of field bounds!";
    if (_isFirstValue)
    {
      _minValue = _maxValue = fValue;                       //ACHTUNG: dirty hack
      _minPos = _maxPos = iPos;                             //später: min/max global überprüfen
      _isFirstValue = false;
    }
    _fData[iPos] = fValue;

    if ( fValue > _maxValue )
    {
      _maxValue = fValue;
      _maxPos   = iPos;
    }

    if ( fValue < _minValue )
    {
      _minValue = fValue;
      _minPos   = iPos;
    }
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  return 1;
}


//SET_VALUE_AT--------------------------------------------------------------------------------------------
/*!Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int labField::setValueAt(unsigned int iX, unsigned int iY, float fValue)
{
  if (_usedStandardCtor) return 0;
  unsigned int iPos = iY*_iSizeX+iX;
  try
  {
    if (iPos >= ( unsigned int ) _iSize) throw "labField:: (set) out of field bounds!";
    if (_isFirstValue)
    {
      _minValue = _maxValue = fValue;                       //ACHTUNG: dirty hack
      _minPos = _maxPos = iPos;                             //später: min/max global überprüfen
      _isFirstValue = false;
    }
    _fData[iPos] = fValue;

    if ( fValue > _maxValue )
    {
      _maxValue = fValue;
      _maxPos   = iPos;
    }

    if ( fValue < _minValue )
    {
      _minValue = fValue;
      _minPos   = iPos;
    }
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  return 1;
}


//GET_VALUE_AT--------------------------------------------------------------------------------------------
/*! Schlaegt die Operation fehl, wird \c 0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
float labField::getValueAt ( unsigned int iPos ) const
{
  if (_usedStandardCtor) return 0;
  try
  {
    if ( iPos >= ( unsigned int ) _iSize )
      throw "labField:: (get) out of field bounds!";
    return _fData[iPos];
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
}


//GET_VALUE_AT--------------------------------------------------------------------------------------------
/*! Schlaegt die Operation fehl, wird \c 0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
float labField::getValueAt ( unsigned int iX, unsigned int iY ) const
{
  if (_usedStandardCtor) return 0;
  unsigned int iPos = iY*_iSizeX+iX;
  try
  {
    if ( iPos >= ( unsigned int ) _iSize )
      throw "labField:: (get) out of field bounds!";
    return _fData[iPos];
  }
  catch ( char *str )
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
}


//NORMALIZE-----------------------------------------------------------------------------------------------
/*! Ein Feld mit homogenen Werten wird auf \c 0 gesetzt. Schlaegt die Operation fehl, wird \c 0
zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int labField::normalize ( void )
{
  if (_usedStandardCtor) return 0;
  _scanMinMax();
  float fRange = _maxValue - _minValue;
  if (fRange == 0) fRange = 1;
  try
  {
    for (unsigned int i = 0; i < _iSize; i++)
      _fData[i] = (_fData[i] - _minValue) / fRange;
  }
  catch ( char *str )
  {
    cout<< "ERROR: " << str << endl;
    return 0;
  }
  _minValue = 0;
  _maxValue = 1.0;
  return 1;

}

//GET_MIN_VALUE-------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
float labField::getMinValue ( void ) const
{
  return _minValue;
}

//GET_MAX_VALUE-------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
float labField::getMaxValue ( void ) const
{
  return _maxValue;
}

//GET_MIN_POS---------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
int labField::getMinPos ( void ) const
{
  return _minPos;
}

//GET_MAX_POS---------------------------------------------------------------------------------------------
/*! */
//--------------------------------------------------------------------------------------------------------
int labField::getMaxPos ( void ) const
{
  return _maxPos;
}

//INVERT--------------------------------------------------------------------------------------------------
/*! Auch Felder die nicht auf den Bereich von \c 0 bis \c 1 skaliert sind koennen invertiert werden.
Schlaegt die Operation fehl, wird \c 0 zurueckgegeben, ansonsten \c 1. */
//--------------------------------------------------------------------------------------------------------
int labField::invert ( void )
{
  if (_usedStandardCtor) return 0;
  float fRange = _maxValue - _minValue;
  try
  {
    for ( unsigned int i = 0; i < _iSize; i++ )
      _fData[i] = fRange - _fData[i] + 2 * _minValue;
  }
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  int iTemp = _minPos;
  _minPos = _maxPos;
  _maxPos = iTemp;
  return 1;
}

//ADD-----------------------------------------------------------------------------------------------------
/*! Es gibt kein eigene Funktion fuer Subtraktion. Diese kann durch Addition negativer Werten durchgefuehrt
werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int labField::add ( float fValue )
{
  if (_usedStandardCtor) return 0;
  try
  {
    for (unsigned  int i = 0; i < _iSize; i++ )
      _fData[i] += fValue;
  }
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }
  _minValue += fValue;
  _maxValue += fValue;
  return 1;
}

//MULTIPLY------------------------------------------------------------------------------------------------
/*! Es gibt kein eigene Funktion fuer Division. Diese kann durch Multiplikation inverser Werten durchgefuehrt
werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int labField::multiply ( float fValue )
{
  if (_usedStandardCtor) return 0;
  try
  {
    for ( unsigned int i = 0; i < _iSize; i++ )
      _fData[i] *= fValue;
  }
  catch (char *str)
  {
    cout << "ERROR: " << str << endl;
    return 0;
  }

  _minValue *= fValue;
  _maxValue *= fValue;

  if ( fValue < 0 )
  {
    int iTemp = _minPos;
    _minPos = _maxPos;
    _maxPos = iTemp;

    float fTemp = _minValue;
    _minValue = _maxValue;
    _maxValue = fTemp;
  }

  if ( fValue == 0 )
  {
    _minValue = _maxValue = 0;
    _minPos   = _maxPos   = _iSize - 1;
  }

  return 1;
}

//ADD-----------------------------------------------------------------------------------------------------
/*! Es koennen nur Felder gleicher Groesse addiert werden. Schlaegt die Operation fehl, wird \c 0.0 zurueckgegeben. */
//--------------------------------------------------------------------------------------------------------
int labField::add ( const labField *oField )
{
  if (_usedStandardCtor) return 0;
  if (!oField) return 0;
  try
  {
    _minValue = _maxValue = getValueAt(0) * oField->getValueAt(0);
    _minPos   = _maxPos   = 0;
    if ( _iSize != oField->getSize() ) throw "labField::field size not equal!";
    for ( unsigned int i = 0; i < _iSize; i++ )
      setValueAt(i, _fData[i] + oField->getValueAt(i) );
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
int labField::multiply ( const labField *oField )
{
  if (_usedStandardCtor) return 0;
  try
  {
    _minValue = _maxValue = getValueAt(0) * oField->getValueAt(0);
    _minPos   = _maxPos   = 0;
    if ( _iSize != oField->getSize() ) throw "labField::field size not equal!";
    for ( unsigned int i = 0; i < _iSize; i++ )
      setValueAt(i, _fData[i] * oField->getValueAt(i) );
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
int labField::merge ( const labField &oField )
{
  if (_usedStandardCtor) return 0;
  try
  {
    if ( _iSize != oField.getSize() ) throw "labField::field size not equal!";
    for (unsigned  int i = 0; i < _iSize; i++ )
      setValueAt(i, max ( _fData[i], oField.getValueAt(i) ) );
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
int labField::getSize ( void ) const
{
  return _iSize;
}


int labField::getSizeX ( void ) const
{
  return _iSizeX;
}


int labField::getSizeY ( void ) const
{
  return _iSizeY;
}



//SET_SIZE------------------------------------------------------------------------------------------------
/*! Kann nur einmal nach Verwendung des Standard-Ctor ( z.b. bei dem Erzeugen eines labField - Arrays mit
\c new )verwendet werden um die Feldgroesse zu setzen.*/
//--------------------------------------------------------------------------------------------------------
int labField::setSize ( int iSize )
{
  if (!_usedStandardCtor) return 0;
  _usedStandardCtor = false;
  try
  {
    if ( iSize < 1 )
      throw "labField::initial field size can not be zero!";

    _fData = new float[iSize];

    if ( _fData == NULL )
      throw "labField::memory allocation failed!";
    else
    {
      _iSize = iSize;
      fillWith( 0.0 );
      _minValue = _maxValue = 0;
      _minPos   = _maxPos   = _iSize - 1;
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
bool labField::isValid ( void ) const
{
  return !_usedStandardCtor;
}


void labField::_scanMinMax ( void )
{
  _minValue = _maxValue = getValueAt(0);
  _minPos   = _maxPos   = 0;
  for ( unsigned int i = 0; i < _iSize; i++ )
  {
    if (_fData[i] < _minValue)
    {
      //_minValue = getValueAt(i);
      _minValue = _fData[i];
      _minPos = i;
    }
    if (_fData[i] > _maxValue)
    {
      //_maxValue = getValueAt(i);
      _maxValue = _fData[i];
      _maxPos = i;
    }
  }
}

int labField::binarize( float fThreshold )
{
  if (_usedStandardCtor) return 0;
  for ( unsigned int i = 0; i < _iSize; i++ )
  {
    //if ( getValueAt(i) >= fThreshold ) setValueAt(i,1.0); else setValueAt(i,0.0);
    if ( _fData[i] >= fThreshold ) setValueAt(i,1.0); else setValueAt(i,0.0);
  }
  return 1;
}

int labField::cutOff( float fThreshold, float valueToSet )
{
  if (_usedStandardCtor) return 0;
  for ( unsigned int i = 0; i < _iSize; i++ )
  {
    if ( _fData[i] <= fThreshold ) setValueAt(i,valueToSet);
  }
  return 1;
}



int labField::fillManhattan( unsigned int iX, unsigned int iY)
{
  if (_usedStandardCtor) return 0;
  for (unsigned int i=0; i<_iSizeX; i++)
  {
    for ( unsigned int j=0; j<_iSizeY; j++)
    {
      setValueAt(i,j, abs(int(iX-i))+abs(int(iY-j)));
    }
  }
  return 1;
}


int labField::fillManhattanTextBoxCorrected( unsigned int iX, unsigned int iY, unsigned int iWidth, unsigned int iHeight)
{
  if (_usedStandardCtor) return 0;
  int tempDist;
  for (unsigned int i=0; i<_iSizeX; i++)
  {
    for ( unsigned int j=0; j<_iSizeY; j++)
    {
      tempDist = abs(int(iX-i))+abs(int(iY-j));
      if (i<iX)
      {
        tempDist = tempDist-min(iWidth,iX-i);
      }
      if (j<iY)
      {
        tempDist = tempDist-min(iHeight,iY-j);
      }
      setValueAt(i,j, tempDist);
    }
  }
  return 1;
}



void labField::scanMinThreshold ( float lowBound, int &minPos, float &minValue )
{
  minValue = FLT_MAX;
  minPos   = -1;
  for ( unsigned int i = 0; i < _iSize; i++ )
  {
    if (_fData[i] < minValue && _fData[i] >= lowBound)
    {
      minValue = _fData[i];
      minPos = i;
    }
  }
}

void labField::scanMaxThreshold ( float upBound, int &maxPos, float &maxValue )
{
  maxValue = FLT_MIN;
  maxPos   = -1;
  for ( unsigned int i = 0; i < _iSize; i++ )
  {
    if (_fData[i] > maxValue && _fData[i] <= upBound)
    {
      maxValue = _fData[i];
      maxPos = i;
    }
  }
}


float* labField::get_fData()
{
  return _fData;
}
