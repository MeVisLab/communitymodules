//----------------------------------------------------------------------------------
/*!
// \file    AnimationParser.cpp
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------


#include "stdafx.h" //DLL-Standard
#include "AnimationParser.h"
#include <iostream>
#include <fstream>


const short AnimationParser::ST_NONE = 1;
const short AnimationParser::ST_INI = 2;
const short AnimationParser::ST_SCRIPT = 3;
const char AnimationParser::DELIMITER_SPACE = ' ';


const string AnimationParser::ERROR_NONE = "";
const string AnimationParser::ERROR_OPEN_LOWLEVEL = "Error opening lowlevel file / file not found?";
const string AnimationParser::ERROR_OPEN_HIGHLEVEL = "Error opening highlevel file / file not found?";
const string AnimationParser::ERROR_OPEN_OBJECTXML1 = "Error opening object xml file 1 / file not found?";
const string AnimationParser::ERROR_OPEN_OBJECTXML2 = "Error opening object xml file 2 / file not found?";
const string AnimationParser::ERROR_OPEN_COMMANDXML = "Error opening command xml file / file not found?";
const string AnimationParser::ERROR_CREATE_LOWLEVEL = "Error create lowlevel file!";
const string AnimationParser::ERROR_INI_HIGHLEVEL = "Error in ini part of high level script!";
const string AnimationParser::ERROR_INI_LOWLEVEL = "Error in ini part of low level script!";



AnimationParser::AnimationParser()
{
	//InitializeXML();
	XMLPlatformUtils::Initialize();

	//Initialisierung der char* mit leeren Strings
	CommandXMLFileName = "";
	ObjectXMLFileName1 = "";
	ObjectXMLString1 = "";
	ObjectXMLFileName2 = "";
}


AnimationParser::~AnimationParser()
{	
	XMLPlatformUtils::Terminate();
}


//! Counts the number of commands replace the command in the given node
/*! \param node command-node */
int AnimationParser::getReplaceCommandCount(DOMNode* node)
{
	int commandCount = 0;
	DOMNode* childNode;
	for (childNode = node->getFirstChild(); childNode != 0; childNode=childNode->getNextSibling())
		if (childNode->getNodeType()==DOMNode::ELEMENT_NODE)
			if (kBasics::toUp((string)XMLString::transcode(childNode->getNodeName()))=="COMMAND")
				commandCount++;

	delete childNode;
	return commandCount;
}



//! Checkt, ob ein Befehl eine Ersetzungsvorlage hat und gibt die ggf. als XML-Knoten zurück.
/*! \param root
 *  \param CommandStr
 *  \param minCommandCount 1, wenn mindestens ein Ersetzungskommando vorhanden sein muss, 0 bei z.B. Parametern, wenn nicht zwingend eine Ersetzung vom Befehl vorliegen muss.
 */
DOMNode* AnimationParser::getCommandReplaceNode(DOMNode* root, string CommandStr, int minCommandCount)
{	
	DOMNode* childNode;
	for (childNode = root->getFirstChild(); childNode != 0; childNode = childNode->getNextSibling())
	{
		if (childNode->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			//string nodeName = kBasics::toUp(XMLString::transcode(childNode->getNodeName()));
			string nodeName = XMLString::transcode(childNode->getNodeName());
			if (kBasics::toUp((string)XMLString::transcode(childNode->getNodeName())) == "COMMAND")
			{
				if (childNode->hasAttributes())
				{
					DOMNamedNodeMap *myAttributes = childNode->getAttributes();
					for (unsigned int i=0;i<myAttributes->getLength();i++)
					{
						DOMAttr *myAttributeNode = (DOMAttr*) myAttributes->item(i);                        
						if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getName()))=="COMMANDSTR" && 
							kBasics::toUp((string)XMLString::transcode(myAttributeNode->getValue()))==kBasics::toUp(CommandStr))
						{
							if (getReplaceCommandCount(childNode)>=minCommandCount)
							{
								return childNode;
							}
						}
						
					}
				}
			}
		}
	}	
	return NULL;
}



//! Checkt, ob ein Objekt eine Ersetzungsvorlage hat und gibt die ggf. als XML-Knoten zurück.
/*! \param root
 *  \param ObjectStr
 */
