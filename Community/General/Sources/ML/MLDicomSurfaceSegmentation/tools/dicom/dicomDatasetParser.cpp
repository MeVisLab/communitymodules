//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    dicomDatasetParser.cpp
// \author  Christian Hoffmann
// \date    2013-11
//
// 
*/
//----------------------------------------------------------------------------------

#include "dicomDatasetParser.h"


ML_START_NAMESPACE

/**
 * \brief  Standard-constructor.
 *
 * \param  [in] dataset The dataset to parse.
 * \throws std::exception If the dataset-pointer or the dataset is empty.
 */
DicomDatasetParser::DicomDatasetParser(SmartPtr_DCMDataSet dataset)
{
	if(!dataset) {
		std::cerr << "The dataset-pointer is empty; the dataset could not be parsed.";
		throw std::exception("Dataset-pointer is empty.");
	}
	if(dataset->isEmpty()) {
		std::cerr << "The dataset is empty and could not be parsed.";
		throw std::exception("Dataset is empty.");
	}
	
	_dataset = dataset;
}

/**
 * \brief  Returns if the dataset holds a surface segmentation.
 *
 * \return True if the MediaStorageSOPClassUID is "SurfaceSegmentationStorage", false otherwise.
 */

std::string  DicomDatasetParser::getMediaStorageSOPClassUID() const
{
  std::string result = "";

	OFString mediaStorageSOPClassUIDString;
	OFCondition status = _dataset->findAndGetOFString(DCM_MediaStorageSOPClassUID, mediaStorageSOPClassUIDString);

  if (status.good())
  {
    result = std::string(mediaStorageSOPClassUIDString.c_str());
  }

  return result;
}

/**
 * \brief  Searches for the first instance of a tag in all sub-elements of a given DcmElement.
 *
 * \param  [in] ancestor The DcmElement whose sub-elements will be examined.
 * \param  [in] tag The tag to be searched.
 * \return The first found element tagged according to the supplied tag. If ancestor is NULL
 *         or no suitable element was found , NULL is returned.
 */
DcmElement* DicomDatasetParser::searchFirstElement(DcmElement* ancestor, const DcmTag tag) const
{
	if(ancestor == NULL) {
		std::cerr << "Ancestor is NULL. Returning NULL.";
		return NULL;
	}

	DcmStack stack;
	if(ancestor->search(tag, stack).bad() || stack.empty())
		return NULL;
	
	return static_cast<DcmElement*>(stack.pop());
}

/**
 * \brief  Searches for the first instance of a tag in all sub-elements of a given DcmElement.
 *
 * The value-representation of the respective sub-element has to be string-compatible.
 *
 * \param  [in] ancestor The DcmElement whose sub-elements will be examined.
 * \param  [in] tag The tag to be searched.
 * \return A string-representation of the first found element tagged according to the supplied
 *         tag. If ancestor is NULL or no suitable element was found, an empty string is 
 *         returned.
 */
char* DicomDatasetParser::searchFirstElementChar(DcmElement* ancestor, const DcmTag tag) const
{
	DcmElement* resultE = searchFirstElement(ancestor, tag);
	char* result = "";
	if(resultE != NULL && resultE->getString(result).good())
		return result;
	return "";
}

/**
 * \brief  Searches for the first instance of a tag in all sub-elements of a given DcmElement.
 *
 * The value-representation of the respective sub-element has to be compatible to unsigned short.
 *
 * \param  [in] ancestor The DcmElement whose sub-elements will be examined.
 * \param  [in] tag The tag to be searched.
 * \return An unsigned short-representation of the first found element tagged according to the
 *         supplied tag. If ancestor is NULL or no suitable element was found, boost::none 
 *         is returned.
 */
boost::optional<unsigned short> DicomDatasetParser::searchFirstElementUnsignedShort(DcmElement* ancestor, const DcmTag tag) const
{
	DcmElement* resultE = searchFirstElement(ancestor, tag);
	unsigned short result = 0;
	if(resultE != NULL && resultE->getUint16(result).good())
		return result;
	return boost::none;
}

