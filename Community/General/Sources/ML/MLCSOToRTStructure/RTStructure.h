//----------------------------------------------------------------------------------
//! The ML module class RTStructure.
/*!
// \file    RTStructure.h
// \author  linal
// \date    2010-01-15
//
// This is a class to hold RT Structure
*/
//----------------------------------------------------------------------------------


#ifndef __mlRTStructure_H
#define __mlRTStructure_H
#include "MLCSOToRTStructureSystem.h"
#include <DCMTree_Lib.h>
#include "DCMTree_IOParameter.h"
#include "DCMTree_Reader.h"
#include "DCMTree_Dict.h"
#include "DCMTree_Tag.h"
#include "DCMTree_Value.h"
#include "DCMTree_Unicode.h"
#include "DCMTree_Writer.h"
#include <CSOBase/CSOList.h>
#include "mlBase.h"

ML_START_NAMESPACE

class MLCSOTORTSTRUCTURE_EXPORT RTStructure : public Base
{
public:
	RTStructure(void)
	{
		
	}
	virtual ~RTStructure(void)
	{
        
	}

  
    std::stringstream stream;
	
private:
	
	
 ML_CLASS_HEADER(RTStructure)

};

ML_END_NAMESPACE

#endif