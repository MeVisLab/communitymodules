//----------------------------------------------------------------------------------
// The ML module class SavePDF.
//
// \file    PDFGenerator_XML.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2016-08-02
//
// Base class for PDF creators.
//----------------------------------------------------------------------------------


// ThirdParty includes
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

// Local includes
#include "PDFGenerator.h"


using namespace xercesc;


ML_START_NAMESPACE


//----------------------------------------------------------------------------------

void PDFGenerator::_createDOMParser()
{
  if (!_xmlParser)
  {
    try
    {
      XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& e)
    {
      char* message = XMLString::transcode(e.getMessage());
      std::cout << "Error during initialization of XML parser! :" << std::endl << message << std::endl;
      XMLString::release(&message);
    }

    _xmlErrorHandler = (ErrorHandler*) new HandlerBase();
    _xmlParser = new XercesDOMParser();

    if (_xmlParser && _xmlErrorHandler)
    {
      _xmlParser->setValidationScheme(XercesDOMParser::Val_Never/*::Val_Always*/);
      _xmlParser->setDoNamespaces(false);
      _xmlParser->setDoSchema(false);
      _xmlParser->setLoadExternalDTD(false);
      _xmlParser->setExitOnFirstFatalError(false);

      _xmlParser->setErrorHandler(_xmlErrorHandler);
    }
    else
    {
      assemblyErrorMessage = "XML parser could not be initialized.";
    }
  }
}

//----------------------------------------------------------------------------------

void PDFGenerator::_destroyDOMParser()
{
  ML_DELETE(_xmlParser);
  ML_DELETE(_xmlErrorHandler);
}

//----------------------------------------------------------------------------------

std::string PDFGenerator::_getValidXMLString(std::string inputXMLString)
{
  std::string result = "";

  if (inputXMLString != "")
  {
    result = "<root>" + inputXMLString + "</root>";

  }

  return result;
}

//----------------------------------------------------------------------------------

DOMElement* PDFGenerator::_parseXMLString(std::string xmlString)
{
  DOMElement* xmlRootElement = NULL;

  _createDOMParser();

  if ( (_xmlParser) && (xmlString != "") )
  {
    xercesc::DOMDocument* xmlDoc = NULL;

    try
    {
      std::string validXMLString = _getValidXMLString(xmlString);

      xercesc::MemBufInputSource stringSource((const XMLByte*)validXMLString.c_str(), (const unsigned int)validXMLString.length(), "dummy", false);
      _xmlParser->parse(stringSource);

      // No need to free this pointer - owned by the parent parser object
      xmlDoc = _xmlParser->getDocument();

      // Get the top-level element
      xmlRootElement = xmlDoc->getDocumentElement();
    }
    catch (const XMLException& e)
    {
      char* message = XMLString::transcode(e.getMessage());
      assemblyErrorMessage = "XML parser XMLException: " + std::string(message);
      XMLString::release(&message);
      xmlRootElement = NULL;
    }
    catch (const DOMException& e)
    {
      char* message = XMLString::transcode(e.msg);
      assemblyErrorMessage = "XML parser DOMException: " + std::string(message);
      XMLString::release(&message);
      xmlRootElement = NULL;
    }
    catch (const std::exception& e)
    {
      assemblyErrorMessage = "XML parser exception: " + std::string(e.what());
      xmlRootElement = NULL;
    }
    catch (const std::string& e) {
      assemblyErrorMessage = "XML parser exception: " + e;
      xmlRootElement = NULL;
    }
    catch (...)
    {
      assemblyErrorMessage = "Unexpected XML parser exception.";
      xmlRootElement = NULL;
    }

  }

  return xmlRootElement;
}

//----------------------------------------------------------------------------------

