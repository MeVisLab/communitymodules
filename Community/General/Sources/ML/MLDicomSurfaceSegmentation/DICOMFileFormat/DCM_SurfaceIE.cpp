//----------------------------------------------------------------------------------
//! The ML module class MLDicomSurfaceSegmentation.
/*!
// \file    DCM_SurfaceIE.cpp
// \author  Axel Newe
// \date    2015-09-25
//
// 
*/
//----------------------------------------------------------------------------------

#include "DCM_SurfaceIE.h"









ML_START_NAMESPACE


void SurfaceIE::AddSurfaceIE(SmartPtr_DCMDataSet datasetPointer)
{
  // Mandatory
  SurfaceIE::_addSurfaceSegmentationModule(datasetPointer);

  // Mandatory
  SurfaceIE::_addSurfaceMeshModule(datasetPointer);

  // Conditional - Required if the surface has been derived from another SOP Instance
  //SurfaceIE::_addCommonInstanceReferenceModule(datasetPointer);

  // Mandatory
  SurfaceIE::_addSOPCommonModule(datasetPointer);
}


void SurfaceIE::_addSurfaceSegmentationModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.8.X.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  //     Include ‘Content Identification Macro’ Table 10-12
  // Add Segment Sequence                           | (0062,0002) | 1 | Describes the segments that are contained within the data. One or more Items shall be present.
  // Add >Segment Number                            | (0062,0004) | 1 | Identification number of the segment. Uniquely identifies a segment within the SOP Instance.
  // Add >Segment Label                             | (0062,0005) | 1 | User-defined label identifying this segment. This may be the same as the Code Meaning (0008,0104) of the Segmented Property Type Code Sequence (0062,000F).
  // Add >Segment Description                       | (0062,0006) | 3 | User-defined description for this segment.
  // Add >Segment Algorithm Type                    | (0062,0008) | 1 | Type of algorithm used to generate the segment. Enumerated Values are: AUTOMATIC = calculated segment SEMIAUTOMATIC = calculated segment with user assistance MANUAL = user-entered segment
  //     >Include ‘General Anatomy Mandatory Macro’ Table 10-5
  // Add >Segmented Property Category Code Sequence | (0062,0003) | 1 | Sequence defining the general category of this segment. This sequence shall contain one item.
  //     >>Include ‘Code Sequence Macro’ Table 8.8-1 Baseline Context ID is 7150.
  // Add >Segmented Property Type Code Sequence     | (0062,000F) | 1 | Sequence defining the specific property type of this segment. This sequence shall contain one item.
  //     >>Include ‘Code Sequence Macro’ Table 8.8-1
  // Add >Surface Count                             | (0066,002A) | 1 | The number of surfaces which comprise this segment. Shall be greater than zero.
  // Add >Referenced Surface Sequence               | (0066,002B) | 1 | Sequence referencing the surfaces composed to construct this segment. The number of Items shall equal the value of Surface Count (0066,002A).
  // Add >>Referenced Surface Number                | (0066,002C) | 1 | Identifies the Surface Number (0066,0003) within the Surface Sequence (0066,0002) to which this reference applies.
  // Add >>Segment Surface Generation Algorithm Identification Sequence | (0066,002D) | 1 | A description of how this segment surface was derived.
  //     >>>Include ‘Algorithm Identification Macro’ Table 10-X Baseline Context ID for Algorithm Family Code Sequence (0066,002F) is 7162.
  // Add >>Segment Surface Source Instance Sequence | (0066,002E) | 2 | A Sequence that identifies the set of Instances by their SOP Class/Instance pair that were used to derive this segment surface. Zero or more items shall be included in this Sequence.
  //     >>>Include ‘Image SOP Instance Reference Macro’ Table C.10-3

  DcmPersonName* PatientNameElement = new DcmPersonName(DCM_PatientName);
  PatientNameElement->putNameComponents("Doe", "John", "", "Dr.", "II.");

  if (datasetPointer->insert(PatientNameElement, true, false).bad())
  {
    std::cout << "Adding patient name (0010,0010) failed.\n";
  }

  DcmLongString* PatientIDElement = new DcmLongString(DCM_PatientID);
  PatientIDElement->putOFStringArray("0123456789");

  if (datasetPointer->insert(PatientIDElement, true, false).bad())
  {
    std::cout << "Adding patient ID (0010,0020) failed.\n";
  }

  DcmDate* PatientBirthDateElement = new DcmDate(DCM_PatientBirthDate);
  PatientBirthDateElement->setOFDate(OFDate(1972,6,26));

  if (datasetPointer->insert(PatientBirthDateElement, true, false).bad())
  {
    std::cout << "Adding patient birth date (0010,0030) failed.\n";
  }

  DcmCodeString* PatientSexElement = new DcmCodeString(DCM_PatientSex);
  PatientSexElement->putOFStringArray("M");

  if (datasetPointer->insert(PatientSexElement, true, false).bad())
  {
    std::cout << "Adding patient sex (0010,0040) failed.\n";
  }
}

