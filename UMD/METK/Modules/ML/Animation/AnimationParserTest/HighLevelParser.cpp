
#ifdef WIN32
#pragma warning (disable : 4275)
#pragma warning (disable : 4503)
#pragma warning (disable : 4786)
#endif


#include <string>
#include <iostream>
#include <fstream>

#include <algorithm> //für replace


// Local includes
//#include "UMDAnimationSystem.h"

// Include base-class header.
#include <ObjMgr/mlObjMgrClient.h>

// ML includes
#include <mlOperatorIncludes.h>


# include "XVEnterScope.h"
# include <Inventor/nodes/SoCube.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
# include "XVLeaveScope.h"


#ifndef _BUILD_ANIMATIONPARSERDLL
#define __IMPORTEXPORT __declspec(dllimport)
#else
#define __IMPORTEXPORT __declspec(dllexport)
#endif

#include "AnimationParser.h"
#include "kScriptFrameList.h"
#include "kBasics.h"


using namespace std;

/*__declspec(dllimport) int openAndParseLLScript(string LLFileName, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList);
__declspec(dllimport) int openAndParseHLScript(string HLFileName, string LLFileName, string CommandXMLFileName, string ObjectNameXMLFileName);*/


int main(int argC, char* argV[])
{

	/*if (argC!=2)
	{
		cerr << "Nenene ... da stimmt mit den Parametern was nicht\n";
		return 1;
	}*/

	string _tempStr;

	AnimationParser* myAnimationParser = new AnimationParser();

	myAnimationParser->setCommandXMLFileName("e:/Modules/MLabModules/Projects/Animation/Scripts/Luebeck/HighLevel-Commands.xml");
	myAnimationParser->setObjectXMLFileName("G:/Leber/Tumor/Luebeck/Pat_EGS/ObjectNames.xml");
	myAnimationParser->openAndParseHLScript("e:/Modules/MLabModules/Projects/Animation/Scripts/Luebeck/init-script.txt","e:/Modules/MLabModules/Projects/Animation/Scripts/Luebeck/LowLevel-08.txt");

	/*list<string> objList;
	myAnimationParser->getObjectStringList("Lunge",objList);

	list<string>::const_iterator iter;
	for ( iter = objList.begin();iter!=objList.end(); iter++)
	{
		string tempName = string(*iter);
		cout << tempName << endl;
	}*/



	/*kScriptFrameList* myList;

	myList = new kScriptFrameList();

	kScriptIniObject* myIniObject = new kScriptIniObject();
	list<kScriptCommandObject>* myLLCommandList = new list<kScriptCommandObject>;
	myAnimationParser->openAndParseLLScript("E:/kmuehler/VisualC++/HighLevelParser/v0.2.dll/Release/LowLevel-01.txt", myIniObject, myLLCommandList);


	myList->setIniObject(*myIniObject);

	list<kScriptCommandObject>::const_iterator iter;
	for ( iter = myLLCommandList->begin();iter!=myLLCommandList->end(); iter++)
	{
		kScriptCommandObject tempCommandObj = kScriptCommandObject(*iter);

		cout << "InsertCommand ..." << tempCommandObj.getCommandStr() << endl;
		int insertFailure = myList->insertCommand(&tempCommandObj); //Wenn es hier zu Fehlern kommen, kann es sein, das myList nicht initialisiert wurde

		if (insertFailure!=0)
		{
			cout << "Command nicht in Liste eingefügt" << "code:" << kBasics::IntToString(insertFailure) << "   " << tempCommandObj.getCommandStr() << "," << tempCommandObj.getObjectName() << endl;
		}
	}


	myList->printList();*/




	/*list<kScriptFrameEntry>* globalFrameEntryList;
	list<kScriptFrameEntry>::const_iterator globalFrameEntryListIter;
	cout << "1" << endl;
	globalFrameEntryList = myList->FrameList[0].getList();
	cout << "2" << endl;
	if (globalFrameEntryList)
	{
		cout << "3" << endl;
		globalFrameEntryListIter = globalFrameEntryList->begin();
	}

	cout << "4" << endl;
	kScriptFrameEntry tempEntry = kScriptFrameEntry(*globalFrameEntryListIter);
	cout << "a" << endl;
	cout << "b" << endl;

	string xyz;
	xyz = tempEntry.getObjectName();

	cout << xyz << endl;

	cout << tempEntry.getCommandStr() << endl;

	std::vector<string> tempEntryParams;
	tempEntryParams = tempEntry.getParams(); //Die Größe wird später nicht abgefragt. Da zerreißt es mich hier höllisch, wenn ich auf ..[0] zugreife und gar kein Parameter existiert ... da muss man mal irgendein Exception-Konzept erarbeiten

	cout << "c" << endl;*/


/*	SbVec3f oldPosition(2,1,0);
	SbVec3f newPosition;
	SbVec3f rotCentre(0,1,0);

	SbRotation* camRot = new SbRotation(SbVec3f(0,1,0),0.5);

	camRot->multVec(oldPosition-rotCentre,newPosition);

	newPosition = newPosition + rotCentre;


	cout << newPosition[0] << "," << newPosition[1] << "," << newPosition[2] << endl;*/

/*	SbMatrix* myMatrix = new SbMatrix(0.507000,0.000000,0.000000,-73.946000,
									  0.000000,0.507000,0.000000,-81.118000,
									  0.000000,0.000000,0.399998,-1062.700000,
									  0.000000,0.000000,0.000000,1.000000);


	SbMatrix inverseM = myMatrix->inverse();

	SbMat output,output2;
	inverseM.getValue(output);
	myMatrix->getValue(output2);

	cout << output[0][0] << " " << output[0][1] << " " << output[0][2] << " " << output[0][3] << endl;
	cout << output[1][0] << " " << output[1][1] << " " << output[1][2] << " " << output[1][3] << endl;
	cout << output[2][0] << " " << output[2][1] << " " << output[2][2] << " " << output[2][3] << endl;
	cout << output[3][0] << " " << output[3][1] << " " << output[3][2] << " " << output[3][3] << endl << endl;


	SbVec3f oldPos,newPos;
	oldPos.setValue(205,165,400);
	newPos.setValue(0,0,0);

	myMatrix->multMatrixVec(oldPos,newPos);

	cout << "NewPos:" << endl;
	cout << newPos[0] << "," << newPos[1] << "," << newPos[2] << endl;*/


	return 0;
}