DOMNode* AnimationParser::getObjectReplaceNode(DOMNode* root, string ObjectStr)
{	
	if (root==NULL)
	{
		std::cout << "root=NULL in getObjectReplaceNode - invalid ObjXML" << std::endl;
		return NULL;
	}
	DOMNode* childNode;
	for (childNode = root->getFirstChild(); childNode != 0; childNode = childNode->getNextSibling())
	{
		if (childNode->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			//string nodeName = kBasics::toUp(XMLString::transcode(childNode->getNodeName()));
			string nodeName = XMLString::transcode(childNode->getNodeName());
			if (kBasics::toUp(nodeName) == "OBJECT")
			{
				if (childNode->hasAttributes())
				{
					DOMNamedNodeMap *myAttributes = childNode->getAttributes();
					for (unsigned int i=0;i<myAttributes->getLength();i++)
					{
						DOMAttr *myAttributeNode = (DOMAttr*) myAttributes->item(i);                        
						if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getName())) == "NAME" && 
							kBasics::toUp((string)XMLString::transcode(myAttributeNode->getValue())) == kBasics::toUp(ObjectStr))
						{
							return childNode;
						}
						
					}
				}
			}
		}
	}
	return NULL;
}



//! Checkt, ob ein Parameter eine Ersetzungsvorlage hat und gibt die ggf. als XML-Knoten zurück
/*! \param root
 *  \param CommandStr
 *  \param ParamStr
 */
DOMNode* AnimationParser::getParamReplaceNode(DOMNode* root, string CommandStr, string ParamStr)
{
	if (root==NULL)
	{
		std::cout << "root=NULL in getParamReplaceNode - invalid CommandXML?" << std::endl;
		return NULL;
	}
	if (DOMNode* commandNode = getCommandReplaceNode(root,CommandStr,0))	 	
	{
		DOMNode* childNode;	
		for (childNode = commandNode->getFirstChild(); childNode != 0; childNode = childNode->getNextSibling())
		{
			if (childNode->getNodeType() == DOMNode::ELEMENT_NODE)
			{
				//string nodeName = kBasics::toUp(XMLString::transcode(childNode->getNodeName()));
				string nodeName = XMLString::transcode(childNode->getNodeName());
				if (kBasics::toUp(nodeName) == "PARAMETER")
				{
					if (childNode->hasAttributes())
					{
						DOMNamedNodeMap *myAttributes = childNode->getAttributes();
						for (unsigned int i=0;i<myAttributes->getLength();i++)
						{
							DOMAttr *myAttributeNode = (DOMAttr*) myAttributes->item(i);                        
							if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getName())) == "PARAMSTR")
							{
								std::vector<string> splittedParam;
								kBasics::split(ParamStr, DELIMITER_SPACE,100,&splittedParam);
								for (unsigned int i=0;i<splittedParam.size();i++)
								{									
									if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getValue())) == kBasics::toUp(splittedParam[i]))
										return childNode;
								}
							}							
						}
					}
				}
			}
		}
	}
	return NULL;
}




//! Gibt die Liste von LLCommand-Objekten zurück, die das als XML-Knoten übergebene Command ersetzen sollen
/*! \param node */
AnimationParser::COLIST AnimationParser::getCommandList(DOMNode* node)
{
	COLIST tempList;
	if (node==NULL)
	{
		std::cout << "node=NULL in getCommandList" << std::endl;
		return tempList;
	}	
	DOMNode* childNode;
	for (childNode = node->getFirstChild(); childNode != 0; childNode = childNode->getNextSibling())
	{
		if (childNode->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			if (kBasics::toUp((string)XMLString::transcode(childNode->getNodeName())) == "COMMAND")
			{
				if (DOMNode* textChild = childNode->getFirstChild())
				{
					//string nodeValue = kBasics::toUp((string)XMLString::transcode(textChild->getNodeValue()));
					string nodeValue = (string)XMLString::transcode(textChild->getNodeValue());
					kScriptCommandObject tempObj(nodeValue);					
					tempList.push_back(tempObj);		
				}
			}
		}
	}	
	return tempList;
}



