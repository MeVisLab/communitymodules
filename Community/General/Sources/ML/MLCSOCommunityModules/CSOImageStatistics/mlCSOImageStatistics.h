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
//! The ML module class CSOImageStats.
/*!
// \file    mlCSOImageStatistics.h
// \author  Reinhard Hameeteman
// \date    2008-01-30
//
// Calculate image statistics at CSO position on a image
*/
//----------------------------------------------------------------------------------


#ifndef __mlCSOImageStatistics_H
#define __mlCSOImageStatistics_H


// Local includes
#include "../MLCSOCommunityModulesSystem.h"
#include "mlAPI.h"

// ML includes
#include "mlOperatorIncludes.h"

#include <mlDiagramData.h> 

#include "CSOBase/CSOList.h"
#include "CSOTools/CSOMath.h"
#include "mlKeyFrameList.h"
#include "mlXMarkerList.h"

ML_START_NAMESPACE


//! Calculate image statistics at CSO position on a image
class MLCSOCOMMUNITYMODULES_EXPORT CSOImageStatistics : public BaseOp
{
public:

  //! Constructor.
  CSOImageStatistics (void);

  ~CSOImageStatistics();

  //! Enumerator for memory handling
  enum MemoryAccess {
    PAGES = 0,
    MEMIMAGE,
    VIRTUALVOLUME
  };

  //! Enumerator for update mode
  enum UpdateMode {
    OFF = 0,
    AUTOUPDATE
  };

  
  //! Handle field changes of the field \c field.
  virtual void handleNotification (Field *field);

  //! Callback member for CSO notifications
  static void CsoListNotifyObserverCB(void* userData, int notificationFlag);

private:

  //! Implements interface for the runtime type system of the ML.
  ML_BASEOP_CLASS_HEADER(CSOImageStatistics);


  //! Setup the internal CSOList: private list or pointer to input list
  void SetupInternalCSOList();

  //! Parse the given Id string
  void ParseInputCSOString();

  //! Add CSO Id to the list that needs to be processed
  void AddCSOId(const std::string& idString);

  //! Process the determined Id list
  void ProcessCSOList(bool shouldSetupInternalCSOList);

  //! Get image statistics of cso
  void GetStatistics( CSO    *cso, 
                      size_t &voxelCount, 
                      double &sum, 
                      double &mean,
                      double &absMean,
                      double &minimum, 
                      double &absMin,
                      double &maximum,
                      double &absMax);

  //! Get a list of all points inside the contour defined by the pathPoints
  //! The contour should be axis aligned
  void GetPointsInsideContour( std::vector< vec3 > const &pathPoints, 
                               std::vector< vec3 >       &contourPoints,
                               SubImgBox const           &voxelBB);

  //! Convert the coordinates in the list from wold to voxel coordinates using the supplied image
  void ConvertCoorinateListToVoxel( std::vector< vec3 > &coordinateList, mlField* image);

  //! Set the output curves
  void SetCurves();

  //! Replace all occurences of the findString in sourceString with the replaceString and
  //! return the new string.
  std::string ReplaceString(std::string sourceString, std::string findString, std::string replaceString );

  //! \name Module Fields
  //@{
  //! The input CSOList.
  BaseField* f_InputCSOList;

  //! The output CSOList. This may contain additional description strings for each CSO
  BaseField* f_OutputCSOList;

  //! Input string for determining the CSOs that are to be processed.
  StringField* f_InputCSOString;

  //! Shall the module work directly on the input CSOList?
  //! Otherwise, it would work on a copy which is set at the output field.
  BoolField* f_WorkDirectlyOnInputList;

  //! Add the statistics as a string to the CSOList
  BoolField   *f_AddDescription;

  //! Format of the description string %sum and %average are replaced by their statistics
  StringField *f_DescriptionString;

  //! Output curves of the selected statistics?
  BoolField  *f_OutputCurve;

  //! Output curve field
  BaseField  *f_OutCurveList;

  //! Select which statistics to add as curve
  BoolField *f_OutputAverage;

  //! Select which statistics to add as curve
  BoolField *f_OutputSum;

  //! Output MarkerList field
  BaseField   *f_OutMarkerList;

  //! Shall the module listen to finishing events?
  BoolField* f_ListenToFinishingNotifications;

  //! Shall the module listen to selection change notifications?
  BoolField* f_ListenToSelectionNotifications;

  //! Enumeration field for update modes.
  EnumField* f_UpdateMode;

  //! Applies changes.
  NotifyField* f_Apply;
  
  //! Calculate statistics on every point inside the CSO
  BoolField   *f_UseAllPointsInsideCSO;

  //! Resolution of the image data inside the CSO
  FloatField  *f_Resolution;

  //! Use voxel size on calculation of the average
  BoolField   *f_UseVoxelSize;

  //! String containing all calculated statistics
  StringField *f_Statistics;

  DoubleField *f_TotalSum;
  DoubleField *f_TotalAverage;
  DoubleField *f_TotalMin;
  DoubleField *f_TotalMax;

  //! mode defines the resampling filter that is applied ("NearestNeighbor", "Trilinear" , "TrilinearSkipBorder")
  EnumField   *f_InterpolationMode;

  //! defines how to access the input image, ("Paged", "Global" , "VirtualVolume")
  EnumField   *f_MemoryAccessMode;

  //@}

  //! \name module member variables
  //@{
  //! A pointer to the input CSOList.
  CSOList* m_InCSOList;

  //! A pointer to the output CSOList.
  CSOList* m_OutCSOList;

  //! The ids of the selected CSOs.
  std::vector<unsigned int> m_SelectedCSOIds;

  //! The actual curveList
  CurveList  *m_OutCurveList;

  //! The CurveData for the average
  std::vector< double > m_AverageSeries;

  //! The CurveData for the sum
  std::vector< double > m_SumSeries;

  //! Output a MarkerList containing all positions that are used to calculate the statistics
  //! The type of each marker is set to the corresponding CSO Id
  XMarkerList *m_OutMarkerList;

  //! Locking variable for removing/adding a notification observer.
  bool m_IsInNotificationCB;

  //! Is this module notifying itself? Locking variable to avoid infinite loops.
  bool m_IsNotifyingMyself;

  //@}
  
};


ML_END_NAMESPACE

#endif // __mlCSOImageStatistics_H


