#include "stdafx.h"
#include "XMLParser.h"


XMLParserErrorHandler::XMLParserErrorHandler() : fSawErrors(false)
{
}

XMLParserErrorHandler::~XMLParserErrorHandler()
{
}


bool XMLParserErrorHandler::getSawErrors() const
{
    return fSawErrors;
}


bool XMLParserErrorHandler::handleError(const DOMError& domError)
{
    fSawErrors = true;
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        std::cerr << "\nWarning at file ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        std::cerr << "\nError at file ";
    else
        std::cerr << "\nFatal Error at file ";

    std::cerr << domError.getLocation()->getURI()
		<< ", line " << domError.getLocation()->getLineNumber()
		<< ", char " << domError.getLocation()->getColumnNumber()
		<< "\n  Message: " << domError.getMessage() << std::endl;

    return true;
}


void XMLParserErrorHandler::resetErrors()
{
    fSawErrors = false;
}


int XMLParser::parseXMLFile(const char* XMLFileName)
{
	bool errorOccurred = false;

	// Initialize the XML4C system
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		std::cerr << "Error during initialization! :\n" << toCatch.getMessage() << std::endl;
		return 1;
	}


	// Instantiate the DOM parser.
	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };

	impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);


	parser->setFeature(XMLUni::fgDOMNamespaces, false);
	parser->setFeature(XMLUni::fgXercesSchema, false);
	parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);

	// enable datatype normalization - default is off
	parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

	// And create our error handler and install it
	XMLParserErrorHandler errorHandler;
	parser->setErrorHandler(&errorHandler);


	bool more = true;
	std::ifstream file_input;

	file_input.open(XMLFileName);

	if (file_input.fail())
	{
		std::cerr <<"Cannot open the file: " << XMLFileName << std::endl;
		return 2;
	}

	while (more)
	{
		char fURI[1000];
		//initialize the array to zeros
		memset(fURI,0,sizeof(fURI));

		more = false;

		//reset error count first
		errorHandler.resetErrors();

		try
		{
			// reset document pool
			parser->resetDocumentPool();


			xml_doc = parser->parseURI(XMLFileName);

		}
		catch (const XMLException& toCatch)
		{
			std::cerr << "\nError during parsing: '" << XMLFileName << "'\n"
				<< "Exception message is:  \n" << toCatch.getMessage() << "\n" << std::endl;
			errorOccurred = true;
			continue;
		}
		catch (const DOMException& toCatch)
		{
			const unsigned int maxChars = 2047;
			XMLCh errText[maxChars + 1];

			std::cerr << "\nDOM Error during parsing: '" << XMLFileName << "'\n"
				<< "DOMException code is:  " << toCatch.code << std::endl;

			if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
				std::cerr << "Message is: " << errText << std::endl;

			errorOccurred = true;
			continue;
		}
		catch (...)
		{
			std::cerr << "\nUnexpected exception during parsing: '" << XMLFileName << "'\n";
			errorOccurred = true;
			continue;
		}


		//  Extract the DOM tree, get the list of all the elements and report the length as the count of elements.
		if (errorHandler.getSawErrors())
		{
			std::cerr << "\nErrors occurred, no output available\n" << std::endl;
			errorOccurred = true;
		}
		else
		{
			//unsigned int elementCount = 0;
			if (xml_doc)
			{
				rootNode =(DOMNode*)xml_doc->getDocumentElement();
			}
		}
	}

	file_input.close();

	if (errorOccurred) return 4;
	else return 0;
}



int XMLParser::parseXMLString(const std::string* xmlString)
{
	bool errorOccurred = false;

	// Initialize the XML4C system
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch)
	{
		std::cerr << "Error during initialization! :\n" << toCatch.getMessage() << std::endl;
		return 1;
	}


	// Instantiate the DOM parser.
	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };

	impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	//parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

/*
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

  */

	MemBufInputSource *isource = new MemBufInputSource( (const XMLByte*)xmlString->c_str(), strlen(xmlString->c_str()), "XMLBuffer", false);
	if (isource == NULL) return 5;
    XercesDOMParser* parser2 = new XercesDOMParser;


	//bool more = true;

	try
	{
		// reset document pool
		parser2->resetDocumentPool();
		parser2->parse(*isource);
		xml_doc = parser2->getDocument();
		
		if (xml_doc==NULL) std::cout << "xml_doc==NULL!!!!" << std::endl;
		DOMElement* root =  0;
		root = xml_doc->getDocumentElement();
	}	
	catch (const XMLException& toCatch)
	{
		std::cerr << "\nError during parsing a xmlString\n"
			<< "Exception message is:  \n" << toCatch.getMessage() << "\n" << std::endl;
		errorOccurred = true;		
	}
	catch (const DOMException& toCatch)
	{
		const unsigned int maxChars = 2047;
		XMLCh errText[maxChars + 1];

		std::cerr << "\nDOM Error during parsing a xmlString\n"
			<< "DOMException code is:  " << toCatch.code << std::endl;

		if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
			std::cerr << "Message is: " << errText << std::endl;

		errorOccurred = true;
	}
	catch (...)
	{
		std::cerr << "\nUnexpected exception during parsing a xmlString\n";
		errorOccurred = true;
	}

	//  Extract the DOM tree, get the list of all the elements and report the length as the count of elements.
	//unsigned int elementCount = 0;
	if (xml_doc)
	{
		rootNode =(DOMNode*)xml_doc->getDocumentElement();
	}

	if (errorOccurred) return 4;
	return 0;
}


XMLParser::~XMLParser()
{
	//  Delete the parser itself.  Must be done prior to calling Terminate, below.
	parser->release();

	// And call the termination method
	XMLPlatformUtils::Terminate();
}