//! Gibt Liste von Objekten einer Ersetzungsliste im XML-Dokument zurück
/*! \param node */
list<string> AnimationParser::getObjectList(DOMNode* node)
{
	list<string> tempList;
	if (node==NULL)
	{
		std::cout << "node=NULL in getObjectList" << std::endl;
		return tempList;
	}	

	DOMNode* childNode;
	for (childNode = node->getFirstChild(); childNode != 0; childNode = childNode->getNextSibling())
	{
		if (childNode->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			if (kBasics::toUp((string)XMLString::transcode(childNode->getNodeName()))=="OBJECT")
			{
				if (DOMNode* textChild = childNode->getFirstChild())
				{
					//string nodeValue = kBasics::toUp((string)XMLString::transcode(textChild->getNodeValue()));
					string nodeValue = (string)XMLString::transcode(textChild->getNodeValue());
					nodeValue = kBasics::replaceString(nodeValue,0x09," "); //remove tabulator
					nodeValue = kBasics::replaceString(nodeValue,0x0d," "); //remove LF
					nodeValue = kBasics::replaceString(nodeValue,0x0a," "); //remove CR					
					nodeValue = kBasics::trimString(nodeValue);
					tempList.push_back(nodeValue);		
				}
			}
		}
	}	
	return tempList;
}



//! Check, ob Parameter eines Commands in der XML-Struktur stehen und ersetzt werden müssen
/*! \param myIniObject
 *  \param CommandObj
 */
bool AnimationParser::checkForParamReplace(kScriptIniObject& myIniObject, kScriptCommandObject& CommandObj)
{				
	float valuePerTimeUnit;
	string singleValue;
	string partOfParam;
	bool somethingChanged=false;

	//Param splitten
	std::vector<string> tempEntryParams;	
	kBasics::splitQuotated(CommandObj.getParamStr(),kScriptFrameEntry::DELIMITER_PARAMS,kBasics::QUOTATION_SINGLE,&tempEntryParams);	
	
	//for each part
	string gesamtParam = "";
	for (size_t j=0; j<tempEntryParams.size(); j++)
	{
		partOfParam = tempEntryParams[j];		
		if (DOMNode* replaceNode = getParamReplaceNode(myCommandXMLParser.getRootNode(), CommandObj.getCommandStr().c_str(), kBasics::trimQuotatedStr(partOfParam, kBasics::QUOTATION_SINGLE)))
		{					
			DOMNamedNodeMap *myAttributes = replaceNode->getAttributes();
			string XMLParamName = "";
			for (unsigned int i=0;i<myAttributes->getLength();i++)
			{
				DOMAttr *myAttributeNode = (DOMAttr*) myAttributes->item(i); 			
				if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getName())) == "PARAMSTR")
				{
					//XMLParamName = kBasics::toUp((string)XMLString::transcode(myAttributeNode->getValue()));
					XMLParamName = (string)XMLString::transcode(myAttributeNode->getValue());
				}
				if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getName())) == "VALUEPERTIMEUNIT")
				{
					valuePerTimeUnit = atof(((string)XMLString::transcode(myAttributeNode->getValue())).c_str());

					//re-calculate the CommandObj
					if (myIniObject.getIntegrity())
					{
						std::vector<string> splittedParam;
						kBasics::split(kBasics::trimQuotatedStr(partOfParam, kBasics::QUOTATION_SINGLE),DELIMITER_SPACE,2,&splittedParam);
						
						//Case A (1 Param, no valuePerTU (doesn't drop in here))
						//Case B (2 Params, 1st: value, 2nd: valuePerTU)
						if (splittedParam.size() == 2) //KEINE Sicherheitsabfrage ... ob es vielleicht auch was anderes als '90 slow' ist
						{							
							if (splittedParam[1] == XMLParamName)
							{
								partOfParam = splittedParam[0];								
								int newTimeEnd = (kBasics::StringToInt(splittedParam[0])/valuePerTimeUnit)+CommandObj.getTimeStart();								
								string newTimeString = string("[")+kBasics::IntToString(CommandObj.getTimeStart())+string(",")+kBasics::IntToString(newTimeEnd)+string("]");
								CommandObj.setTimeStr(newTimeString);
							}				
						}
						//Case C (1 Param, 1st: valuePerTU)						
						else
						{							
							if (splittedParam[0] == XMLParamName)
							{					
								partOfParam = kBasics::FloatToString((CommandObj.getTimeEnd()-CommandObj.getTimeStart())*valuePerTimeUnit);
							}
						}
					}
					else
					{
						cerr << "\nKonnte neuen Parameterwert nicht recalculaten ...\n\n";
					}			
				}
				else if (kBasics::toUp((string)XMLString::transcode(myAttributeNode->getName())) == "SINGLEVALUE")
				{					
					//singleValue = kBasics::toUp((string)XMLString::transcode(myAttributeNode->getValue()));
					singleValue = (string)XMLString::transcode(myAttributeNode->getValue());
					partOfParam = singleValue;
				}
			}

			//rekursiv nochmal versuchen ... hier kommt man nur rein, wenn was ersetzt wurde, also die Möglichkeit besteht, das nochmal ersetzt werden muss
			somethingChanged = true;
		}
		
		if (gesamtParam!="") gesamtParam = gesamtParam +" ";
		gesamtParam = gesamtParam + partOfParam;
	}

	CommandObj.setParamStr(gesamtParam);
	
	if (somethingChanged)
	{		
		checkForParamReplace(myIniObject,CommandObj);
	}
	return true;
}



