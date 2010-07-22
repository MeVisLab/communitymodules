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