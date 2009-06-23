
#ifdef WIN32
#pragma warning (disable : 4275)
#pragma warning (disable : 4503)
#pragma warning (disable : 4786)
#endif


//#include <string>
//#include <iostream>
//#include <fstream>

#include <algorithm> //für replace


#include <Inventor/SbLinear.h>


#ifndef _BUILD_ANIMATIONPARSERDLL
#define __IMPORTEXPORT __declspec(dllimport)
#else
#define __IMPORTEXPORT __declspec(dllexport)
#endif

#include "AnimationParser.h"
#include "kBasics.h"


using namespace std;


class __IMPORTEXPORT AnimationParser;

void printQ(SbRotation rot)
{
	float q0,q1,q2,q3;
	rot.getValue(q0,q1,q2,q3);
	cout << q0 << "  " << q1 << "  " << q2 << "  " << q3 << "  " << endl;
}



int main(int argC, char* argV[])
{
	string _tempStr;

	stringstream LLStream;

	LLStream << "abcde" << endl;

	//char l[256];
	//if (LLStream.getline(l,256))
	//	cout << l;

	AnimationParser* myAnimationParser = new AnimationParser();
	string tempError = myAnimationParser->setCommandXMLFileName("e:/Modules/MLabModules/ML/Animation/Scripts/ENT/HighLevel-Commands.xml");
	if (tempError!=AnimationParser::ERROR_NONE)
		return 0;
	tempError = myAnimationParser->setObjectXMLFileName("e:/Modules/MLabModules/ML/Animation/Scripts/ENT/ObjectNames.xml","");
	if (tempError!=AnimationParser::ERROR_NONE)
		return 0;

	std::stringstream myHL;
	std::stringstream myLL;

	myHL << "[Ini]\nLengthTimeUnit=25\nLengthSeconds=30\nRealTime=no\n\n";
	myHL << "[Script]\n[0] 'System' setBackground white\n";


	string parseFailure = myAnimationParser->openAndParseHLStream(myHL.str(), myLL);
	cout << "Error: " << parseFailure << endl;
	parseFailure = myAnimationParser->openAndParseHLScript("e:/Modules/MLabModules/ML/Animation/Scripts/ENT/miccai-hals31-A.txt", "e:/Modules/MLabModules/ML/Animation/Scripts/ENT/ll.txt");
	cout << "Error: " << parseFailure << endl;
	cout << myLL.str();


	kScriptIniObject myIniObject;
	list<kScriptCommandObject>* myLLCommandList = new list<kScriptCommandObject>;
	parseFailure = myAnimationParser->openAndParseLLStream(myLL.str(), &myIniObject, myLLCommandList);

	return 0;
}