void SurfaceIE::_addSurfaceMeshModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.X.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add Number of Surfaces                   | (0066,0001) | 1  | Number of surfaces contained in the Instance. Shall be 1 or more. Shall be the same as the number of Items in Surface Sequence (0066,0002).
  // Add Surface Sequence                     | (0066,0002) | 1  | The surfaces that are described within the data. There shall be Number of Surfaces (0066,0001) Items in the sequence. See C.X.1.1.1.
  // Add >Surface Number                      | (0066,0003) | 1  | Identification number of the surface. Uniquely identifies a surface within this SOP instance. Shall start at a value of 1, and increase monotonically by 1.
  // Add >Surface Comments                    | (0066,0004) | 3  | User-defined comments describing the surface.
  // Add >Surface Processing                  | (0066,0009) | 2  | Specifies whether the surface has been modified subsequent to the original generation of the surface. Enumerated Values: YES NO See C.X.1.1.2.
  // Add >Surface Processing Ratio            | (0066,000a) | 2C | The Ratio of Remaining points to Original points after processing. Required if Surface Processing (0066,0009) is YES.
  // Add >Surface Processing Description      | (0066,000b) | 3  | A textual description of the surface processing performed.
  // Add >Surface Processing Algorithm Identification Sequence (0066,0035) | 2C | Describes the processing method. Required if Surface Processing (0066,0009) is YES.
  //     >>Include ‘Algorithm Identification Macro’ Table 10-X Baseline Context ID for Algorithm Family Code Sequence (0066,002F) is 7162.
  // Add >Recommended Display Grayscale Value | (0062,000C) | 1  | A default single gray unsigned value in which it is recommended that the maximum pixel value in this surface be rendered on a monochrome display. The units are specified in P-Values from a minimum of 0000H (black) up to a maximum of FFFFH (white). Note: The maximum P-Value for this Attribute may be different from the maximum P-Value from the output of the Presentation LUT, which may be less than 16 bits in depth.
  // Add >Recommended Display CIELab Value    | (0062,000D) | 1  | A default triplet value in which it is recommended that the surface be rendered on a color display. The units are specified in PCS-Values, and the value is encoded as CIELab. See C.10.7.1.1
  // Add >Recommended Presentation Opacity    | (0066,000c) | 1  | Specifies the opacity in which it is recommended that the surface be rendered. See C.X.1.1.3.
  // Add >Recommended Presentation Type       | (0066,000d) | 1  | Specifies the presentation type in which it is recommended that the surface be rendered. See C.X.1.1.3.
  // Add >Finite Volume                       | (0066,000e) | 1  | Indicates, whether the surface represents a solid (“waterproof”) object with an outside and an inside. Enumerated Values: YES = Contains a finite volume NO = Does not contain a finite volume UNKNOWN = Might or might not contain a finite volume See C.X.1.1.4.
  // Add >Manifold                            | (0066,0010) | 1  | Indicates whether the surface is describing an n-1 dimensional manifold in the underlying n-dimensional vector space. Enumerated Values: YES = Manifold in every point NO = Does contain non-manifold points UNKNOWN = Might or might not contain non-manifold points See C.X.1.1.5.
  // Add >Surface Points Sequence             | (0066,0011) | 1  | The point positions representing vertices of the surface. Only one item shall be permitted in the sequence.
  //     >> Include ‘Points Macro’ Table C.X-2
  // Add >Surface Points Normals Sequence     | (0066,0012) | 2  | The normals on the surface for each point. Only one item shall be permitted in the sequence. See C.X.1.1.6.
  //     >>Include ‘Vectors Macro’ Table C.X-3 The Number of Vectors (0066,001E) shall equal Number of Points (0066,0015) in this Surface Sequence Item.. The Vector Dimensionality (0066,001F) shall be 3. If Finite Volume (0066,000e) is YES, the normals of the vertices shall point toward the outside of the object. If Finite Volume (0066,000e) is not YES, the direction of the normals shall be consistent where possible.
  // Add >Surface Mesh Primitives Sequence    | (0066,0013) | 1  | Only one item shall be permitted in the sequence. 
  //     >>Include ‘Surface Mesh Primitives Macro Table’ C.X-4 The primitives’ indices shall not exceed Number of Points (0066,0015) in this Surface Sequence Item..


  DcmPersonName* PatientNameElement = new DcmPersonName(DCM_PatientName);
  PatientNameElement->putNameComponents("Doe", "John", "", "Dr.", "II.");

  if (datasetPointer->insert(PatientNameElement, true, false).bad())
  {
    std::cout << "Adding patient name (0010,0010) failed.\n";
  }

  DcmLongString* PatientIDElement = new DcmLongString(DCM_PatientID);
  PatientIDElement->putOFStringArray("0123456789");

  if (datasetPointer->insert(PatientIDElement, true, false).bad())
  {
    std::cout << "Adding patient ID (0010,0020) failed.\n";
  }

  DcmDate* PatientBirthDateElement = new DcmDate(DCM_PatientBirthDate);
  PatientBirthDateElement->setOFDate(OFDate(1972,6,26));

  if (datasetPointer->insert(PatientBirthDateElement, true, false).bad())
  {
    std::cout << "Adding patient birth date (0010,0030) failed.\n";
  }

  DcmCodeString* PatientSexElement = new DcmCodeString(DCM_PatientSex);
  PatientSexElement->putOFStringArray("M");

  if (datasetPointer->insert(PatientSexElement, true, false).bad())
  {
    std::cout << "Adding patient sex (0010,0040) failed.\n";
  }



