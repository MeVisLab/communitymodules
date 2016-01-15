//----------------------------------------------------------------------------------
//! The ML module class MatlabScriptWrapper.
/*!
// \file    mlMatlabScriptWrapperOutputData.h
// \author  Jens Kaftan, Alexander Broersen
// \date    2010-09-15
//
// Class for storing output data created in Matlab
*/
//----------------------------------------------------------------------------------


#ifndef __mlMatlabScriptWrapperOutputData_H
#define __mlMatlabScriptWrapperOutputData_H

// SDK includes
#include <mlBasics.h>

// Include the Matlab engine. Note that the correct path to this
// file must be set in the MLMatlabScriptWrapper.pro file.
#include "engine.h"

ML_START_NAMESPACE

//! The output memory management class used in MatlabScriptWrapper
class MatlabScriptWrapperOutputData
{
public:
	//! Constructor.
	MatlabScriptWrapperOutputData (void)
	{
		_data = NULL;
		_dataClass = ML_BAD_DATA_TYPE;
		_ext = Vector();
		_minval = 0;
		_maxval = 0;
	};
	//! Destructor.
	~MatlabScriptWrapperOutputData ()
	{
		if(_data) {
			delete []_data;
			_data = NULL;
		}
	};

	//! Copy new data to class (and deallocate old data if existent)
	void SetData(const Vector& extend, const MLPhysicalDataType datatype, const double* data)
	{
		if(_data != NULL) {
			delete []_data;
			_data = NULL;
		}
		_ext = extend;
		_dataClass = datatype;

		SubImgBox temp = SubImgBox(extend);
		int elementSize = 0;
		try {
			switch (_dataClass) {
				case MLdoubleType: elementSize = sizeof(double);   _data = new double[temp.getSize()];   break;
				case MLfloatType:  elementSize = sizeof(float);    _data = new float[temp.getSize()];    break;
				case MLint8Type:   elementSize = sizeof(int8_T);   _data = new int8_T[temp.getSize()];   break;
				case MLuint8Type:  elementSize = sizeof(uint8_T);  _data = new uint8_T[temp.getSize()];  break;
				case MLint16Type:  elementSize = sizeof(int16_T);  _data = new int16_T[temp.getSize()];  break;
				case MLuint16Type: elementSize = sizeof(uint16_T); _data = new uint16_T[temp.getSize()]; break;
				case MLint32Type:  elementSize = sizeof(int32_T);  _data = new int32_T[temp.getSize()];  break;
				case MLuint32Type: elementSize = sizeof(uint32_T); _data = new uint32_T[temp.getSize()]; break;
				case MLint64Type:
				default:
					std::cerr << "MatlabScriptWrapperOutputData::SetData - Output type from Matlab not supported" << std::endl;
			}
			if(_data != NULL) {
				memcpy(_data, data, temp.getSize()*elementSize);
			}
		}
		catch(std::bad_alloc) {
			std::cerr << "MatlabScriptWrapperOutputData::SetData - " << "error during memory allocation" << std::endl;
		}
	};

	void SetMinMaxValue(const MLldouble min, const MLldouble max)
	{
		_minval = min;
		_maxval = max;
	};

	inline void* GetData() { return(_data); };
	inline MLPhysicalDataType GetDataType() { return(_dataClass); };
	inline Vector GetExtend() { return(_ext); };
	inline MLldouble GetMinValue() { return(_minval); };
	inline MLldouble GetMaxValue() { return(_maxval); };

private:
	void* _data;
	MLPhysicalDataType _dataClass;
	Vector _ext;
	MLldouble _minval;
	MLldouble _maxval;
};

ML_END_NAMESPACE


#endif // __mlMatlabScriptWrapperOutputData_H
