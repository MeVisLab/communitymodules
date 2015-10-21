//----------------------------------------------------------------------------------
//! The ML module class DicomSurfaceSegmentationLoad.
/*!
// \file    dicomSSODatasetParser.cpp
// \author  Christian Hoffmann
// \date    2013-11
//
// 
*/
//----------------------------------------------------------------------------------

#include "dicomSSODatasetParser.h"


ML_START_NAMESPACE


/**
 * \brief  Standard-constructor.
 *
 * \param  [in] dataset The dataset to parse.
 * \throws std::exception If the dataset-pointer is empty, the dataset is empty or 
 *         the dataset does not hold a surface segmentation object.
 */
DicomSSODatasetParser::DicomSSODatasetParser(SmartPtr_DCMDataSet dataset) : DicomDatasetParser(dataset)
{
	DicomDatasetParser parser(dataset);
	if (parser.getMediaStorageSOPClassUID() != UID_SurfaceSegmentationStorage) 
  {
		std::cerr << "Creating DicomSSODatasetParser instance failed: MediaStorageSOPClassUID is not SurfaceSegmentationStorage.";
		throw std::exception("MediaStorageSOPClassUID is not SurfaceSegmentationStorage.");
	}
}

/**
 * \brief  Returns all 3D-elements contained in the current dataset.
 *
 * \param  [in] isParsingNormalsEnabled Enables the retrieval of the vertice normals.
 * \param  [in] treadSegmentationObjectsOfUnknownTypeAsMeshes If this parameter is activated, surface segmentation objects of unknown type are treated as meshes.
 *         Otherwise, they are discarded. The geometrical element type is determined by Coordinates3D::setSurfaceComment(char*).
 * \return All 3D-elements contained in the current dataset. If it does not hold any
 *         3D-elements, an empty vector is returned.
 */
std::vector<Element3D> DicomSSODatasetParser::parseDataset(const bool isParsingNormalsEnabled) const
{
	std::vector<DcmItem*> segmentSequenceItems = getSequenceItems(DCM_SegmentSequence); //does not perform a deep-search
	std::vector<Element3D> elements3d = parseSegmentSequenceItems(segmentSequenceItems);

	std::vector<DcmItem*> referencedSeriesSequenceItems = getSequenceItems(DCM_ReferencedSeriesSequence);
	if(!referencedSeriesSequenceItems.empty()) {
		//note: only the referenced series ID of the *first* referenced series sequence is added to the 3D-elements
		addReferencedSeriesSequenceInformationToElements(elements3d, referencedSeriesSequenceItems[0]);
	}

	std::vector<DcmItem*> surfaceSequenceItems = getSequenceItems(DCM_SurfaceSequence); 
	std::vector<DcmItem*> recommendedPresentationTypeItems = getSequenceItems(DCM_RecommendedPresentationType); 
	std::vector<Coordinates3D> coordinates3d = parseSurfaceSequenceItems(surfaceSequenceItems, isParsingNormalsEnabled);
	matchElementsWithCoordinates(elements3d, coordinates3d);

	return elements3d;
}

/**
 * \brief  Parses all items of a segment-sequence into Element3D-instances.
 *
 * \param  [in] segmentSequenceItems The items to parse.
 * \return The resulting Element3D-instances. If no 3D-elements were found, an
 *         empty vector is returned.
 */
std::vector<Element3D> DicomSSODatasetParser::parseSegmentSequenceItems(std::vector<DcmItem*> segmentSequenceItems) const
{
	std::vector<Element3D> result;

	BOOST_FOREACH(DcmItem* item, segmentSequenceItems) {
		DcmElement* element = reinterpret_cast<DcmElement*>(item);
		Element3D element3D(element);

		//SegmentNumber
		boost::optional<unsigned short> segmentNumber = searchFirstElementUnsignedShort(element, DCM_SegmentNumber);
		if(segmentNumber)
			element3D.setSegmentNumber(*segmentNumber);

		//SegmentLabel
		element3D.setSegmentLabel(searchFirstElementChar(element, DCM_SegmentLabel));

		//AnatomicRegion
		DcmElement* anatomicRegionSequence = searchFirstElement(element, DCM_AnatomicRegionSequence);
		if(anatomicRegionSequence != NULL)
			element3D.setAnatomicRegion(searchFirstElementChar(anatomicRegionSequence, DCM_CodeMeaning));

		//ReferencedSurfaceNumber
		DcmElement* referencedSurfaceSequence = searchFirstElement(element, DCM_ReferencedSurfaceSequence);
		if(referencedSurfaceSequence != NULL) {
			boost::optional<unsigned long> referencedSurfaceNumber = searchFirstElementUnsignedLong(referencedSurfaceSequence, DCM_ReferencedSurfaceNumber);
			if(referencedSurfaceNumber)
				element3D.setReferencedSurfaceNumber(*referencedSurfaceNumber);
		}
		
		result.push_back(element3D);
	}

	return result;
}

