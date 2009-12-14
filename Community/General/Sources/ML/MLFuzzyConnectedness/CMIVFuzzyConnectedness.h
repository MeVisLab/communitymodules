/*=========================================================================

CMIVFuzzyConnectedness

A competing fuzzy connectedness tree algorithm

Reference:
1. Tizon X, Smedby Ö. Segmentation with gray-scale connectedness
can separate arteries and veins in MRA. J Magn Reson Imaging 2002;
15(4):438-45.
2. Löfving A, Tizon X, Persson P, Smedby Ö. Angiographic 
visualization of the coronary arteries in computed tomography
angiography with virtual contrast injection. The Internet Journal
of Radiology 2006;4(2)
3. Wang C, Smedby Ö. Coronary artery Segmentation and Skeletonization
In Volumetric Medical Images based on Competing Fuzzy Connectedness
Tree. Accepted by MICCAI 2007.

Author: Chunliang Wang (chunliang.wang@imv.liu.se)

Copyright (c) 2007-2009
Center for Medical Image Science and Visualization (CMIV),
Linköping University, Sweden, http://www.cmiv.liu.se/
ALL RIGHTS RESERVED

This image processing plugin from CMIV is free software;
you can redistribute it and/or modify it under the terms of the
GNU Lesser General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option)
any later version.

This image processing plugin is distributed in
the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#ifndef __CMIVFuzzyConnectedness_H
#define __CMIVFuzzyConnectedness_H


class CMIVFuzzyConnectedness
{
public:
  
  CMIVFuzzyConnectedness();  
  void setProperty(long width, long height, long amount,  float minval);
  void startShortestPathSearchAsFloat(float* pIn, float* pOut, unsigned char* pMarker, unsigned char* pPointers);
  
private:
  long imageWidth,imageHeight,imageAmount,imageSize;
  float *inputData;
  float *outputData;
  unsigned char* directionOfData;
  unsigned char* colorOfData;

  float minmumValue;
  
  void runFirstRoundFasterWith26Neigbhorhood();
  void checkSaturatedPoints();
  void caculateColorMapFromPointerMap(unsigned char* pColor, unsigned char* pPointers);
  unsigned char colorOfParent(int pointer);
};
#endif // __CMIVFuzzyConnectedness_H
