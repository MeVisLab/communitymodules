//! Classes XMLParser and XMLParserErrorHandler based on Xerces.
//! \file XMLParser.h

#ifndef _XMLParser_
#define _XMLParser_

#pragma warning(disable:4251)
#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport)
#endif
#else
#define __COMMON_IMPORTEXPORT
#endif

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <iostream>
#include <fstream>
#include <string>

XERCES_CPP_NAMESPACE_USE


//!  Simple error handler deriviative to install on parser
class __COMMON_IMPORTEXPORT XMLParserErrorHandler : public DOMErrorHandler
{
public:
  XMLParserErrorHandler();
  ~XMLParserErrorHandler();

  bool getSawErrors() const;
  bool handleError(const DOMError& domError);
  void resetErrors();

private :
  XMLParserErrorHandler(const XMLParserErrorHandler&);
  void operator=(const XMLParserErrorHandler&);
  bool    fSawErrors; //This is set if we get any errors
};



//! An XMLParser class based on Xerces.
class __COMMON_IMPORTEXPORT XMLParser
{
private:
  DOMImplementation *impl;
  DOMBuilder        *parser;
  XERCES_CPP_NAMESPACE::DOMDocument* xml_doc;
  DOMNode* rootNode;

public:
  XMLParser() { }
  ~XMLParser();


  DOMNode* getRootNode() { return rootNode; }

  int parseXMLFile(const char*);
  int parseXMLString(const std::string* xmlString);

  int countChildElements(DOMNode *n);
};


#endif // _XMLParser_