/**
 * \brief  Parses all items of a surface-sequence into Coordinates3D-instances.
 *
 * \param  [in] surfaceSequenceItems The items to parse.
 * \param  [in] isParsingNormalsEnabled Enables the retrieval of the vertice normals using 
 *         the surface-sequence.
 * \param  [in] treadSegmentationObjectsOfUnknownTypeAsMeshes If this parameter is activated, surface segmentation objects of unknown type are treated as meshes.
 *         Otherwise, they are discarded. The geometrical element type is determined by Coordinates3D::setSurfaceComment(char*).
 * \return The resulting Coordinates3D-instances. If no coordinates were found, an
 *         empty vector is returned.
 */
std::vector<Coordinates3D> DicomSSODatasetParser::parseSurfaceSequenceItems(std::vector<DcmItem*> surfaceSequenceItems, const bool isParsingNormalsEnabled) const
{
	std::vector<Coordinates3D> result;

	BOOST_FOREACH(DcmItem* surfaceSequence, surfaceSequenceItems) 
  {
		Coordinates3D coordinates3d;

		DcmElement* surfaceSequenceE = reinterpret_cast<DcmElement*>(surfaceSequence);

		//SurfaceNumber
		boost::optional<unsigned long> surfaceNumber = searchFirstElementUnsignedLong(surfaceSequenceE, DCM_SurfaceNumber);
		if(surfaceNumber)
			coordinates3d.setSurfaceNumber(*surfaceNumber);

		//SurfaceComments
		coordinates3d.setSurfaceComment(searchFirstElementChar(surfaceSequenceE, DCM_SurfaceComments));

		//IsVolumeFinite
		std::string isVolumeFinite = searchFirstElementChar(surfaceSequenceE, DCM_FiniteVolume);
		std::transform(isVolumeFinite.begin(), isVolumeFinite.end(), isVolumeFinite.begin(), ::tolower);
		if(isVolumeFinite == "yes")
			coordinates3d.setIsVolumeFinite(true); //false is default

		//Coordinates
		unsigned long numberOfCoordinates;
		const float* coordinates = getCoordinates(surfaceSequence, numberOfCoordinates);
		if(coordinates == NULL) 
			continue;
		std::vector<DcmItem*> surfaceMeshSequenceItems = getSequenceItems(surfaceSequence, DCM_SurfaceMeshPrimitivesSequence);
		if(!surfaceMeshSequenceItems.empty()) {
			switch(coordinates3d.getElementType()) 
      {
      case Coordinates3D::ElementType_Marker: 
        {
					addCoordinatesToCoordinates3dInstance(surfaceMeshSequenceItems.at(0), DCM_VertexPointIndexList, coordinates, coordinates3d);
					break;
				}
				case Coordinates3D::ElementType_Centerline: 
        {
					std::vector<DcmItem*> lineSequenceItems = getSequenceItems(surfaceMeshSequenceItems.at(0), DCM_LineSequence);
					if(lineSequenceItems.empty()) 
						break;
					addCoordinatesToCoordinates3dInstance(lineSequenceItems.at(0), DCM_PrimitivePointIndexList, coordinates, coordinates3d);
					break;
				}
				case Coordinates3D::ElementType_Unknown: 
        {
						break;
				}
				case Coordinates3D::ElementType_Segmentation: 
        {
					addCoordinatesToCoordiantes3DInstanceUsingIndices(surfaceMeshSequenceItems.at(0), DCM_TrianglePointIndexList, coordinates, coordinates3d, numberOfCoordinates);
					std::vector<DcmItem*> surfacePointsSequenceItems = getSequenceItems(surfaceSequence, DCM_SurfacePointsSequence);
					std::vector<DcmItem*> surfacePointsNormalsSequenceItems = getSequenceItems(surfaceSequence, DCM_SurfacePointsNormalsSequence);
					if(surfacePointsSequenceItems.empty() || surfacePointsNormalsSequenceItems.empty()) 
						break;
					if(isParsingNormalsEnabled)
						setNormalOfCoordinates3dInstance(surfacePointsNormalsSequenceItems[0], coordinates3d);
					break;
				}
			} // switch
		}
		result.push_back(coordinates3d);
	}
	return result;
}

