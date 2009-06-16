//----------------------------------------------------------------------------------
//! The ML module class UMDBoundingBoxCalculator2.
/*!
// \file    mlUMDBoundingBoxCalculator2.h
// \author  Konrad
// \date    2006-05-16
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __mlUMDBoundingBoxCalculator2_H
#define __mlUMDBoundingBoxCalculator2_H


// Local includes
#ifndef __UMDBoundingBoxCalculator2System_H
#include "UMDBoundingBoxCalculator2System.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

// Wrap inventor includes into XVEnterScope and XVLeaveScope to avoid
// collisions between OpenInventor and Windows headers.
#include "XVEnterScope.h"
#include <Inventor/engines/SoComputeBoundingBox.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include "XVLeaveScope.h"

#include "kBasics.h"

ML_START_NAMESPACE


//! 
class UMDBOUNDINGBOXCALCULATOR2_EXPORT UMDBoundingBoxCalculator2 : public BaseOp
{
public:

  //! Constructor.
  UMDBoundingBoxCalculator2 (void);
  ~UMDBoundingBoxCalculator2 (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);


  //! 
  StringField* boundingBoxString;

  //! 
  Vec3fField* boundingBoxMin;  

  //! 
  Vec3fField* boundingBoxMax;

  //! 
  Vec3fField* objectCenter;

  //! 
  Vec3fField* boxCenter;

  //!
  SoNodeField* inputObject;



private:

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  SoFieldSensor* _minSensor;

  //!
  SoComputeBoundingBox *_moduleComputeBoundingBox;
  
  //!
  SoCalculator *_moduleCalculator;


  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(UMDBoundingBoxCalculator2)


  //! Called when field \c min changed
  void minChanged(SoFieldSensor* sensor);
  //! forward CB when field \c min changed
  static void minChangedCB(void *data, SoSensor* sens);

};


ML_END_NAMESPACE

#endif // __mlUMDBoundingBoxCalculator2_H


