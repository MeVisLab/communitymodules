//----------------------------------------------------------------------------------
//! This header files contains some macro definitions
/*! 
//
// \file    PRC_Macros.h
// \author  Axel Newe
// \date    2015-08-01
*/


#ifndef _PRC_Macros_H
#define _PRC_Macros_H

// Functions
#define WriteUnsignedInteger( value )               pbs << (MLuint32)(value);
#define WriteInteger( value )                       pbs << (int32_t)(value);
#define WriteCharacter( value )                     pbs << (MLuint8)(value);
#define WriteDouble( value )                        pbs << (double)(value);
#define WriteBit( value )                           pbs << (bool)(value);
#define WriteBoolean( value )                       pbs << (bool)(value);
#define WriteString( value )                        pbs << (value);
#define SerializeContentPRCBase                     serializeContentPRCBase(pbs);
#define SerializeGraphics                           serializeGraphics(pbs);
#define SerializePRCBaseWithGraphics                { serializeContentPRCBase(pbs); serializeGraphics(pbs); }
#define SerializeRepresentationItemContent          serializeRepresentationItemContent(pbs);
#define SerializeRepresentationItem( value )        (value)->serializeRepresentationItem(pbs);
#define SerializeMarkup( value )                    (value).serializeMarkup(pbs);
#define SerializeReferenceUniqueIdentifier( value ) (value).serializeReferenceUniqueIdentifier(pbs);
#define SerializeContentBaseTessData                serializeContentBaseTessData(pbs);
#define SerializeTessFace( value )                  (value)->serializeTessFace(pbs);
#define SerializeUserData                           PRC_UserData(0,0).write(pbs);
#define SerializeLineAttr( value )                  pbs << (MLuint32)((value)+1);
#define SerializeVector3d( value )                  (value).serializeVector3d(pbs);
#define SerializeVector2d( value )                  (value).serializeVector2d(pbs);
#define SerializeName( value )                      writeName(pbs, (value));
#define SerializeInterval( value )                  (value).serializeInterval(pbs);
#define SerializeDomain( value )                    (value).serializeDomain(pbs);
#define SerializeParameterization                   serializeParameterization(pbs);
#define SerializeUVParameterization                 serializeUVParameterization(pbs);
#define SerializeTransformation                     serializeTransformation(pbs);
#define SerializeBaseTopology                       serializeBaseTopology(pbs);
#define SerializeBaseGeometry                       serializeBaseGeometry(pbs);
#define SerializePtrCurve( value )                  { WriteBoolean( false ); if((value)==NULL) pbs << (MLuint32)PRC_TYPE_ROOT; else (value)->serializeCurve(pbs); }
#define SerializePtrSurface( value )                { WriteBoolean( false ); if((value)==NULL) pbs << (MLuint32)PRC_TYPE_ROOT; else (value)->serializeSurface(pbs); }
#define SerializePtrTopology( value )               { WriteBoolean( false ); if((value)==NULL) pbs << (MLuint32)PRC_TYPE_ROOT; else (value)->serializeTopoItem(pbs); }
#define SerializeContentCurve                       serializeContentCurve(pbs);
#define SerializeContentWireEdge                    serializeContentWireEdge(pbs);
#define SerializeContentBody                        serializeContentBody(pbs);
#define SerializeTopoContext                        serializeTopoContext(pbs);
#define SerializeContextAndBodies( value )          (value).serializeContextAndBodies(pbs);
#define SerializeBody( value )                      (value)->serializeBody(pbs);
#define ResetCurrentGraphics                        resetGraphics();
#define SerializeContentSurface                     serializeContentSurface(pbs);
#define SerializeCompressedUniqueId( value )        (value).serializeCompressedUniqueId(pbs);
#define SerializeUnit( value )                      (value).serializeUnit(pbs);
#define SerializeBoundingBox                        serializeBoundingBox(pbs);
#define SerializeAttributeEntry                     serializeAttributeEntry(pbs);
#define SerializeContentSingleAttribute( value )    (value).serializeSingleAttribute(pbs);
#define SerializeAttribute( value )                 (value).serializeAttribute(pbs);
#define SerializeAttributeData                      serializeAttributes(pbs);
#define WriteUncompressedUnsignedInteger( value )   writeUncompressedUnsignedInteger(out, (MLuint32)(value));


#define SerializeFileStructureUncompressedUniqueId( value )               (value).serializeFileStructureUncompressedUniqueId(out);
#define WriteUnsignedIntegerWithVariableBitNumber( value, bit_number )    writeUnsignedIntegerWithVariableBitNumber( pbs, (value), (bit_number) );
#define WriteIntegerWithVariableBitNumber( value, bit_number )            writeIntegerWithVariableBitNumber( pbs, (value), (bit_number) );
#define WriteDoubleWithVariableBitNumber( value, bit_number )             writeDoubleWithVariableBitNumber( pbs, (value), (bit_number) );
#define WriteNumberOfBitsThenUnsignedInteger( value )                     writeNumberOfBitsThenUnsignedInteger( pbs, value );
#define SerializeCompressedFace( value )                                  (value)->serializeCompressedFace( pbs, brep_data_compressed_tolerance );






#endif // _PRC_Macros_H