/**
 * \brief  Returns the coordinates of all surface-points contained in a given surface-sequence.
 *
 * \param  [in] surfaceSequence
 * \param  [out] numberOfCoordinates
 * \return
 */
const float* DicomSSODatasetParser::getCoordinates(DcmItem* surfaceSequence, unsigned long& numberOfCoordinates) const
{
	numberOfCoordinates = 0;

	std::vector<DcmItem*> surfacePointsSequenceItems = getSequenceItems(surfaceSequence, DCM_SurfacePointsSequence);
	if(surfacePointsSequenceItems.empty()) 
		return NULL;

	if(surfacePointsSequenceItems.at(0)->findAndGetUint32(DCM_NumberOfSurfacePoints, numberOfCoordinates).bad())
		return NULL;

	const float* surfacePoints;
	if(surfacePointsSequenceItems.at(0)->findAndGetFloat32Array(DCM_PointCoordinatesData, surfacePoints).bad()) 
		return NULL;

	return surfacePoints;
}

/**
 * \brief  Adds coordinates to an instance of Coordinates3D.
 *
 * This function is especially suitable for markers and centerlines. For meshes, see <c>addCoordinatesToCoordiantes3DInstanceUsingIndices</c>.
 *
 * \param  [in] surfaceMeshSequenceItem The surfacemesh-sequence item containing the pointIndexList.
 * \param  [in] pointIndexListTag The tag of the pointIndexList.
 * \param  [in] coordinates The coordinates to be added to the Coordinates3D-instance.
 * \param  [in,out] coordinates3d The Coordinates3D-instance.
 * \return True if the operation was successful, false otherwise.
 */
bool DicomSSODatasetParser::addCoordinatesToCoordinates3dInstance(DcmItem* surfaceMeshSequenceItem, const DcmTag pointIndexListTag, const float* coordinates, Coordinates3D& coordinates3d)
{
	const Uint16* pointIndexList;
	unsigned long pointIndexListNumElements;

	if(surfaceMeshSequenceItem->findAndGetUint16Array(pointIndexListTag, pointIndexList, &pointIndexListNumElements).bad()) 
		return false;

	for (unsigned long i=0; i<pointIndexListNumElements; i++)
		coordinates3d.addCoordinate(coordinates[(pointIndexList[i]-1)*3], coordinates[(pointIndexList[i]-1)*3+1], coordinates[(pointIndexList[i]-1)*3+2]);

	return true;
}

/**
 * \brief  Adds coordinates to an instance of Coordinates3D using the indices stored in a pointIndexList.
 *
 * This function is especially suitable for meshes. For markers and centerlines, see <c>addCoordinatesToCoordiantes3DInstanceUsingIndices</c>.
 *
 * \param  [in] surfaceMeshSequenceItem The surfacemesh-sequence item containing the pointIndexList.
 * \param  [in] pointIndexListTag The tag of the pointIndexList.
 * \param  [in] coordinates The coordinates to be added to the Coordinates3D-instance.
 * \param  [in,out] coordinates3d The Coordinates3D-instance.
 * \param  [in] numOfCoordinates The number of coordinates stored in the <c>coordinates</c>-parameter.
 * \return True if the operation was successful, false otherwise.
 */
bool DicomSSODatasetParser::addCoordinatesToCoordiantes3DInstanceUsingIndices(DcmItem* surfaceMeshSequenceItem, DcmTag pointIndexListTag, const float* coordinates, Coordinates3D& coordinates3d, const unsigned long numOfCoordinates)
{
	const Uint16* pointIndexList;
	unsigned long pointIndexListNumElements;

	if(surfaceMeshSequenceItem->findAndGetUint16Array(pointIndexListTag, pointIndexList, &pointIndexListNumElements).bad()) 
		return false;
	
	const unsigned short lowestValueInList = getLowestValueInList(pointIndexList, pointIndexListNumElements);
	if(lowestValueInList != 1) { //1 because in DICOM the lowest pointIndexList-entry is 1
		std::stringstream errorMessageStream;
		errorMessageStream << "A mesh (surface #" << coordinates3d.getSurfaceNumber().get() << ") does not adhere to the DICOM standard! \n\nThe lowest index contained in the list pertaining to tag " << pointIndexListTag.getTagName() << " is " << lowestValueInList << " instead of 1. \nWorkaround is active.";
		std::string errorMessage = errorMessageStream.str();
		MessageBoxA(NULL, errorMessage.c_str(), "DICOM standard violation", MB_OK | MB_ICONWARNING | MB_SYSTEMMODAL);
		boost::erase_all(errorMessage, "\r");
		boost::erase_all(errorMessage, "\n");
		std::cerr << errorMessage;
	}

	for (unsigned long i=0; i<pointIndexListNumElements; i++)
		coordinates3d.addCoordinateIndex(pointIndexList[i] - lowestValueInList); 

	for (unsigned long i=0; i<numOfCoordinates; i++)
		coordinates3d.addCoordinate(coordinates[i*3], coordinates[i*3+1], coordinates[i*3+2]);

	return true;
}

