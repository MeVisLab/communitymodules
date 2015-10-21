//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_SharedTypes.h
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __DCM_SharedTypes_H
#define __DCM_SharedTypes_H

// Local includes
#include <boost/smart_ptr.hpp>
#include <dcmtk/dcmdata/dctk.h>

//ML includes
#include <mlModuleIncludes.h>
#include <mlXMarkerList.h>

ML_START_NAMESPACE

typedef boost::shared_ptr<DcmDataset> SmartPtr_DCMDataSet;








ML_END_NAMESPACE

#endif // __DCM_SharedTypes_H