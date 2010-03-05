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
//----------------------------------------------------------------------------------
//! The ML module class CSOSetProperties.
/*!
// \file    mlCSOSetProperties.h
// \author  Reinhard Hameeteman / Olaf Konrad
// \date    2007-11-06
//
// Set the properties of all cso in a list to a single value
*/
//----------------------------------------------------------------------------------


#ifndef __mlCSOSetProperties_H
#define __mlCSOSetProperties_H


// Local includes
#include "../MLCSOCommunityModulesSystem.h"

// ML includes
#include "mlOperatorIncludes.h"

#include "CSOBase/CSOList.h"


ML_START_NAMESPACE


//! Set the properties of all cso in a list to a single value
class MLCSOCOMMUNITYMODULES_EXPORT CSOSetProperties : public BaseOp
{
public:

  //! Constructor.
  CSOSetProperties (void);

  //! Destructor
  ~CSOSetProperties();

  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);
  void SetProperties(bool shouldSetupInternalCSOList);

  //! Sets up the internal pointer to the input CSOList.
  void SetupInternalCSOList();

  //! Parses the CSO input string and fills the internal _filteredCSOIds vector.
  void ParseInputCSOString();

  //! Adds the id of a CSO to the internal list if the given id (as a string) does exist.
  void AddCSOId(const std::string& idString);

  //! Parses the CSO input string and fills the internal _filteredCSOIds vector.
  void ParseInputGroupString();

  //! Adds the id of a CSO to the internal list if the given id (as a string) does exist.
  void AddGroupId(const std::string& idString);

  static void CSOListNotifyObserverCB(void* userData, int notificationFlag);

