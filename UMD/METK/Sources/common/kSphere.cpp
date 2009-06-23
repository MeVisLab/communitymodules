//----------------------------------------------------------------------------------
/*!
// \file    kSphere.cpp
// \author  Konrad Mühler
// \date    2006-11-16
//
// 
*/
//----------------------------------------------------------------------------------


//#include "stdafx.h"//muss mit rein und hat was mit vorkompilierten headern zu tun
#include "kSphere.h"


namespace kSphere
{
	void getPoints(SbVec3f center, const double radius, list<SbVec3f>* points, int recursiondeep)
	{
		vector<SbVec3f> start;
		SbVec3f a,b,c,d,e,f;
		a.setValue(radius,0,0); b.setValue(0,radius,0); c.setValue(0,0,radius); d.setValue(-radius,0,0); e.setValue(0,-radius,0); f.setValue(0,0,-radius);
		start.push_back(a);start.push_back(b);start.push_back(c);start.push_back(d);start.push_back(e);start.push_back(f);
		//points->push_back(a);points->push_back(b);points->push_back(c);points->push_back(d);points->push_back(e);points->push_back(f);

		kSphere::subdivide(recursiondeep-1,&start[0],&start[1],&start[2], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[0],&start[2],&start[4], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[0],&start[4],&start[5], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[0],&start[5],&start[1], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[3],&start[2],&start[1], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[3],&start[1],&start[5], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[3],&start[5],&start[4], &center, radius, points);
		kSphere::subdivide(recursiondeep-1,&start[3],&start[4],&start[2], &center, radius, points);

		std::cout << points->size() << "  points before cleanup" << std::endl;
		points = clearDoubles(points);

		std::cout << points->size() << "  points generated" << std::endl;
	}


	void scalevec(float length,SbVec3f* vec)
	{
		int i;
		float oldlength = sqrt((*vec)[0]*(*vec)[0]+(*vec)[1]*(*vec)[1]+(*vec)[2]*(*vec)[2]);
		for (i=0;i<3;i++)
		{
			(*vec)[i] = (*vec)[i]*length/oldlength;
		}
	}

	
	void subdivide(int rek, SbVec3f* pa, SbVec3f* pb, SbVec3f* pc, SbVec3f* center, float radius, list<SbVec3f>* points)
	{		
		if (rek>0)
		{
			SbVec3f pab,pbc,pca;
			//Normal of triangle
			//SbVec3f normalVec = ((*pa)-(*pb)).cross((*pb)-(*pc));
			//normalVec.normalize();			

			/*SbVec3f x;
			x= *pb;
			x.normalize();
			if (x.dot(normalVec)<-0.001)
			{
				normalVec = -normalVec;
				//return;
			}*/



			//New point  sphere center -> normalVec
			//normalVec = normalVec*radius+(*center);			
			//points->push_back(normalVec);

			

			/*std::cout << "pa:" << kBasics::Vec3fToString(*pa) << std::endl;
			std::cout << "pb:" << kBasics::Vec3fToString(*pb) << std::endl;
			std::cout << "pc:" << kBasics::Vec3fToString(*pc) << std::endl;
			std::cout << "normalVec:" << kBasics::Vec3fToString(normalVec) << std::endl << std::endl;*/


			for (int i=0;i<3;i++)
			{
	    		pab[i] = ((*pa)[i]+(*pb)[i])/2;
	    		pbc[i] = ((*pb)[i]+(*pc)[i])/2;
	    		pca[i] = ((*pc)[i]+(*pa)[i])/2;
			}
			scalevec(radius,&pab);
			scalevec(radius,&pbc);
			scalevec(radius,&pca);
			subdivide(rek-1,pa,&pab,&pca,center, radius, points);
			subdivide(rek-1,&pab,pb,&pbc,center, radius, points);
			subdivide(rek-1,&pbc,pc,&pca,center, radius, points);
			subdivide(rek-1,&pab,&pbc,&pca,center, radius, points);	

		}
		else
		{

			points->push_back(*pa + *center);
			points->push_back(*pb + *center);
			points->push_back(*pc + *center);
		}

		return;
    }


	list<SbVec3f>* clearDoubles(list<SbVec3f>* points)
	{
		bool doubleFound;
		float x,y,z,x2,y2,z2;
		list<SbVec3f>* newPoints = new list<SbVec3f>;		
		list<SbVec3f>::const_iterator iter,iter2;
		for (iter=points->begin(); iter!=points->end(); iter++)
		{	
			SbVec3f pointToTest = SbVec3f(*iter);
			
			pointToTest.getValue(x,y,z);
			doubleFound = false;
			for (iter2=newPoints->begin(); iter2!=newPoints->end(); iter2++)
			{
				SbVec3f pointToTest2 = SbVec3f(*iter2);
				pointToTest2.getValue(x2,y2,z2);
				if (x==x2 && y==y2 && z==z2)
				{
					//std::cout << "ping" << std::endl;
					doubleFound=true;				
				}
			}

			if (!doubleFound)
			{
				newPoints->push_back(pointToTest);
				//std::cout << "--poong" << std::endl;
			}
			
			//std::cout << "-----------" << std::endl;
		}

		return newPoints;		
	}

}