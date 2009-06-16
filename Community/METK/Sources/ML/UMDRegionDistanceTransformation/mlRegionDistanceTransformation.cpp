//----------------------------------------------------------------------------------
//! The ML module class RegionDistanceTransformation.
/*!
// \file    mlRegionDistanceTransformation.cpp
// \author  Konrad Muehler
// \date    2009-03-17
//
// 
*/
//----------------------------------------------------------------------------------

// Local includes
#include "mlRegionDistanceTransformation.h"

//ML_START_NAMESPACE

//! write your initialization (e.g. fields, number of inputs/outputs) in this method
void RegionDistanceTransformation::_initializeAll() {
	_globalDistanceField = NULL;
	_changeNumberInputs(1);
	_changeNumberOutputs(2);
}

//! write your image processing algorithms in this method
void RegionDistanceTransformation::_processImages() {

	Box extent = _getBox();

	if (_globalDistanceField) delete _globalDistanceField;
	_globalDistanceField = new labField((extent.x1-extent.x0),(extent.y1-extent.y0));
	_globalDistanceField->fillWith(32000);
	_componentIDs = new labField((extent.x1-extent.x0),(extent.y1-extent.y0));
	_componentIDs->fillWith(-1.0);

	calcConnectedComp();

	calcGlobalDistanceField();

	int min,max;
	min=320000;
	max=-320000;
	double pixel;

	int x, y;
	for (y = extent.y0+1; y < extent.y1-1; y++) {
		for (x = extent.x0+1; x < extent.x1-1; x++) {
			pixel = _globalDistanceField->getValueAt(x, y);
			if (pixel>max) max=pixel;
			if (pixel<min) min=pixel;
			_setPixelOut(0,x, y, 0, pixel);
			pixel = _componentIDs->getValueAt(x, y);			
			_setPixelOut(1,x, y, 0, pixel);
		}
	}
	std::cout << "min=" << min << "  max=" << max << std::endl;
}


void RegionDistanceTransformation::calcGlobalDistanceField()
{
	ML_TRACE_IN("void RegionDistanceTransformation::calcGlobalDistanceField()");

	//Create distance field
	//First only for background or not
	unsigned long int pos;
	float a,b,c,d;
	int currentDistance = 0;
	float currentComponentID;
	Box extent = _getBox();
	for (int i=extent.x0+1; i<extent.x1-1; i++) //Rand lassen
	{
		for (int j=extent.y0+1; j<extent.y1-1; j++) //Rand lassen
		{			
			pos = j*(extent.x1-extent.x0)+i;
			currentDistance = _globalDistanceField->getValueAt(pos);
			a=_globalDistanceField->getValueAt(pos-1);
			b=_globalDistanceField->getValueAt(pos+1);
			c=_globalDistanceField->getValueAt(pos-(extent.x1-extent.x0));
			d=_globalDistanceField->getValueAt(pos+(extent.x1-extent.x0));
			currentComponentID = _componentIDs->getValueAt(i,j);

			if (currentComponentID==0.0) //is pixel a background pixel
			{			
				currentDistance = min(currentDistance,min(a+1,min(b+1,min(c+1,d+1))));
			}
			else
			{
				//Is there a background pixel in the 4-nei. then its a border pixel with distance=0
				//better: pixel that not belongs to component
				if (_componentIDs->getValueAt(i-1,j)!=currentComponentID ||
					_componentIDs->getValueAt(i+1,j)!=currentComponentID ||
					_componentIDs->getValueAt(i,j-1)!=currentComponentID ||
					_componentIDs->getValueAt(i,j+1)!=currentComponentID)
				{
					currentDistance = 0;
				}
				else
				{
					currentDistance = min(currentDistance,min(a+1,min(b+1,min(c+1,d+1))));
				}
			}
			_globalDistanceField->setValueAt(pos,currentDistance);
		}
	}

	//And reverse
	for (int i=extent.x1-2; i>extent.x0; i--) //Rand lassen
	{
		for (int j=extent.y1-2; j>extent.y0; j--) //Rand lassen
		{			
			pos = j*(extent.x1-extent.x0)+i;
			currentDistance = _globalDistanceField->getValueAt(pos);
			a=_globalDistanceField->getValueAt(pos-1);
			b=_globalDistanceField->getValueAt(pos+1);
			c=_globalDistanceField->getValueAt(pos-(extent.x1-extent.x0));
			d=_globalDistanceField->getValueAt(pos+(extent.x1-extent.x0));
			currentComponentID = _componentIDs->getValueAt(i,j);

			if (currentComponentID==0.0) //is pixel a background pixel
			{			
				currentDistance = min(currentDistance,min(a+1,min(b+1,min(c+1,d+1))));
			}
			else
			{
				//Is there a background pixel in the 4-nei. then its a border pixel with distance=0
				if (_componentIDs->getValueAt(i-1,j)!=currentComponentID ||
					_componentIDs->getValueAt(i+1,j)!=currentComponentID  ||
					_componentIDs->getValueAt(i,j-1)!=currentComponentID  ||
					_componentIDs->getValueAt(i,j+1)!=currentComponentID)
				{
					currentDistance = 0;
				}
				else
				{
					currentDistance = min(currentDistance,min(a+1,min(b+1,min(c+1,d+1))));
				}
			}
			_globalDistanceField->setValueAt(pos,currentDistance);
		}
	}
}




