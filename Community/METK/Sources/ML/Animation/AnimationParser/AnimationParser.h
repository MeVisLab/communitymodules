//----------------------------------------------------------------------------------
/*!
// \file    AnimationParser.h
// \author  Konrad Mühler
// \date    2005-07-05
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef _ANIMATIONPARSERDLL_
#define _ANIMATIONPARSERDLL_

// Local includes
#include "AnimationSystem.h"

#include <list>
#include <vector>

#include "kBasics.h"
#include "kScriptCommandObject.h"
#include "kScriptIniObject.h"
#include "kScriptFrameList.h"

#include "XMLParser.h"
#include "xercesc\dom\DOMDocument.hpp"
#include "xercesc\dom\DOMNode.hpp"
#include "xercesc\dom\DOMElement.hpp"
#include "xercesc\dom\DOMText.hpp"
#include "xercesc\dom\DOMRange.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/parsers/DOMBuilderImpl.hpp>


using namespace std;


//!  AnimationParser is the class for script parsing. 
/*!  It contains all functions for parsing a script using XML files for command- and object-replacement
 *
 *   The Library is for script parser and frame management
 */
class ANIMATION_EXPORT AnimationParser
{
private:

	//! \defgroup SectionType SectionTypes
	//@{		
		static const short ST_NONE;		//!< Unknown SectionType
		static const short ST_INI;		//!< Ini-Part	
		static const short ST_SCRIPT;	//!< Script-Part
	//@}

	static const char DELIMITER_SPACE; //!< Delimiter for parts of commands
		
	typedef std::list<kScriptCommandObject> COLIST; //!< Typdef for Lists of CommandObjects
	
	XMLParser myObjectXMLParser1;	//!< Parser for first ObjectXML-File
	XMLParser myObjectXMLParser2;	//!< Parser for second ObjectXML-File	
	XMLParser myCommandXMLParser;	//!< Parser for CommandXML-File
	
	const char* CommandXMLFileName; //!< FileName of CommandXML-File	
	const char* ObjectXMLFileName1; //!< FileName of first ObjectXML-File
	string ObjectXMLString1;		//!< String of first ObjectXML
	const char* ObjectXMLFileName2; //!< FileName of second ObjectXML-File


	//--------------------------------------------------------
	//Functions
	//--------------------------------------------------------
	int getReplaceCommandCount(DOMNode* node);
	DOMNode* getCommandReplaceNode(DOMNode* root, string CommandStr, int minCommandCount);
	DOMNode* getObjectReplaceNode(DOMNode* root, string ObjectStr);
	DOMNode* getParamReplaceNode(DOMNode* root, string CommandStr, string ParamStr);
	COLIST getCommandList(DOMNode* node);
	list<string> getObjectList(DOMNode* node);
	bool checkForParamReplace(kScriptIniObject& myIniObject, kScriptCommandObject& CommandObj);
	COLIST checkForCommandReplace(kScriptIniObject& myIniObject, kScriptCommandObject CommandObj);
	COLIST checkForObjectReplace(kScriptCommandObject CommandObj);
	string parseHLScript(iostream& streamHL, ostream& streamLL);
	string parseLLScript(iostream& streamLL, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList);
	void writeDocToXML(const XERCES_CPP_NAMESPACE::DOMDocument* doc, const DOMImplementation* impl, const string fileName);
	void writeDocToXMLString(const XERCES_CPP_NAMESPACE::DOMDocument* doc, const DOMImplementation* impl, string* xmlString);
	string removeComments(string line);	
	string XMLCh2String(const XMLCh * xString);

protected:
	virtual ~AnimationParser();

public:
	//--------------------------------------------------------
	//Functions
	//--------------------------------------------------------
	AnimationParser();	
	string setCommandXMLFileName(const char* value);
	string getCommandXMLFileName();
	string setObjectXML1FileName(const char* file1);
	string getObjectXML1FileName();
	string setObjectXML1String(string xmlString);
	string getObjectXML1String();
	string setObjectXML2FileName(const char* file2);
	string getObjectXML2FileName();

	string openAndParseLLScript(const char* LLFileName, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList);
	string openAndParseLLStream(stringstream& LLScript, kScriptIniObject* iniObject, list<kScriptCommandObject>* LLCommandList);
	string openAndParseHLScript(const char* HLFileName, const char* LLFileName);
	string openAndParseHLStream(stringstream& HLScript, ostream& LLScript);
	
	void getObjectStringList(const char* objectName, std::vector<string>* returnStringList);	
	void getObjectStringList(const char* objectName, HashTable<string>* returnStringList);
	void createNewObjectXMLFile(const char* fileName);
	void createNewObjectXMLString(string* xmlString);
	void createNewCommandXMLFile(const char* fileName);
	void createNewCommandXMLString(string* xmlString);
	void addObjectToXMLFile(const string fileName, const string orgObject, const list<string>* replaceObjectList);
	void addObjectToXMLString(string* xmlString, const string orgObject, const list<string>* replaceObjectList);

	string parseObjectCommand(const char *objectLine, std::vector<string>* returnStringList);

	//! \defgroup Errors
	//@{		
		static const string ERROR_NONE;				//!< No error, all ok
		static const string ERROR_OPEN_LOWLEVEL;	//!< Error opening lowlevel file / file not found?
		static const string ERROR_OPEN_HIGHLEVEL;	//!< Error opening highlevel file / file not found?
		static const string ERROR_OPEN_OBJECTXML1;	//!< Error opening object xml file 1 / file not found?
		static const string ERROR_OPEN_OBJECTXML2;	//!< Error opening object xml file 2 / file not found?
		static const string ERROR_OPEN_COMMANDXML;	//!< Error opening command xml file / file not found?
		static const string ERROR_CREATE_LOWLEVEL;	//!< Error create lowlevel file!
		static const string ERROR_INI_HIGHLEVEL;	//!< Error in ini part of high level script!
		static const string ERROR_INI_LOWLEVEL;		//!< Error in ini part of low level script!	
	//@}
};

#endif // _ANIMATIONPARSERDLL_