//! Gibt Liste der ersetzenden CommandObjekte zurück
/*! Muss nichts ersetzt werden, wird eine 1-elementige Liste mit dem OriginalCommand zurückgegeben
 *  \param myIniObject
 *  \param CommandObj
 */
AnimationParser::COLIST AnimationParser::checkForCommandReplace(kScriptIniObject& myIniObject, kScriptCommandObject CommandObj)
{				
	COLIST CommandList;	
	if (DOMNode* replaceNode = getCommandReplaceNode(myCommandXMLParser.getRootNode(),CommandObj.getCommandStr(),1))
	{		
		COLIST replaceList = getCommandList(replaceNode);
		COLIST::const_iterator iter;
		for ( iter = replaceList.begin();iter!=replaceList.end(); iter++)
		{		
			kScriptCommandObject tempObj = kScriptCommandObject(*iter);
			if (kBasics::toUp(tempObj.getTimeStr())=="T")
				tempObj.setTimeStr(CommandObj.getTimeStr());
			if (kBasics::toUp(tempObj.getObjectName())=="O")
				tempObj.setObjectName(CommandObj.getObjectName());

			//Parameter splitten und einzeln durchgehen, ob O oder P Ersetzungen vorzunehmen sind				
			std::vector<string> tempEntryParams;			
			kBasics::splitQuotated(tempObj.getParamStr(),kScriptFrameEntry::DELIMITER_PARAMS,kBasics::QUOTATION_SINGLE,&tempEntryParams);

			//for each part
			string gesamtParam = "";
			string partOfParam;
			for (size_t j=0; j<tempEntryParams.size(); j++)
			{
				if (kBasics::toUp(tempEntryParams[j])=="O")
				{
					partOfParam = "'"+kBasics::trimQuotatedStr(CommandObj.getObjectName(), kBasics::QUOTATION_SINGLE)+"'";
				}
				else if (kBasics::toUp(tempEntryParams[j])=="P")
				{
					partOfParam = CommandObj.getParamStr();
				}
				else
				{
					partOfParam = tempEntryParams[j];					
				}
								
				if (gesamtParam!="") gesamtParam = gesamtParam +" ";				
				gesamtParam = gesamtParam + partOfParam;								
			}
			
			tempObj.setParamStr(gesamtParam);					

			//Trying to replace the command recursive			
			COLIST CommandListFinal = checkForCommandReplace(myIniObject,tempObj);						

			//Try to replace the Params ofthe final CommandList
			COLIST::const_iterator iterFinal;
			for (iterFinal = CommandListFinal.begin(); iterFinal!=CommandListFinal.end(); iterFinal++)
			{
				checkForParamReplace(myIniObject,tempObj);				 
				CommandList.push_back(kScriptCommandObject(*iterFinal));
			}
		}						
	}
	else
	{
		checkForParamReplace(myIniObject, CommandObj);
		CommandList.push_back(CommandObj);
	}
	return CommandList;
}