/**
 * \brief  Searches for the first instance of a tag in all sub-elements of a given DcmElement.
 *
 * The value-representation of the respective sub-element has to be compatible to unsigned long.
 *
 * \param  [in] ancestor The DcmElement whose sub-elements will be examined.
 * \param  [in] tag The tag to be searched.
 * \return An unsigned long-representation of the first found element tagged according to the
 *         supplied tag. If ancestor is NULL or no suitable element was found, boost::none 
 *         is returned.
 */
boost::optional<unsigned long> DicomDatasetParser::searchFirstElementUnsignedLong(DcmElement* ancestor, const DcmTag tag) const
{
	DcmElement* resultE = searchFirstElement(ancestor, tag);
	unsigned long result = 0;
	if(resultE != NULL && resultE->getUint32(result).good())
		return result;
	return boost::none;
}

/**
 *  \brief  Returns the attribute value associated with a specific tag.
 *
 * The value representation for the tag has to convertible into a char sequence.
 *
 *  \return The value of the tag. If retrieving the value failed, an empty string is returned.
 */
inline const char* DicomDatasetParser::getCharAttribute(const DcmTagKey tag) const
{
	OFString result;
	if(_dataset->findAndGetOFString(tag, result).bad())
		return "";

	const char* resultC = result.c_str();
	return resultC;
}

/**
 *  \brief  Returns the attribute value associated with a specific tag.
 *
 * The value representation for the tag has to convertible into a string.<br />
 * Note that only the top-most level of the current dataset is exmained, i.e. this 
 * method does not perform a deep-search.
 *
 *  \return The value of the tag. If retrieving the value failed, an empty string is returned.
 */
std::string DicomDatasetParser::getStringAttribute(const DcmTagKey tag) const
{
	const char* resultChars;
  std::string result = "";

  if(_dataset->findAndGetString(tag, resultChars).good())
  {
		result = std::string(resultChars);
  }

	return result;
}

/**
 * \brief  Returns all elements associated with a certain tag.<br />
 *
 * Note that this function always performs a deep-search.
 *
 * \param  [in] tag The tag to serach for.
 * \return All elements with the given tag contained in the dataset. If no
 *         element was found, an empty stack is returned.
 */
boost::shared_ptr<DcmStack> DicomDatasetParser::getElementsByTag(const DcmTag tag) const
{
	boost::shared_ptr<DcmStack> result(new DcmStack());
	_dataset->findAndGetElements(tag, *result);
	return result;
}

/**
 * \brief  Returns all items which are direct children of a given sequence-tag.
 *
 * Note that only the top-most level of the current dataset is exmained, i.e. this 
 * method does not perform a deep-search.
 *
 * \param  [in] sequenceTag The sequence-tag to search for.
 * \return All found items. If no items were found during the search, an empty vector 
 *         is returned.
 */
std::vector<DcmItem*> DicomDatasetParser::getSequenceItems(const DcmTag sequenceTag) const
{
	std::vector<DcmItem*> sequenceItems;

	long i=0;
	DcmItem* foundItem;
	while(_dataset->findAndGetSequenceItem(sequenceTag, foundItem, i).good()) {
		sequenceItems.push_back(foundItem);
		i++;
	}

	return sequenceItems;
}

/**
 * \brief  Returns all items which are direct children of a given sequence-tag.
 *
 * Note that only the top-most level of the start-item is exmained, i.e. this 
 * method does not perform a deep-search.
 *
 * \param  [in] item The item the search is started from. 
 * \param  [in] sequenceTag The sequence-tag to search for.
 * \return All found items. If the <c>item</c> is NULL or no items were found during
 *         the search, an empty vector is returned.
 */
std::vector<DcmItem*> DicomDatasetParser::getSequenceItems(DcmItem* item, const DcmTag sequenceTag) const
{
	std::vector<DcmItem*> sequenceItems;
	if(item == NULL) {
		std::cerr << "Item is NULL. Returning an empty vector.";
		return sequenceItems;
	}

	long i=0;
	DcmItem* foundItem;
	while(item->findAndGetSequenceItem(sequenceTag, foundItem, i).good()) {
		sequenceItems.push_back(foundItem);
		i++;
	}

	return sequenceItems;
}

ML_END_NAMESPACE