std::vector<DOMElement*> PDFGenerator::_getAllNodeElements(DOMElement* parentElement, const char* nodeName, const char* attributeName, const char* attributeValue)
{
  std::vector<DOMElement*> result;

  if (parentElement)
  {
    XMLCh* xmlNodeName = XMLString::transcode(nodeName);
    DOMNodeList* nodeList = parentElement->getElementsByTagName(xmlNodeName);
    XMLString::release(&xmlNodeName);

    size_t nodeListLength = nodeList->getLength();

    for (int i = 0; i < nodeListLength; i++)
    {
      DOMElement* thisNodeElement = dynamic_cast<DOMElement*>(nodeList->item(i));
      std::string thisNodeAttributeValue = _getAttributeValue(thisNodeElement, attributeName);

      if (attributeName)
      {
        if (attributeValue)
        {
          if (thisNodeAttributeValue == attributeValue)
          {
            result.push_back(thisNodeElement);
          }
        }
        else
        {
          if (thisNodeAttributeValue != "") // This means that attribute must be present
          {
            result.push_back(thisNodeElement);
          }
        }
      } // if (attributeName)
      else
      {
        result.push_back(thisNodeElement); // Simply take all node elements
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------------------

std::vector<DOMElement*> PDFGenerator::_getAllNodeElements(DOMElement* parentElement, std::string nodeName, std::string attributeName, std::string attributeValue)
{
  const char* attributeNameChar = NULL;
  const char* attributeValueChar = NULL;

  if (attributeName != "")
  {
    attributeNameChar = attributeName.c_str();
  }

  if (attributeValue != "")
  {
    attributeValueChar = attributeValue.c_str();
  }

  return _getAllNodeElements(parentElement, nodeName.c_str(), attributeNameChar, attributeValueChar);
}

//----------------------------------------------------------------------------------

DOMElement* PDFGenerator::_getNodeElement(DOMElement* parentElement, const char* nodeName, const char* attributeName, const char* attributeValue)
{
  DOMElement* result = NULL;

  if (parentElement)
  {

    XMLCh* xmlNodeName = XMLString::transcode(nodeName);
    DOMNodeList* nodeList = parentElement->getElementsByTagName(xmlNodeName);
    XMLString::release(&xmlNodeName);

    size_t nodeListLength = nodeList->getLength();

    if (nodeListLength > 0)
    {
      if (attributeName)
      {
        for (int i = 0; i < nodeListLength; i++)
        {
          DOMElement* thisNodeElement = dynamic_cast<DOMElement*>(nodeList->item(i));
          std::string thisNodeAttributeValue = _getAttributeValue(thisNodeElement, attributeName);

          if (attributeValue)
          {
            if (thisNodeAttributeValue == attributeValue)
            {
              result = thisNodeElement;
              break;
            }
          }
          else
          {
            if (thisNodeAttributeValue != "")
            {
              result = thisNodeElement;
              break;
            }
          }
        }
      }
      else  // Simply take first found node
      {
        result = dynamic_cast<DOMElement*>(nodeList->item(0));
      }

    }

  }

  return result;
}

//----------------------------------------------------------------------------------

DOMElement* PDFGenerator::_getNodeElement(DOMElement* parentElement, std::string nodeName, std::string attributeName, std::string attributeValue)
{
  const char* attributeNameChar = NULL;
  const char* attributeValueChar = NULL;

  if (attributeName != "")
  {
    attributeNameChar = attributeName.c_str();
  }

  if (attributeValue != "")
  {
    attributeValueChar = attributeValue.c_str();
  }

  return _getNodeElement(parentElement, nodeName.c_str(), attributeNameChar, attributeValueChar);
}

//----------------------------------------------------------------------------------

std::string PDFGenerator::_getAttributeValue(DOMElement* element, const char* attributeName)
{
  std::string result = "";

  if (element)
  {
    XMLCh* xmlAttributeName = XMLString::transcode(attributeName);
    char* tempResult = XMLString::transcode(element->getAttribute(xmlAttributeName));
    result = tempResult;
    XMLString::release(&tempResult);
  }

  return result;
}

//----------------------------------------------------------------------------------

std::string PDFGenerator::_getAttributeValue(DOMElement* element, std::string attributeName)
{
  return _getAttributeValue(element, attributeName.c_str());
}

//----------------------------------------------------------------------------------

std::string PDFGenerator::_getInnerText(DOMElement* element)
{
  std::string result = "";

  if (element)
  {
    DOMNodeList* children = element->getChildNodes();
    int numChildren = children->getLength();

    for (int i = 0; i < numChildren; i++)
    {
      std::string tempResult = "";

      DOMNode* thisChildNode = children->item(i);

      DOMText* thisChildText = dynamic_cast<DOMText*>(thisChildNode);

      if (thisChildText)
      {
        char* tempXMLString1 = XMLString::transcode(thisChildText->getTextContent());
        tempResult = tempXMLString1;
        XMLString::release(&tempXMLString1);
      }
      else
      {
        DOMElement* thisChildElement = dynamic_cast<DOMElement*>(thisChildNode);

        if (thisChildElement)
        {
          const XMLCh* thisChildTagName = thisChildElement->getTagName();

          if (thisChildTagName)
          {
            char* tempXMLString2 = XMLString::transcode(thisChildTagName);

            if (strcmp(tempXMLString2, "br") == 0)
            {
              tempResult = "\n";
            }

            XMLString::release(&tempXMLString2);
          }

          // Recursively call function to parse inner tags as well!
          tempResult += _getInnerText(thisChildElement);
        }

      }

      result += tempResult;
    }

    /*
    char* tempResult = XMLString::transcode(element->getTextContent());
    result = tempResult;
    XMLString::release(&tempResult);
    */
  }

  return result;
}

//----------------------------------------------------------------------------------


ML_END_NAMESPACE
