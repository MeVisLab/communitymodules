//----------------------------------------------------------------------------------
/*!
// \file    kBasics.h
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------

#ifndef _KBASICS_
#define _KBASICS_


#pragma warning(disable:4251)

#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport) 
#endif
#else
#define MLIMAGEUTILITIES_EXPORT
#endif


#include <iostream>
#include <fstream>
#include <iomanip> 
#include <string>
#include <sstream> 
#pragma warning(disable:4786)
#include <list>
#include <vector>
#include <algorithm> //for string-replace
#include <limits>
//#include <math.h>
#include <Inventor/SbLinear.h>
#include <time.h>

//#pragma warning(disable:4251)


using namespace std;

//! kBasics contains all often used functions, i.e. for convertions. 
namespace kBasics
{
	struct optionstruct{
		string name;
		int count;
		char shortname;
		optionstruct(string n,int c,char s):name(n),count(c),shortname(s){}
		optionstruct():name(""),count(0),shortname(' '){}
	};
	const double PI = 3.1415926535897932384626433832795028841972;	

	const char QUOTATION_SINGLE = '\'';
	const char QUOTATION_DOUBLE = '\"';

	struct POINT {
		int x,y;
		POINT() : x(-1), y(-1) {} // default constructor
		POINT(int x_, int y_) : x(x_), y(y_) {} // constructor
	};

	struct POINTF {
		float x,y;
		POINTF() : x(-1), y(-1) {} // default constructor
		POINTF(float x_, float y_) : x(x_), y(y_) {} // constructor
	};


	struct LINE {
		POINT p1,p2;
	};

	struct LINEF {
		POINTF p1,p2;
	};


	struct BOX {
		POINT corner1,corner2,corner3,corner4;
	};



	//--------------------------------------------------------
	//Functions
	//--------------------------------------------------------
	__COMMON_IMPORTEXPORT void split(const string toSplit,const char delimiter, const int max_size, vector<string>* strVector);	
	__COMMON_IMPORTEXPORT void splitQuotated(const string toSplit, const char delimiter, const char quotationmark, vector<string>* strVector);
	__COMMON_IMPORTEXPORT const string CharToString(const char* value);
	__COMMON_IMPORTEXPORT const int StringToInt(const string value);
	__COMMON_IMPORTEXPORT const string IntToString(const int value);
	__COMMON_IMPORTEXPORT float StringToFloat(const string value);
	__COMMON_IMPORTEXPORT const string FloatToString(const float value);
	__COMMON_IMPORTEXPORT double StringToDouble(const string value);
	__COMMON_IMPORTEXPORT const string DoubleToString(const double value);
	__COMMON_IMPORTEXPORT const string BoolToString(const bool value);
	__COMMON_IMPORTEXPORT const bool StringToBool(const string value);
	__COMMON_IMPORTEXPORT const string eraseStrPart(const string toDelete, string orgString);
	__COMMON_IMPORTEXPORT const string replaceString(string s, const string value, const string replacement);
	__COMMON_IMPORTEXPORT const string replaceString(string s, const char value, const string replacement);
	__COMMON_IMPORTEXPORT const string trimString(string s, char c=' ');
	//__COMMON_IMPORTEXPORT const string trimStringLFCR(string s);
	__COMMON_IMPORTEXPORT const string trimQuotatedStr(string value, const char quotationmark);
	__COMMON_IMPORTEXPORT const string getPath(const string value, const char slash);
	__COMMON_IMPORTEXPORT const string getFilename(const string value, const char slash);
	__COMMON_IMPORTEXPORT const string Vec3fToString(const SbVec3f value,const char delimiter);
	__COMMON_IMPORTEXPORT const string Vec3fToString(const SbVec3f value);
	__COMMON_IMPORTEXPORT const SbVec3f StringToVec3f(const string value, const char delimiter);
	__COMMON_IMPORTEXPORT const string SbRotationToString(const SbRotation value);
	__COMMON_IMPORTEXPORT const string toUp(string value); //toUppercase
	__COMMON_IMPORTEXPORT const double maxDiff(const SbVec3f value1, const SbVec3f value2);
	__COMMON_IMPORTEXPORT const double maxDiff(const SbVec4f value1, const SbVec4f value2);	
	__COMMON_IMPORTEXPORT const bool fileExists(const char* fileName);
	__COMMON_IMPORTEXPORT const string leadingZero(int number, int totalLength);	
	__COMMON_IMPORTEXPORT const SbVec3f RGB_to_HSV( SbVec3f RGB );
	__COMMON_IMPORTEXPORT SbVec3f HSV_to_RGB( SbVec3f HSV );
	__COMMON_IMPORTEXPORT char getOptions(vector<string>& in,const list<optionstruct>& options,string& value);

	template <typename T>
	__COMMON_IMPORTEXPORT const short Signum(T value);
	__COMMON_IMPORTEXPORT const int round(float value);


	__COMMON_IMPORTEXPORT void DebugTest(string debugStr, short level);

	__COMMON_IMPORTEXPORT void initRand();
	__COMMON_IMPORTEXPORT int random( int a, int e);

	__COMMON_IMPORTEXPORT void quickSort(double* values, int start, int end);

	__COMMON_IMPORTEXPORT double getCurrentTime();

	__COMMON_IMPORTEXPORT bool lineIntersection(POINT p1, POINT p2, POINT p3, POINT p4, POINT &intersectionPoint);
	__COMMON_IMPORTEXPORT bool boxIntersection(POINT r1_corner1, POINT r1_corner2, POINT r2_corner1, POINT r2_corner2, BOX &intersectionBox);
	__COMMON_IMPORTEXPORT bool pointInBox(POINTF p1, POINT corner1, POINT corner2);

}

#endif // _KBASICS_