//! Gibt Liste der ersetzenden CommandObjekte zurück
/*! Entweder bei mehreren Objekten die duplizierten Commands oder bei nur einer Objektersetzung (oder keiner) das OriginalCommando mit dem Objekt
 *  \param CommandObj
*/
AnimationParser::COLIST AnimationParser::checkForObjectReplace(kScriptCommandObject CommandObj)
{			
	COLIST CommandList;	
	std::vector<DOMNode*> replaceNodes;	
	DOMNode* tempNode = getObjectReplaceNode(myObjectXMLParser1.getRootNode(),CommandObj.getObjectName());
	replaceNodes.push_back(tempNode);
	if (strlen(ObjectXMLFileName2)!=0)
	{
		DOMNode* tempNode2 = getObjectReplaceNode(myObjectXMLParser2.getRootNode(),CommandObj.getObjectName());
		replaceNodes.push_back(tempNode2);
	}
	
	bool objReplaced = false;
	for (unsigned int i=0; i<replaceNodes.size(); i++)
	{	
		DOMNode* replaceNode = replaceNodes[i];
		if (replaceNode && !objReplaced)
		{		
			list<string> replaceList = getObjectList(replaceNode);
			list<string>::const_iterator iter;
			for ( iter = replaceList.begin();iter!=replaceList.end(); iter++)
			{						
				string tempName = string(*iter);
				kScriptCommandObject tempObj = CommandObj;
				tempObj.setObjectName(tempName);
				
				COLIST CommandList2 = checkForObjectReplace(tempObj);
				COLIST::const_iterator iter2;				
					
				for (iter2 = CommandList2.begin(); iter2!=CommandList2.end(); iter2++)
				{				
					CommandList.push_back(kScriptCommandObject(*iter2));
				}
			}						
			objReplaced = true;
		}
	}
	if (!objReplaced)
	{
		CommandList.push_back(CommandObj);
	}
	return CommandList;
}



//! Parst ein HighLevel-Script in ein LowLevel-Script
/*! \param streamHL */
/*! \param streamLL */
string AnimationParser::parseHLScript(iostream& streamHL, ostream& streamLL)
{
	char l[256];
	short SectionType = ST_NONE;
	kScriptIniObject myIniObject;
		
	while (streamHL.getline(l,256)) //256=maxAnz Zeichen
	{
		string line(l);
		/////////line = kBasics::toUp(line);
		line = removeComments(line);

		if (kBasics::toUp(line).find("[INI]",0)!=string::npos)
		{
			SectionType=ST_INI;
			streamLL << line << endl;
		}
		else if (kBasics::toUp(line).find("[SCRIPT]",0)!=string::npos)
		{
			SectionType=ST_SCRIPT;
			streamLL << line << endl;
		}
		else if (SectionType==ST_INI)
		{
			streamLL << line << endl;			
			myIniObject.parseStr(line);
		}
		else if (SectionType==ST_SCRIPT)
		{	
			if (!myIniObject.getIntegrity())
			{				
				return ERROR_INI_HIGHLEVEL;
			}

			kScriptCommandObject CommandObj;
			if (CommandObj.setCommandLine(line))
			{				
				COLIST CommandList = checkForCommandReplace(myIniObject,CommandObj);								
				COLIST::const_iterator iter;
				for (iter = CommandList.begin(); iter!=CommandList.end(); iter++)
				{
					COLIST CommandList2 = checkForObjectReplace(kScriptCommandObject(*iter));
					COLIST::const_iterator iter2;				
				
					for (iter2= CommandList2.begin(); iter2!=CommandList2.end(); iter2++)
					{		
						string outLine = kScriptCommandObject(*iter2).getTimeStr() +
						    			 " '" + kScriptCommandObject(*iter2).getObjectName() + "'" +
										 " " + kScriptCommandObject(*iter2).getCommandStr() +
										 " " + kScriptCommandObject(*iter2).getParamStr() +
										 "\n";												
						streamLL << outLine;				
					}	
				}
			}
			else
			{				
				//Zeile konnte nicht gelesen werden - error				
			}
		}
	}
	return ERROR_NONE;
}



//! Parst ein LowLevel-Script in eine CommandList und ein IniObject
/*! \param fileLL
 *  \param iniObject
 *  \param LLCommandList
 */
string AnimationParser::parseLLScript(iostream& streamLL, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList)
{
	char l[256];
	short SectionType = ST_NONE;
		
	while (streamLL.getline(l,256)) //256=maxAnz Zeichen
	{
		string line(l);
		///////////////line = kBasics::toUp(line);		

		if (kBasics::toUp(line).find("[INI]",0)!=string::npos)
		{
			SectionType=ST_INI;			
		}
		else if (kBasics::toUp(line).find("[SCRIPT]",0)!=string::npos)
		{
			SectionType=ST_SCRIPT;			
		}
		else if (SectionType==ST_INI)
		{			
			iniObject->parseStr(line);
		}
		else if (SectionType==ST_SCRIPT)
		{	
			if (!iniObject->getIntegrity())
			{				
				return ERROR_INI_LOWLEVEL;
			}

			kScriptCommandObject tempCommandObj(line); 
			LLCommandList->push_back(tempCommandObj);
		}
	}
	return ERROR_NONE;
}


