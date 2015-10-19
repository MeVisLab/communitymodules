#include "vpPoint.h"

CvpPoint::CvpPoint()
{
  x = y = z = 0;
}

CvpPoint::CvpPoint(float x, float y, float z)
{
  this->x = x;
  this->y = y;
  this->z = z;
  iNr++;
  if (vPointContainer) vPointContainer->push_back(this);
}

int CvpPoint::removeNeighbour (CvpPoint* oPoint)
{
  for (unsigned int i=0; i<vNeighbours.size(); i++)
  {
    if (vNeighbours[i]==oPoint)
    {
      vNeighbours.erase(vNeighbours.begin()+i);
      return 1;
    }
  }
  return 0;
}

void CvpPoint::addNeighbour (CvpPoint* oPoint)
{
  vNeighbours.push_back(oPoint);
}

int CvpPoint::getNrOfNeighbours (void)
{
  return vNeighbours.size();
}

CvpPoint* CvpPoint::getNeighbour (int i)
{
  if ((unsigned int)i >= vNeighbours.size()) return 0;
  return vNeighbours[i];
}

int CvpPoint::compareTo (CvpPoint* oPoint)
{
  if      ( x > oPoint->x) return 1;
  else if ( x < oPoint->x) return -1;
  else if ( y > oPoint->y) return 1;
  else if ( y < oPoint->y) return -1;
  else if ( z > oPoint->z) return 1;
  else if ( z < oPoint->z) return -1;
  return 0;
}

int CvpPoint::iNr = 0;
vector<CvpPoint*> *CvpPoint::vPointContainer = 0;

