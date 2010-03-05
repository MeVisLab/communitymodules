//----------------------------------------------------------------------------------
//! The ML module class LinearInterpolateXMarkerList.
/*!
// \file    mlLinearInterpolateXMarkerList.h
// \author  Coert Metz
// \date    2009-09-09
//
// Linearly interpolate an XMarkerList
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

#ifndef __mlLinearInterpolateXMarkerList_H
#define __mlLinearInterpolateXMarkerList_H


// Local includes
#ifndef __MLXMarkerListCommunityModulesSystem_H
#include "MLXMarkerListCommunityModulesSystem.h"
#endif

// ML includes
#include "mlOperatorIncludes.h"

#include "mlXMarkerList.h"

ML_START_NAMESPACE


//! Linearly interpolate an XMarkerList
class MLXMARKERLISTCOMMUNITYMODULES_EXPORT LinearInterpolateXMarkerList : public BaseOp
{
public:

  //! Constructor.
  LinearInterpolateXMarkerList ();

  //! Handle field changes of the field field.
  virtual void handleNotification (Field *field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! Input and output XMarkerList fields
  BaseField *_inputXMarkerListFld;
  BaseField *_outputXMarkerListFld;
  XMarkerList _outputXMarkerList;

  //! Sample distance output XMarkerList
  FloatField *_sampleDistanceFld;

  //@}

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(LinearInterpolateXMarkerList)
};


ML_END_NAMESPACE

#endif // __mlLinearInterpolateXMarkerList_H

