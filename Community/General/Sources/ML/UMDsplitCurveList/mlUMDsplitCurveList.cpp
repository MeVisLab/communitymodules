//----------------------------------------------------------------------------------
//! The ML module class UMDsplitCurveList.
/*!
// \file    mlUMDsplitCurveList.cpp
// \author  Tobias Mönch
// \date    2009-08-05
//
// Extracts single curves and several curves from a CureList object.
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlUMDsplitCurveList.h"

ML_START_NAMESPACE

using namespace std;

std::string
asString( double d, int precision = 6 )
{
std::ostringstream s ;
s.precision( precision ) ;
s << d ;
return s.str() ;
}

//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(UMDsplitCurveList, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
UMDsplitCurveList::UMDsplitCurveList ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("UMDsplitCurveList::UMDsplitCurveList ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();
  
  FieldContainer *fc = getFieldContainer();
  
  // add input field
  _inputCurveListFld    = fc->addBase("inCurveList");
  
  // output fields
  (_curveList1Fld       = fc->addBase("outCurveList1"))->setBaseValue(&_curveList1);
  (_curveDataFld        = fc->addBase("outCurveData"))->setBaseValue(&_curveData);
  (_curveList2Fld       = fc->addBase("outCurveList2"))->setBaseValue(&_curveList2);

  // GUI element(s)
  _availableCurvesStrFld  = fc->addString("AvailableCurvesStr");
  _availableCurvesFld  = fc->addString("AvailableCurves");
  _availableCurvesFld->setStringValue("");
  
  
  this->activateAttachments();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void UMDsplitCurveList::handleNotification (Field *field)
{
  ML_TRACE_IN("UMDsplitCurveList::handleNotification ()"); 
  // Handle changes of module parameters and connectors here.

  // some status variable
  bool inputOK = false;

  // check if input is available
  if (_inputCurveListFld->getBaseValue() != NULL)
  {
    // initially clear the list of curves
    _availableCurvesStrFld->setStringValue("");
    _availableCurves.clear();
    _availableCurvesVec.clear();

    // if the input is of type "CurveList" ....
    if (strncasecmp(_inputCurveListFld->getBaseValue()->getTypeIdName(),"CurveList",9)==0)
    {
      // set status
      inputOK = true;
      
      // get the data
      _inputCurveList =  (ml::CurveList *)_inputCurveListFld->getBaseValue();
  
      // run over the curves
      for (int i=0; i<_inputCurveList->getNumCurves(); i++)
      {
        // add a "," as separator
        if (i!=0) _availableCurves.append(",");

        // append the strings to both: the output string and a similar string-vector
        _availableCurves.append(asString(i+1,0) + ": " + _inputCurveList->getCurveList()[i]->getTitle().c_str());
        
        _availableCurvesVec.push_back(asString(i+1,0) + ": " + _inputCurveList->getCurveList()[i]->getTitle().c_str());
      }
      // assign the string to the field
      _availableCurvesStrFld->setStringValue(_availableCurves);
    }
  }
  // clear the list
  else _availableCurvesStrFld->setStringValue("");

  // check again if input is available 
  if (((field == _availableCurvesFld) ||(field == _inputCurveListFld)) && (_inputCurveListFld->getBaseValue() != NULL) && (_inputCurveList->getNumCurves() > 0) && inputOK)
  {
    // clear the output curveLists and the curveData
    _curveList1.getCurveList().clear();
    _curveList2.getCurveList().clear();
    _curveData.clear();

    // status and index variables
    unsigned int selectionIdx=0;
    bool found = false;

    // run over the available curves ...
    for (unsigned int i=0; i<_availableCurvesVec.size(); i++)
    {
      // check which curve has been selected
      if (_availableCurvesVec[i].compare(_availableCurvesFld->getStringValue()) == 0)
      {
        // remember its index and set status to true
        selectionIdx = i;
        found = true;
      }
      // as long as the selected curve was not found in the list, push all curves into curveList1
      if (!found) _curveList1.getCurveList().push_back(_inputCurveList->getCurveList()[i]);
      
      // after the selected curve has been found, push all curves into curveList2
      else if (found && (i>selectionIdx)) _curveList2.getCurveList().push_back(_inputCurveList->getCurveList()[i]);
    }
    // assign the selected curve as single output
    _curveData = *_inputCurveList->getCurveList()[selectionIdx];

    // do the output to the fields
    _curveList1Fld->setBaseValue(&_curveList1);
    _curveList2Fld->setBaseValue(&_curveList2);
    _curveDataFld->setBaseValue(&_curveData);
  }
}

//----------------------------------------------------------------------------------
//! Update internal module state after load or clone and enable notification handling again.
//----------------------------------------------------------------------------------
void UMDsplitCurveList::activateAttachments ()
{
  ML_TRACE_IN("UMDsplitCurveList::activateAttachments ()");

  // Update members to new field state here.
  // Call super class functionality to enable notification handling again.
  BaseOp::activateAttachments ();
}



ML_END_NAMESPACE