// POINTS MACRO ATTRIBUTES
// 
// Number Of Points                | (0066,0015) | 1  | Specifies the number of points in the point set.
// Point Coordinates Data          | (0066,0016) | 1  | See C.X.2.1.1
// Point Position Accuracy         | (0066,0017) | 3  | A single standard deviation of the error for all the points’ spatial positions. The units shall be the same as the units of the coordinate system in which the point coordinates are specified.
// Average Point Distance          | (0066,0018) | 3  | The average point distance of the point set. It is given by the average of the distances to the nearest neighbor over all points. The units shall be the same as the units of the coordinate system in which the point coordinates are specified.
// Maximum Point Distance          | (0066,0019) | 3  | The maximum distance of one point to its nearest neighbor. The units shall be the same as the units of the coordinate system in which the point coordinates are specified.
// Points Bounding Box Coordinates | (0066,001A) | 3  | Two 3D locations defining the cuboid bounding box, parallel to the coordinate system axes, encompassing the point set.
// Axis of Rotation                | (0066,001B) | 3  | A 3D location that combined with Center of Rotation (0066,001C) specifies the preferred axis of rotation of this object.
// Center of Rotation              | (0066,001C) | 1C | A 3D location defining the preferred center of rotation for this point set. Required if Axis of Rotation (0066,001B) is present. May be present otherwise.


