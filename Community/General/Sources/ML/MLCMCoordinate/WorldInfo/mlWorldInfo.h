//-----------------------------------------------------------------------
//
// Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
// Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of BIGR nor the names of its contributors 
//       may be used to endorse or promote products derived from this software 
//       without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-----------------------------------------------------------------------
//! Defines and implements an operator showing
//! information about the input module.
/*!
// \file    mlWorldInfo.h
// \author  Wolf Spindler
// \date    02/2002
*/
//======================================================
#ifndef __mlWorldInfo_H
#define __mlWorldInfo_H

// Local includes
#include "../MLCMCoordinateSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#ifndef __mlRotation_H
#include "mlRotation.h"
#endif

#ifndef __mlRotationField_H
#include "mlRotationField.h"
#endif

// Show imagePropertyExtensions if defined.
//#define _ML_SHOW_EXTENDED_IMAGE_PROPERTIES

ML_START_NAMESPACE

//========================================================================
//! Implements the class showing information about input operator.
//======================================================================
class MLCMCOORDINATE_EXPORT WorldInfo: public BaseOp{

public:
  //! Constructor
  WorldInfo();

  //@{ Access methods to parameter fields. For documentation see corresponding fields.
  inline IntField    *getSizeXFld()       const { return _sizeXFld; }
  inline IntField    *getSizeYFld()       const { return _sizeYFld; }
  inline IntField    *getSizeZFld()       const { return _sizeZFld; }
  inline IntField    *getSizeCFld()       const { return _sizeCFld; }
  inline IntField    *getSizeTFld()       const { return _sizeTFld; }
  inline IntField    *getSizeUFld()       const { return _sizeUFld; }
  inline IntField    *getPsizeXFld()      const { return _psizeXFld; }
  inline IntField    *getPsizeYFld()      const { return _psizeYFld; }
  inline IntField    *getPsizeZFld()      const { return _psizeZFld; }
  inline IntField    *getPsizeCFld()      const { return _psizeCFld; }
  inline IntField    *getPsizeTFld()      const { return _psizeTFld; }
  inline IntField    *getPsizeUFld()      const { return _psizeUFld; }
  inline DoubleField *getMinFld()         const { return _minFld; }
  inline DoubleField *getMaxFld()         const { return _maxFld; }
  inline DoubleField *getVXFld()          const { return _vXFld; }
  inline DoubleField *getVYFld()          const { return _vYFld; }
  inline DoubleField *getVZFld()          const { return _vZFld; }
  inline DoubleField *getVoxelVolFld()    const { return _VoxelVolFld; }
  inline DoubleField *getTotalVolFld()    const { return _TotalVolFld; }
  inline DoubleField *getMegaVoxelFld()   const { return _MegaVoxelFld; }
  inline StringField *getDTypeFld()       const { return _dTypeFld; }
  inline StringField *getTypeInfoFld()    const { return _typeInfoFld; }
#ifdef _ML_SHOW_EXTENDED_IMAGE_PROPERTIES
  inline StringField *getImgPropExtFld()  const { return _imgPropExtFld; }
#endif
  inline DoubleField *getX0Fld()          const { return _x0Fld; }
  inline DoubleField *getY0Fld()          const { return _y0Fld; }
  inline DoubleField *getZ0Fld()          const { return _z0Fld; }
  inline DoubleField *getA0Fld()          const { return _a0Fld; }
  inline DoubleField *getX1Fld()          const { return _x1Fld; }
  inline DoubleField *getY1Fld()          const { return _y1Fld; }
  inline DoubleField *getZ1Fld()          const { return _z1Fld; }
  inline DoubleField *getA1Fld()          const { return _a1Fld; }
  inline DoubleField *getX2Fld()          const { return _x2Fld; }
  inline DoubleField *getY2Fld()          const { return _y2Fld; }
  inline DoubleField *getZ2Fld()          const { return _z2Fld; }
  inline DoubleField *getA2Fld()          const { return _a2Fld; }
  inline DoubleField *getX3Fld()          const { return _x3Fld; }
  inline DoubleField *getY3Fld()          const { return _y3Fld; }
  inline DoubleField *getZ3Fld()          const { return _z3Fld; }
  inline DoubleField *getA3Fld()          const { return _a3Fld; }
  inline MatrixField *getMatFld()         const { return _matFld; }
  inline StringField *getCDimInfoFld()    const { return _cDimInfoFld; }
  inline StringField *getTDimInfoFld()    const { return _tDimInfoFld; }
  inline StringField *getUDimInfoFld()    const { return _uDimInfoFld; }
  inline StringField *getTDimTableFld()   const { return _tDimTableFld; }
  inline NotifyField *getUpdateFld()      const { return _updateFld; }
  inline StringField *getMemImgFld()      const { return _memImgFld; }
  inline StringField *getMemImg2Fld()     const { return _memImg2Fld; }
  inline IntField    *getBypassIndexFld() const { return _bypassIndexFld; }
  inline IntField    *getInPlaceIndexFld()const { return _inPlaceIndexFld; }
  inline StringField *getOpNameFld()      const { return _opNameFld; }