private:


  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(CSOSetProperties);

  //! The input CSOList.
  CSOList    *m_InputCSOList;
  BaseField  *f_InputCSOList;

  //! The input CSOList.
  CSOList    *m_OutputCSOList;
  BaseField  *f_OutputCSOList;

  //! Input string for determining the CSOs that are to be modified.
  StringField* f_InputCSOString;

  //! Input string for determining the Groups that are to be modified.
  StringField* f_InputGroupString;

  //! Set the input string to CSOs or Groups
  EnumField *f_InputMode;
  enum mode { CSOINPUT, GROUPINPUT };

  //! Shall the module work directly on the input CSOList?
  //! Otherwise, it would work on a copy which is returned by the output field.
  BoolField* f_WorkDirectlyOnInputList;

  //! Shall the module listen to finishing events?
  BoolField* f_ListenToFinishingNotifications;

  //! Shall the module listen to selection change notifications?
  BoolField* f_ListenToSelectionNotifications;

  //! Auto update mode.
  BoolField* f_AutoUpdate;

  //! Applies changes.
  NotifyField* f_Update;

  //! Locking variable for removing/adding a notification observer.
  bool m_IsInNotificationCB;

  //! The ids of the selected CSOs.
  std::vector<unsigned int> m_selectedCSOIds;

  //! The ids of the selected Groups.
  std::vector<unsigned int> m_selectedGroupIds;

  //! Is this module notifying itself? Locking variable to avoid infinite loops.
  bool m_IsNotifyingMyself;



  // CSO props
  BoolField   *f_CSOComputeNormal;
  StringField *f_CSOLabel;
  StringField *f_CSODescription;
  BoolField   *f_CSOIsSelected;
  BoolField   *f_CSOClearSelection;
  FloatField  *f_CSOLineWidth;
  EnumField   *f_CSOLineStyle;
  ColorField  *f_CSOColor;
  FloatField  *f_CSOAlpha;
  IntField    *f_CSOTimePointIndex;
  EnumField   *f_CSOVoxelWriteMode;
  FloatField  *f_CSOVoxelWriteValue;
  BoolField   *f_CSOShowState;
  BoolField   *f_CSOVoxelizeState;
  BoolField   *f_CSOEditableState;
  EnumField   *f_CSOMarkerMode;
  ColorField  *f_CSOMarkerColor;
  FloatField  *f_CSOMarkerAlpha;
  FloatField  *f_CSOMarkerSize;

  //! Group props
  StringField *f_GroupLabel;
  StringField *f_GroupDescription;
  BoolField   *f_GroupIsSelected;
  BoolField   *f_GroupClearSelection;
  FloatField  *f_GroupLineWidth;
  EnumField   *f_GroupLineStyle;
  ColorField  *f_GroupColor;
  FloatField  *f_GroupAlpha;
  IntField    *f_GroupTimePointIndex;
  EnumField   *f_GroupVoxelWriteMode;
  FloatField  *f_GroupVoxelWriteValue;
  BoolField   *f_GroupShowState;
  BoolField   *f_GroupVoxelizeState;
  BoolField   *f_GroupEditableState;
  EnumField   *f_GroupMarkerMode;
  ColorField  *f_GroupMarkerColor;
  FloatField  *f_GroupMarkerAlpha;
  FloatField  *f_GroupMarkerSize;
  IntField    *f_GroupMaximumNumCSOs;
  EnumField   *f_GroupOverflowHandling;
  EnumField   *f_GroupRemoveFromGroupHandling;
  EnumField   *f_GroupDeleteGroupCSOHandling;

  BoolField   *f_GroupUseVisuals;
  BoolField   *f_GroupUseShowState;
  BoolField   *f_GroupUseVoxelizeState;
  BoolField   *f_GroupUseEditableState;
  BoolField   *f_GroupUseTimePointIndex;
  BoolField   *f_GroupUseVoxelWrite;
  BoolField   *f_GroupUseMarkerSettings;

  //! Determine which fields to set
  // CSO props
  BoolField *f_SetCSOComputeNormal;
  BoolField *f_SetCSOLabel;
  BoolField *f_SetCSODescription;
  BoolField *f_SetCSOIsSelected;
  BoolField *f_SetCSOLineWidth;
  BoolField *f_SetCSOLineStyle;
  BoolField *f_SetCSOColor;
  BoolField *f_SetCSOAlpha;
  BoolField *f_SetCSOTimePointIndex;
  BoolField *f_SetCSOVoxelWriteMode;
  BoolField *f_SetCSOVoxelWriteValue;
  BoolField *f_SetCSOShowState;
  BoolField *f_SetCSOVoxelizeState;
  BoolField *f_SetCSOEditableState;
  BoolField *f_SetCSOMarkerMode;
  BoolField *f_SetCSOMarkerColor;
  BoolField *f_SetCSOMarkerAlpha;
  BoolField *f_SetCSOMarkerSize;

  //! Group props
  BoolField *f_SetGroupLabel;
  BoolField *f_SetGroupDescription;
  BoolField *f_SetGroupIsSelected;
  BoolField *f_SetGroupLineWidth;
  BoolField *f_SetGroupLineStyle;
  BoolField *f_SetGroupColor;
  BoolField *f_SetGroupAlpha;
  BoolField *f_SetGroupTimePointIndex;
  BoolField *f_SetGroupVoxelWriteMode;
  BoolField *f_SetGroupVoxelWriteValue;
  BoolField *f_SetGroupShowState;
  BoolField *f_SetGroupVoxelizeState;
  BoolField *f_SetGroupEditableState;
  BoolField *f_SetGroupMarkerMode;
  BoolField *f_SetGroupMarkerColor;
  BoolField *f_SetGroupMarkerAlpha;
  BoolField *f_SetGroupMarkerSize;

  BoolField *f_SetGroupUseVisuals;
  BoolField *f_SetGroupUseShowState;
  BoolField *f_SetGroupUseVoxelizeState;
  BoolField *f_SetGroupUseEditableState;
  BoolField *f_SetGroupUseTimePointIndex;
  BoolField *f_SetGroupUseVoxelWrite;
  BoolField *f_SetGroupUseMarkerSettings;

  BoolField *f_SetGroupMaximumNumCSOs;
  BoolField *f_SetGroupOverflowHandling;
  BoolField *f_SetGroupRemoveFromGroupHandling;
  BoolField *f_SetGroupDeleteGroupCSOHandling;
};


ML_END_NAMESPACE

#endif // __mlCSOSetProperties_H