int RegionDistanceTransformation::calcConnectedComp()
{
	ML_TRACE_IN("int RegionDistanceTransformation::calcConnectedComp()");

	//Calculate the connected components
	multimap<int,int> voxelToWatch;
	unsigned long int pos;
	multimap<int,int>::iterator tempVoxel;
	int currentCompID=0;
	int tempX,tempY;
	float currentObjID;
	Box extent = _getBox();
	for (int i=extent.x0; i<extent.x1; i++)
	{
		for (int j=extent.y0; j<extent.y1; j++)
		{
			pos = j*(extent.x1-extent.x0)+i;
			if (_componentIDs->getValueAt(pos)==-1.0)
			{				
				currentCompID++;
				currentObjID = _getPixel(i,j,0);

				//Find neigbourhs
				voxelToWatch.clear();
				voxelToWatch.insert(pair<int,int>(i,j));
				while (voxelToWatch.size()>0)
				{
					//pop first voxel
					tempVoxel = voxelToWatch.begin();
					tempX=(int)tempVoxel->first;
					tempY=(int)tempVoxel->second;
					pos = tempY*(extent.x1-extent.x0)+tempX;
					voxelToWatch.erase(voxelToWatch.begin());

					if (_componentIDs->getValueAt(pos)==-1.0)
					{
						if (tempX<extent.x1-1)						
							if (_getPixel(tempX+1,tempY,0)==currentObjID
								&& _componentIDs->getValueAt(pos+1)==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX+1,tempY));												

						if (tempX>0)
							if (_getPixel(tempX-1,tempY,0)==currentObjID
								&& _componentIDs->getValueAt(pos-1)==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX-1,tempY));

						if (tempY<extent.y1-1)
							if (_getPixel(tempX,tempY+1,0)==currentObjID 
								&& _componentIDs->getValueAt(pos+(extent.x1-extent.x0))==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX,tempY+1));

						if (tempY>0)
							if (_getPixel(tempX,tempY-1,0)==currentObjID
								&& _componentIDs->getValueAt(pos-(extent.x1-extent.x0))==-1.0)
								voxelToWatch.insert(pair<int,int>(tempX,tempY-1));
					}

					if (currentObjID==0)
					{
						_componentIDs->setValueAt(pos,0);
					}
					else
					{
						int oldValue = _componentIDs->getValueAt(pos);
						if (oldValue!=currentCompID)
						{
							_componentIDs->setValueAt(pos,currentCompID);							
						}
					}
				}
			}
		}
	}

	return currentCompID;
}