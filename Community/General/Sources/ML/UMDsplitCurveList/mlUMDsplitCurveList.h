//----------------------------------------------------------------------------------
//! The ML module class UMDsplitCurveList.
/*!
// \file    mlUMDsplitCurveList.h
// \author  Tobias Mönch
// \date    2009-08-05
//
// Extracts single curves and several curves from a CureList object.
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDsplitCurveList_H
#define __mlUMDsplitCurveList_H


// Local includes
#include "UMDsplitCurveListSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#include "mlCurveData.h"
#include "mlCurveList.h"

ML_START_NAMESPACE


//! Extracts single curves and several curves from a CureList object.
class UMDSPLITCURVELIST_EXPORT UMDsplitCurveList : public BaseOp
{
public:

  //! Constructor.
  UMDsplitCurveList ();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

  //! Update internal module state after load or clone and enable notification handling again.
  virtual void activateAttachments ();

  //@}

private:

  // input and output fields
  BaseField   *_inputCurveListFld;
  BaseField		*_curveList1Fld;
  BaseField		*_curveList2Fld;
  BaseField		*_curveDataFld;

  // the internal data
  CurveData   _curveData;
  CurveList   _curveList1;
  CurveList   _curveList2;
  CurveList   *_inputCurveList;

  // GUI element(s)
  StringField *_availableCurvesFld;
  StringField *_availableCurvesStrFld;

  // for handling the curve names
  std::vector<std::string> _availableCurvesVec;
  std::string _availableCurves;
  
  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(UMDsplitCurveList)
};


ML_END_NAMESPACE

#endif // __mlUMDsplitCurveList_H