// VECTORS MACRO ATTRIBUTES

// Number of Vectors      | (0066,001E) | 1 | The number of vectors in the Vector Coordinate Data (0066,0021). See C.X.3.1.
// Vector Dimensionality  | (0066,001F) | 1 | The dimensionality of the underlying vector field. See C.X.3.1.
// Vector Accuracy        | (0066,0020) | 3 | A single standard deviation for all the vectors’ coordinates. The units shall be the same as the units of the coordinate system in which the vector coordinates are specified. See C.X.3.1.
// Vector Coordinate Data | (0066,0021) | 1 | A data stream of coordinates encoded as floats. See C.X.3.1.


// SURFACE MESH PRIMITIVES MACRO ATTRIBUTES

// Vertex Point Index List     | (0066,0025) | 2 | Contains n point indices describing Vertices. See C.X.4.1.
// Edge Point Index List       | (0066,0024) | 2 | Contains 2n point indices describing unconnected Edges. See C.X.4.1.
// Triangle Point Index List   | (0066,0023) | 2 | Contains 3n point indices describing unconnected Triangles. See C.X.4.1.
// Triangle Strip Sequence     | (0066,0026) | 2 | All Triangle Strips in this Surface. Zero or more Items shall be present.
// >Primitive Point Index List | (0066,0029) | 1 | See C.X.4.1.
// Triangle Fan Sequence       | (0066,0027) | 2 | All Triangle Fans in this Surface. Zero or more Items shall be present.
// >Primitive Point Index List | (0066,0029) | 1 | See C.X.4.1.
// Line Sequence               | (0066,0028) | 2 | All Lines in this Surface. Zero or more Items shall be present.
// >Primitive Point Index List | (0066,0029) | 1 | See C.X.4.1.
// Facet Sequence              | (0066,0034) | 2 | All Facets in this Surface. Each sequence Item describes one facet. Zero or more Items shall be present.
// >Primitive Point Index List | (0066,0029) | 1 | See C.X.4.1.


}

