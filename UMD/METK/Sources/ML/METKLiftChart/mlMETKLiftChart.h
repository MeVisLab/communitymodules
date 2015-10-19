//----------------------------------------------------------------------------------
//! ML ObjMgrClient METKLiftChart module METKLiftChart.
/*!
// \file    mlMETKLiftChart.h
// \author  Christian Tietjen
*/
//----------------------------------------------------------------------------------


#ifndef __MLMETKLIFTCHART_H
#define __MLMETKLIFTCHART_H


// Local includes
#include "METKLiftChartSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <ObjMgr/mlObjMgrClient.h>

class SoView2DLiftChart;
class SoSeparator;
class LiftChartData;

ML_START_NAMESPACE


//! ML ObjMgrClient METKLiftChart module METKLiftChart.
class METKLIFTCHART_EXPORT METKLiftChart : public ObjMgrClient
{
public:

  //! Constructor/Destructor.
  METKLiftChart();
   ~METKLiftChart();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  virtual void handleObjMgrNotification();

  virtual void activateAttachments();


private:

  typedef ObjMgrClient inherited;

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //!

   BoolField* _active;
   EnumField* _aggregation;
   FloatField* _minImportance;
   FloatField* _maxExtent;
   BoolField* _divideBySide;
   BoolField* _hideSideUnkown;
   IntField* _verticalOffset;


   SoNodeField* _outInventor;

  //@}

   SoView2DLiftChart* _view2DLiftChart;

   void buildLiftChart();
   LiftChartData* _liftChartData;

   // next available structure which provides all necessary data
   const omObject* getNextStructure(const omObject* current = 0);

  //! Implements interface for the runtime type system of the ML.
  ML_CLASS_HEADER(METKLiftChart)

};


ML_END_NAMESPACE

#endif // __MLMETKLIFTCHART_H