  inline BoolField   *getInConnFld()      const { return _inConnFld; }
  inline BoolField   *getInputValidFld()  const { return _inputValidFld; }
  //@}


private:
  //----------------------------------------------------------------------
  // Methods.
  //----------------------------------------------------------------------
  //! Called when a field in the field container changes. Used to detect input field changes.
  void handleNotification(Field* field);

  //! Updates all fields to the new values.
  void _updateInfos();

  //@{\name The extents of the input image; defaults are 0.
  IntField    *_sizeXFld,  *_sizeYFld,  *_sizeZFld,
              *_sizeCFld,  *_sizeTFld,  *_sizeUFld;
  VectorField *_sizeFld;
  //@}

  //@{\name Page size; defaults are 0.
  IntField    *_psizeXFld, *_psizeYFld, *_psizeZFld,
              *_psizeCFld, *_psizeTFld, *_psizeUFld;
  VectorField *_psizeFld;
  //@}

  //@{\name Image minimum and maximum; defaults are 0.
  DoubleField *_minFld, *_maxFld;
  //@}

  //@{\name Voxel size; defaults are 0.
  DoubleField *_vXFld, *_vYFld, *_vZFld, *_VoxelVolFld, *_TotalVolFld, *_MegaVoxelFld;
  Vec3fField *_voxelSizeFld;
  //@}

  //@{\name Data type; default is "Unknown"
  StringField *_dTypeFld;
  //@}

  //@{\name Info string from PagedImg, eventually containing user description about invalidation.
  StringField *_pImgInfoFld;
  //@}

  //@{\name Data type information; default is a description string with many information lines.
  StringField *_typeInfoFld;
  //@}

#ifdef _ML_SHOW_EXTENDED_IMAGE_PROPERTIES
  //@{\name Shows values of image property extensions as string; default is empty string.
  StringField *_imgPropExtFld;
  //@}
#endif

  //@{\name World matrix; default is a 4x4 unit matrix.
  DoubleField *_x0Fld,  *_y0Fld,  *_z0Fld,  *_a0Fld,
              *_x1Fld,  *_y1Fld,  *_z1Fld,  *_a1Fld,
              *_x2Fld,  *_y2Fld,  *_z2Fld,  *_a2Fld,
              *_x3Fld,  *_y3Fld,  *_z3Fld,  *_a3Fld;
  MatrixField *_matFld;
  //@}

  ///TB 16.8.2001
  //@{\name Fields for C/T/U-Dim infos; defaults are empty strings.
  StringField *_cDimInfoFld, *_tDimInfoFld, *_uDimInfoFld;

  ///TB 15.11.2001: Added table for time points info; default is an empty string.
  StringField *_tDimTableFld;
  //@}

  //@{\name Update button.
  NotifyField *_updateFld;
  //@}

  //@{\name Memory image and operator infos.
  StringField *_memImgFld, *_memImg2Fld, *_opNameFld;
  //@}

  //! Shows the input from which bypassing is used; -1 on default and if bypassing is off.
  IntField    *_bypassIndexFld;

  //! Shows the input from which inplace is used; -1 on default and if inplace is off.
  IntField    *_inPlaceIndexFld;

  //@{\name Connection state fields; defaults are false.
  BoolField *_inConnFld, *_inputValidFld;
  //@}

  DoubleField *_originXFld, *_originYFld, *_originZFld;
  Vec3fField *_originFld;

  DoubleField *_extentXFld, *_extentYFld, *_extentZFld;
  Vec3fField *_extentFld;

  DoubleField *_wSizeXFld, *_wSizeYFld, *_wSizeZFld;
  Vec3fField *_wSizeFld;

  DoubleField *_centerXFld, *_centerYFld, *_centerZFld;
  Vec3fField *_centerFld;

  SubImgBoxfField *_boxFld;
  BoolField *_alignToAxisFld;
  BoolField *_useVoxelCenterFld;
  BoolField *_includeAllVoxelsFld;

  RotationField *_rotationFld;

  //----------------------------------------------------------------------
  //! Macro implementing the interface for the runtime system,
  //! defined in mlRuntimeSubClass.h.
  //----------------------------------------------------------------------
  ML_BASEOP_CLASS_HEADER(WorldInfo);

};

ML_END_NAMESPACE

#endif // of __mlWorldInfo_H