void SurfaceIE::_addSOPCommonModule(SmartPtr_DCMDataSet datasetPointer)
{  
  // Reference: C.12.1
  // Type 1: required, needs content | Type 2: required, may be empty | Type 3: optional

  // Add SOP Class UID                         | (0008,0016) | 1  | Uniquely identifies the SOP Class. See C.12.1.1.1 for further explanation. See also PS 3.4.
  // Add SOP Instance UID                      | (0008,0018) | 1  | Uniquely identifies the SOP Instance. See C.12.1.1.1 for further explanation. See also PS 3.4.
  // Add Specific Character Set                | (0008,0005) | 1C | Character Set that expands or replaces the Basic Graphic Set. Required if an expanded or replacement character set is used. See C.12.1.1.2 for Defined Terms.
  // Add Instance Creation Date                | (0008,0012) | 3  | Date the SOP Instance was created. 
  // Add Instance Creation Time                | (0008,0013) | 3  | Time the SOP Instance was created.
  // Add Instance Creator UID                  | (0008,0014) | 3  | Uniquely identifies device which created the SOP Instance.
  // Add Related General SOP Class UID         | (0008,001A) | 3  | Uniquely identifies a Related General SOP Class for the SOP Class of this Instance. See PS 3.4.
  // Add Original Specialized SOP Class UID    | (0008,001B) | 3  | The SOP Class in which the Instance was originally encoded, but which has been replaced during a fall-back conversion to the current Related General SOP Class. See PS 3.4.
  // Add Coding Scheme Identification Sequence | (0008,0110) | 3  | Sequence of items that map values of Coding Scheme Designator (0008,0102) to an external coding system registration, or to a private or local coding scheme. One or more items may be present in the sequence.
  // Add >Coding Scheme Designator             | (0008,0102) | 1  | The value of a Coding Scheme Designator, used in this SOP Instance, which is being mapped.
  // Add >Coding Scheme Registry               | (0008,0112) | 1C | The name of the external registry where further definition of the identified coding scheme may be obtained. Required if  coding scheme is registered. Defined term: HL7
  // Add >Coding Scheme UID                    | (0008,010C) | 1C | The coding scheme UID identifier. Required if coding scheme is identified by an ISO 8824 object identifier compatible with the UI VR. 
  // Add >Coding Scheme External ID            | (0008,0114) | 2C | The coding scheme identifier as defined in an external registry. Required if coding scheme is registered and Coding Scheme UID (0008,010C) is not present.
  // Add >Coding Scheme Name                   | (0008,0115) | 3  | The coding scheme full common name >Coding Scheme Version (0008,0103) 3 The coding scheme version associated with the Coding Scheme Designator (0008,0102).
  // Add >Responsible Organization             | (0008,0116) | 3  | Name of the organization responsible for the Coding Scheme. May include organizational contact information.
  // Add Timezone Offset From UTC              | (0008,0201) | 3  | Contains the offset from UTC to the timezone for all DA and TM Attributes present in this SOP Instance. Encoded as an ASCII string in the format “&ZZZZ”. The components of this string, from left to right, are & = “+” or “-”, and ZZZZ = Hours and Minutes of offset. 
  //     Notes: 
  //     1. This encoding is the same as described in PS 3.5 for the DT Value Representation. 
  //     2. This Attribute does not apply to values with a DT Value Representation, which may contain an explicitly encoded timezone. 
  //     3. The corrected time may cross a 24 hour boundary. For example, if Local Time = 1.00 a.m. and Offset = +0200, then UTC = 11.00 p.m. (23.00) the day before. 
  //     4. The “+” sign may not be omitted. Time earlier than UTC is expressed as a negative offset. Note: For example: UTC = 5.00 a.m. Local Time = 3.00 a.m. Offset = -0200 The local timezone offset is undefined if this Attribute is absent.
  // Add Contributing Equipment Sequence       | (0018,A001) | 3 | Sequence of Items containing descriptive attributes of related equipment which has contributed to the acquisition, creation or modification of the composite instance. One or more Items may be included in this Sequence. See C.12.1.1.5 for further explanation.
  // Add >Purpose of Reference Code Sequence   | (0040,A170) | 1 | Describes the purpose for which the related equipment is being reference. Only a single Item shall be permitted in this sequence. See C.12.1.1.5 for further explanation.
  //     >>Include ‘Code Sequence Macro’ Table 8.8-1 Defined Context ID 7005. 
  // Add >Manufacturer                         | (0008,0070) | 1 | Manufacturer of the equipment that contributed to the composite instance.
  // Add >Institution Name                     | (0008,0080) | 3 | Institution where the equipment that contributed to the composite instance is located.
  // Add >Institution Address                  | (0008,0081) | 3 | Address of the institution where the equipment that contributed to the composite instance is located.
  // Add >Station Name                         | (0008,1010) | 3 | User defined name identifying the machine that contributed to the composite instance.
  // Add >Institutional Department Name        | (0008,1040) | 3 | Department in the institution where the equipment that contributed to the composite instance is located.
  // Add >Manufacturer’s Model Name            | (0008,1090) | 3 | Manufacturer’s model name of the equipment that contributed to the composite instance.
  // Add >Device Serial Number                 | (0018,1000) | 3 | Manufacturer’s serial number of the equipment that contributed to the composite instance.
  // Add >Software Versions                    | (0018,1020) | 3 | Manufacturer’s designation of the software version of the equipment that contributed to the composite instance.
  // Add >Spatial Resolution                   | (0018,1050) | 3 | The inherent limiting resolution in ,, of the acquisition equipment for high contrast objects for the data gathering and reconstruction technique chosen. If variable across the images of the series, the value at the image center.
  // Add >Date of Last Calibration             | (0018,1200) | 3 | Date when the image acquisition device calibration was last changed in any way. Multiple entries may be used for additional calibrations at other times. See C.7.5.1.1.1 for further explanation. 
  // Add >Time of Last Calibration             | (0018,1201) | 3 | Time when the image acquisition device calibration was last changed in any way. Multiple entries may be used. See C.7.5.1.1.1 for further explanation.
  // Add >Contribution DateTime                | (0018,A002) | 3 | The Date & Time when the equipment contributed to the composite instance.
  // Add >Contribution Description             | (0018,A003) | 3 | Description of the contribution the equipment made to the composite instance.
  // Add Instance Number                       | (0020,0013) | 3 | A number that identifies this Composite object instance.
  // Add SOP Instance Status                   | (0100,0410) | 3 | A flag that indicates the storage status of the SOP Instance. Not Specified (NS) implies that this SOP Instance has no special storage status, and hence no special actions need be taken. Original (OR) implies that this is the primary SOP instance for the purpose of storage, but that it has not yet been authorized for
  //         diagnostic use. Authorized Original (AO) implies that this is the primary SOP instance for the purpose of storage, which has been authorized for diagnostic use. Any copies of an Authorized Original should be given the status of Authorized Copy. Authorized Copy (AC) implies that this is a copy of an Authorized Original SOP Instance.
  //         Enumerated Values: NS, OR, AO, AC Note: Proper use of these flags is specified in Security Profiles. Implementations that do not conform to such Security Profiles may not necessarily handle these flags properly.
  // Add SOP Authorization Date and Time       | (0100,0420) | 3 | The date and time when the SOP Instance Status (0100,0410) was set to AO.
  // Add SOP Authorization Comment             | (0100,0424) | 3 | Any comments associated with the setting of the SOP Instance Status (0100,0410) to AO.
  // Add Authorization Equipment Certification Number (0100,0426) 3 The certification number issued to the Application Entity that set the SOP Instance Status (0100,0410) to AO.
  //     Include ‘Digital Signatures Macro’ Table C.12-5
  // Add Encrypted Attributes Sequence          | (0400,0500) | 1C | Sequence of Items containing encrypted DICOM data. One or more Items shall be present. Required if application level confidentiality is needed and certain recipients are allowed to decrypt all or portions of the Encrypted Attributes Data Set. See C.12.1.1.4.1.
  // Add >Encrypted Content Transfer Syntax UID | (0400,0510) | 1  | Transfer Syntax used to encode the encrypted content. Only Transfer Syntaxes that explicitly include the VR and use Little Endian encoding shall be used.
  // Add >Encrypted Content                     | (0400,0520) | 1  | Encrypted data. See C.12.1.1.4.2.


  DcmPersonName* PatientNameElement = new DcmPersonName(DCM_PatientName);
  PatientNameElement->putNameComponents("Doe", "John", "", "Dr.", "II.");

  if (datasetPointer->insert(PatientNameElement, true, false).bad())
  {
    std::cout << "Adding patient name (0010,0010) failed.\n";
  }

  DcmLongString* PatientIDElement = new DcmLongString(DCM_PatientID);
  PatientIDElement->putOFStringArray("0123456789");

  if (datasetPointer->insert(PatientIDElement, true, false).bad())
  {
    std::cout << "Adding patient ID (0010,0020) failed.\n";
  }

  DcmDate* PatientBirthDateElement = new DcmDate(DCM_PatientBirthDate);
  PatientBirthDateElement->setOFDate(OFDate(1972,6,26));

  if (datasetPointer->insert(PatientBirthDateElement, true, false).bad())
  {
    std::cout << "Adding patient birth date (0010,0030) failed.\n";
  }

  DcmCodeString* PatientSexElement = new DcmCodeString(DCM_PatientSex);
  PatientSexElement->putOFStringArray("M");

  if (datasetPointer->insert(PatientSexElement, true, false).bad())
  {
    std::cout << "Adding patient sex (0010,0040) failed.\n";
  }
}







ML_END_NAMESPACE

