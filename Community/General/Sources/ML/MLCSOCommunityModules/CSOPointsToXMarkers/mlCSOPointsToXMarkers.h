//----------------------------------------------------------------------------------
//! The ML module class CSOPointsToXMarkers.
/*!
// \file    mlCSOPointsToXMarkers.h
// \author  Coert Metz
// \date    2007-07-11
//
// Convert CSO points to an XMarkerList. Additionaly CSO normals can be computed
// and exported.
*/
//----------------------------------------------------------------------------------

/* =================================================================================
   Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR),
   Departments of Radiology and Medical Informatics, Erasmus MC. All
   rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of BIGR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =================================================================================*/

#ifndef __mlCSOPointsToXMarkers_H
#define __mlCSOPointsToXMarkers_H

// Local includes
#include "../MLCSOCommunityModulesSystem.h"

// ML includes
#include "mlOperatorIncludes.h"
#include "mlXMarkerList.h"
#include <CSOBase/CSOList.h>

ML_START_NAMESPACE

//! 
class MLCSOCOMMUNITYMODULES_EXPORT CSOPointsToXMarkers : public BaseOp
{
public:

  //! Constructor.
  CSOPointsToXMarkers (void);

  //! Destructor
  ~CSOPointsToXMarkers (void);

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field * /*field*/);

  //! Observer for CSO List
  static void CSOListNotifyObserverCB(void* userData, int notificationFlag);

private:

  // ----------------------------------------------------------
  //@{ \name Module field declarations
  // ----------------------------------------------------------

  //! Input CSO list field and output XMarkerList field
  BaseField *_inputCSOListFld, *_outputXMarkerListFld;
  
  //! Field for index of CSO in CSOList
  IntField *_listIndexFld;

  //! Option to convert pathpoints as well
  BoolField *_convertPathPoints;

  //! Option to compute and output CSO normals as well
  BoolField *_outputCSONormals;

  //@}

  // Output XMarkerList
  XMarkerList _outputXMarkerList;

  // Input CSOList
  CSOList * _inputCSOList;

  void convertCSOToXMarkerList();

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(CSOPointsToXMarkers)

};


ML_END_NAMESPACE

#endif // __mlCSOPointsToXMarkers_H