string AnimationParser::removeComments(string line)
{
	std::vector<string> splittedLine;
	kBasics::split(line, '#',255,&splittedLine);
	string newline = "";
	for (unsigned int i=0;i<splittedLine.size();i++)
	{									
		if (i%2==0)
		{
			newline = newline + splittedLine[i];
		}
	}
	return newline;
}


//! Setzt für das Parser-Objekt das CommandXML-File
/*! \param value */
string AnimationParser::setCommandXMLFileName(const char* value)
{	
	if (kBasics::fileExists(value))
	{
		CommandXMLFileName = value;
		myCommandXMLParser.parseXMLFile(CommandXMLFileName);
		return ERROR_NONE;
	}
	return ERROR_OPEN_COMMANDXML;
}


//! Returns filename of command file
string AnimationParser::getCommandXMLFileName()
{
	return kBasics::CharToString(CommandXMLFileName);
}




//! Setzt für das Parser-Objekt die ObjectXML-Files<br>
//! Es sind zwei - Ein File für die (möglicherweise) automatisch generierte ObjectNames.xml und ein File für fallunabhängige Bezeichnungen wie Objektanweisungen
/*! \param value */
string AnimationParser::setObjectXML1FileName(const char* file1)
{
	if (kBasics::fileExists(file1))
	{
		ObjectXMLFileName1 = file1;
		myObjectXMLParser1.parseXMLFile(ObjectXMLFileName1);
	}
	else return ERROR_OPEN_OBJECTXML1;
	
	return ERROR_NONE;
}


//! Returns filename of objects xml file
string AnimationParser::getObjectXML1FileName()
{
	return kBasics::CharToString(ObjectXMLFileName1);
}



string AnimationParser::setObjectXML1String(const string xmlString)
{
	ObjectXMLFileName1 = "";	
	ObjectXMLString1 = xmlString;
	int tempError = myObjectXMLParser1.parseXMLString(&xmlString);	
	if (tempError!=0) return kBasics::IntToString(tempError);
	
	return "";
}


string AnimationParser::getObjectXML1String()
{
	return ObjectXMLString1;
}




string AnimationParser::setObjectXML2FileName(const char* file2)
{	
	if (strlen(file2)!=0)
	{
		if (kBasics::fileExists(file2))
		{
			ObjectXMLFileName2 = file2;
			myObjectXMLParser2.parseXMLFile(ObjectXMLFileName2);
		}
		else return ERROR_OPEN_OBJECTXML2;
	}
	else ObjectXMLFileName2 = "";
	return ERROR_NONE;
}


string AnimationParser::getObjectXML2FileName()
{	
	return kBasics::CharToString(ObjectXMLFileName2);
}


//! Öffnet ein HighLevel-Script und veranlasst das Parsen
/*! \param HLFileName
 *  \param LLFileName
 */
string AnimationParser::openAndParseHLScript(const char* HLFileName, const char* LLFileName)
{
	fstream fileHL(HLFileName);	
	ofstream fileLL(LLFileName); 	
	
	if (!fileHL)
	{
		return ERROR_OPEN_HIGHLEVEL + " Filename: " + HLFileName;
	}
	if (!fileLL)
	{		
		return ERROR_CREATE_LOWLEVEL;
	}

	fileLL.clear();	
	
	//stringstream tempHL;
	//((istream)fileHL) >> tempHL;

	return parseHLScript(fileHL,fileLL);;
}


string AnimationParser::openAndParseHLStream(stringstream& HLScript, ostream& LLScript)
{		
	//std::stringstream HLStream; // Dieser Handstand ist nötig da das direkte Übergeben von Stringstreams in keinster Weise funktioniert hat
	//HLStream << HLScript;
	return parseHLScript(HLScript,LLScript);		
}


//! Öffnet ein LowLevel-Script und veranlasst die Umwandlung
/*! Gibt Zeiger IniObject und CommandListe zurück
 *  \param LLFileName
 *  \param iniObject
 *  \param LLCommandList
 */
