//----------------------------------------------------------------------------------
//! The ML module class RegionDistanceTransformation.
/*!
// \file    mlRegionDistanceTransformation.h
// \author  Konrad Muehler
// \date    2009-03-17
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef __mlRegionDistanceTransformation_H
#define __mlRegionDistanceTransformation_H

// small image interface include
#include "SmallImageInterface.h"

#include <map>
#include <vector>
#include <list>
#include "labField.h"
//#include "kBasics.h"
//#include "HashTable.h"

// Local includes
#include "UMDRegionDistanceTransformationSystem.h"

//! 
class UMDREGIONDISTANCETRANSFORMATION_EXPORT RegionDistanceTransformation : public SmallImageInterface
{
public:
	//! the image processing is done in this method
	virtual void _processImages();

	//! write initialization code herein
	virtual void _initializeAll();

private:
	
	labField *_globalDistanceField;
	labField *_componentIDs;
	void calcGlobalDistanceField();
	int calcConnectedComp();

};

#endif // __mlRegionDistanceTransformation_H