/**
 * \brief  Sets the normal of a Coordinates3D-instance.
 *
 * \param  [in] surfacePointsNormalsSequenceItem The surface points normals sequence-item containing the normal.
 * \param  [in,out] coordinates3d The Coordinates3D-instance which normal is to be set.
 * \return True if the normal was successfully retrieved from the sequence-item; false otherwise.
 */
bool DicomSSODatasetParser::setNormalOfCoordinates3dInstance(DcmItem* surfacePointsNormalsSequenceItem, Coordinates3D& coordinates3d)
{
	const float* normalCoords;
	unsigned long numNormalCoords = 0;
	if(surfacePointsNormalsSequenceItem->findAndGetFloat32Array(DCM_VectorCoordinateData, normalCoords).bad(), &numNormalCoords) return false;
	
	for (std::size_t i=0; i<numNormalCoords; i+=3)
		coordinates3d.addNormal(normalCoords[i], normalCoords[i+1], normalCoords[i+2]);

	return true;
}

/**
 * \brief  Adds the ID of a series instance referenced in a referenced series sequence to all element os a surface sequence.
 * 
 * \param  [in,out] surfaceSequenceItems The Element3D-instances the referenced ID is added to.
 * \param  [in] referencedSeriesSequenceItem The referenced series sequence item containing the ID.
 * \return True if the ID was successfully retrieved from the referenced series sequence, false otherwise.
 */
bool DicomSSODatasetParser::addReferencedSeriesSequenceInformationToElements(std::vector<Element3D>& surfaceSequenceItems, DcmItem* referencedSeriesSequenceItem) const
{
	if(referencedSeriesSequenceItem == NULL) {
		std::cerr << "Referenced series sequence item is NULL. Returning without adding referenced series instance ids to the surface sequence items.";
		return false;
	}

	char* referencedSeriesInstanceId = searchFirstElementChar(reinterpret_cast<DcmElement*>(referencedSeriesSequenceItem), DCM_SeriesInstanceUID);
	
	for (std::size_t i=0; i<surfaceSequenceItems.size(); i++)
		surfaceSequenceItems[i].setReferencedSeriesInstanceId(referencedSeriesInstanceId);

	return true;
}

/**
 * \brief  Matches surface-sequence items with the corresponding 3D-coordinates.
 *
 * To each surface-sequence item, the corresponding Coordinates3D-instance is associated 
 * via the referenced surface number.
 *
 * \param  [in,out] surfaceSequenceItems The surface-sequence items which will get associations to their
 *         corresponding 3D-coordinates.
 * \param  [in] coordinates3d The Cooordinate3D-instances which will be linked with the surface-sequence items.
 */
void DicomSSODatasetParser::matchElementsWithCoordinates(std::vector<Element3D>& surfaceSequenceItems, std::vector<Coordinates3D>& coordinates3d)
{
	for (std::size_t i=0; i<surfaceSequenceItems.size(); i++)
	{
		boost::optional<unsigned long> referencedSurfaceNumber = surfaceSequenceItems[i].getReferencedSurfaceNumber();
		if(!referencedSurfaceNumber)
			return;

		for (std::size_t j=0; j<coordinates3d.size(); j++) {
			boost::optional<unsigned long> surfaceNumber = coordinates3d[j].getSurfaceNumber();
			if(surfaceNumber && *surfaceNumber == *referencedSurfaceNumber)
				surfaceSequenceItems[i].setReferencedSurface(coordinates3d[j]);
		}
	}
}


/**
 * \brief  Returns the lowest value contained in a list.
 *
 * \param  [in] list The list to examine.
 * \param  [in] numElements The number of elements contained in the list.
 * \return The lowest value contained in the list.
 * \throws std::exception If the list is NULL.
 */
unsigned short DicomSSODatasetParser::getLowestValueInList(const unsigned short* list, const unsigned long numElements)
{
	if(list == NULL)
		throw std::exception("Parameter list is NULL.");

	unsigned short result = std::numeric_limits<unsigned short>::max();

	for (unsigned long i=0; i<numElements; i++)
		if(list[i] < result)
			result = list[i];

	return result;
}


ML_END_NAMESPACE