string AnimationParser::openAndParseLLScript(const char* LLFileName, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList)
{
	fstream fileLL(LLFileName);		
	
	if (!fileLL)
	{
		cerr << "Error opening lowlevel file / file not found?\n";
		return ERROR_OPEN_LOWLEVEL;
	}
	
	return parseLLScript(fileLL, iniObject, LLCommandList); 
}


string AnimationParser::openAndParseLLStream(stringstream& LLScript, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList)
{
	//std::stringstream LLStream;
	//LLStream << LLScript;
	return parseLLScript(LLScript, iniObject, LLCommandList); 
}



//! Gibt die Ersetzungen für eine Objekt-Bezeichnung zurück
/*! \param objectName
 *  \param returnStringList
 */
void AnimationParser::getObjectStringList(const char* objectName, std::vector<string>* returnStringList)
{
	COLIST tempCommandList;
	kScriptCommandObject tempCommandObj;
	tempCommandObj.setObjectName(objectName);
	tempCommandList = checkForObjectReplace(tempCommandObj);

	COLIST::const_iterator iter;
	kScriptCommandObject tempObj;
	for ( iter = tempCommandList.begin();iter!=tempCommandList.end(); iter++)
	{
		tempObj = kScriptCommandObject(*iter);
		returnStringList->push_back(tempObj.getObjectName());
	}	
}

//! Gibt die Ersetzungen für eine Objekt-Bezeichnung zurück (HashTable)
/*! \param objectName
 *  \param returnStringList
 */
void AnimationParser::getObjectStringList(const char* objectName, HashTable<string>* returnStringList)
{
	COLIST tempCommandList;
	kScriptCommandObject tempCommandObj;
	tempCommandObj.setObjectName(objectName);
	tempCommandList = checkForObjectReplace(tempCommandObj);

	COLIST::const_iterator iter;
	kScriptCommandObject tempObj;
	for ( iter = tempCommandList.begin();iter!=tempCommandList.end(); iter++)
	{		
		tempObj = kScriptCommandObject(*iter);
		returnStringList->insert(tempObj.getObjectName(),tempObj.getObjectName());
	}	
}



//! Fügt Objekt-Bezeichnung mit dessen Ersetzungvorschriften zu einem ObjectXML-File hinzu
/*! \param fileName
 *  \param orgObject
 *  \param replaceObjectList
 */
void AnimationParser::addObjectToXMLFile(const string fileName, const string orgObject, const list<string>* replaceObjectList)
{
    XercesDOMParser* parser = new XercesDOMParser;
    parser->parse(fileName.c_str());
    
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Range"));
	DOMDocument* doc = parser->getDocument();
	DOMElement* root = doc->getDocumentElement();


	DOMElement* objNode = doc->createElement(XMLString::transcode("OBJECT"));
	objNode->setAttribute(XMLString::transcode("name"),XMLString::transcode(orgObject.c_str()));
	
	list<string>::const_iterator iter;
	for ( iter = replaceObjectList->begin(); iter!=replaceObjectList->end(); iter++)
	{				
		DOMElement* tempNode = doc->createElement(XMLString::transcode("OBJECT"));
		tempNode->appendChild(doc->createTextNode(XMLString::transcode(string(*iter).c_str())));
		objNode->appendChild(tempNode);		
	}

	root->appendChild(objNode);

	writeDocToXML(doc,impl,fileName);

    delete parser;
}


//HIER MÜSSTE MAL ALLES MIT TRY CATCH BLÖCKEN ABGEFANGEN WERDE!!!
void AnimationParser::addObjectToXMLString(string* xmlString, const string orgObject, const list<string>* replaceObjectList)
{
	MemBufInputSource *isource = new MemBufInputSource( (const XMLByte*)xmlString->c_str(), strlen(xmlString->c_str()), "XMLBuffer", false);
	if (isource == NULL) return;

    XercesDOMParser* parser = new XercesDOMParser;
    parser->parse(*isource);
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Range"));
	DOMDocument* doc = 0;
	
	try
	{
		doc = parser->getDocument();
	}  
	catch (const XMLException& toCatch)
	{  
		char* message = XMLString::transcode(toCatch.getMessage());  
		cout << "Exception message is: \n" << message << "\n";  
		XMLString::release(&message);  
		return;  
	}
	
	DOMElement* root =  0;
	
	try
	{  
		root = doc->getDocumentElement();
	}  
    catch (const XMLException& toCatch)
	{  
		char* message = XMLString::transcode(toCatch.getMessage());  
		cout << "Exception message is: \n" << message << "\n";  
		XMLString::release(&message);  
		return;  
	}  	
		

	if (root==NULL)
	{
		cout << "root==NULL!!!!" << std::endl;
		return;
	}	

	DOMElement* objNode = doc->createElement(XMLString::transcode("OBJECT"));
	objNode->setAttribute(XMLString::transcode("name"),XMLString::transcode(orgObject.c_str()));	
	list<string>::const_iterator iter;
	for ( iter = replaceObjectList->begin(); iter!=replaceObjectList->end(); iter++)
	{				
		DOMElement* tempNode = doc->createElement(XMLString::transcode("OBJECT"));		
		tempNode->appendChild(doc->createTextNode(XMLString::transcode(string(*iter).c_str())));		
		objNode->appendChild(tempNode);		
	}	
	root->appendChild(objNode);	
	writeDocToXMLString(doc,impl,xmlString);

    delete parser;
}




//-----------------------------------------------------------
//Funktionen zum Erzeugen einer neuen ObjectXML-Datei
//-----------------------------------------------------------

//! Erzeugt neues ObjectXML-File
/*! \param fileName */
void AnimationParser::createNewObjectXMLFile(const char* fileName)
{
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Range"));
	DOMDocument* doc = impl->createDocument(0, XMLString::transcode("ObjectNames"), 0);
	//DOMElement* root = doc->getDocumentElement();
	writeDocToXML(doc,impl,fileName);
	doc->release();
}



void AnimationParser::createNewObjectXMLString(string* xmlString)
{
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Range"));
	DOMDocument* doc = impl->createDocument(0, XMLString::transcode("ObjectNames"), 0);
	//DOMElement* root = doc->getDocumentElement();
	writeDocToXMLString(doc,impl,xmlString);		
	doc->release();
}



//! Erzeugt neues CommandXML-File
void AnimationParser::createNewCommandXMLFile(const char* fileName)
{
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Range"));
	DOMDocument* doc = impl->createDocument(0, XMLString::transcode("HighLevel"), 0);
	//DOMElement* root = doc->getDocumentElement();
	writeDocToXML(doc,impl,fileName);		
	doc->release();
}


void AnimationParser::createNewCommandXMLString(string* xmlString)
{
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("Range"));
	DOMDocument* doc = impl->createDocument(0, XMLString::transcode("HighLevel"), 0);
	//DOMElement* root = doc->getDocumentElement();
	writeDocToXMLString(doc,impl,xmlString);		
	doc->release();
}


//! Schreibt eine XMLDOC-Struktur in eine Datei
/*! \param doc
 *  \param impl
 *  \param fileName
 */
void AnimationParser::writeDocToXML(const DOMDocument* doc, const DOMImplementation* impl, const string fileName)
{
	DOMWriter* theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, true))
		theSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, true);

	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
		theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

	XMLFormatTarget *myFormTarget;
	myFormTarget = new LocalFileFormatTarget(fileName.c_str());

	theSerializer->writeNode(myFormTarget, *doc);

	delete theSerializer;
	delete myFormTarget;
}



void AnimationParser::writeDocToXMLString(const DOMDocument* doc, const DOMImplementation* impl, string* xmlString)
{
	DOMWriter* theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTSplitCdataSections, true))
		theSerializer->setFeature(XMLUni::fgDOMWRTSplitCdataSections, true);

	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
		theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

	if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

	//XMLFormatTarget *myFormTarget;
	//myFormTarget = new LocalFileFormatTarget(fileName.c_str());

	//theSerializer->writeNode(myFormTarget, *doc);
	XMLCh* resultString = theSerializer->writeToString(*doc);
	//std::cout << "Result:" << XMLCh2String(resultString) << std::endl;
	*xmlString = XMLCh2String(resultString);
	//std::cout << "xmlString:" << *xmlString << std::endl;


	delete theSerializer;
	//delete myFormTarget;
}





string AnimationParser::XMLCh2String(const XMLCh * xString)
{
	if (xString)
	{
		// Convert string.
		char * s =  xercesc::XMLString::transcode(xString);
		std::string retval(s);

		// Cleanup
		xercesc::XMLString::release(&s);      

		return retval;
	}

	else
	{	
		return std::string("");
	}    
}
