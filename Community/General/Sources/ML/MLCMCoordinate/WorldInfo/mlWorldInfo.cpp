//-----------------------------------------------------------------------
//
// Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR), 
// Departments of Radiology and Medical Informatics, Erasmus MC. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of BIGR nor the names of its contributors 
//       may be used to endorse or promote products derived from this software 
//       without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-----------------------------------------------------------------------
//! Defines and implements an operator showing
//! information about the input module.
/*!
// \file    mlWorldInfo.cpp
// \author  Wolf Spindler / Reinhard Hameeteman
// \date    03/2002 10/2008
*/
//======================================================
#ifndef __mlLinearAlgebra_H
#include <mlLinearAlgebra.h>
#endif
#ifndef __mlWorldInfo_H
#include "mlWorldInfo.h"
#endif
#ifndef __mlTools_H
#include "mlTools.h"
#endif


ML_START_NAMESPACE

  //-------------------------------------------------------------------
  // Macro to implement runtime system methods (see mlRuntimeSubClass.h)
  //-------------------------------------------------------------------
  ML_BASEOP_CLASS_SOURCE(WorldInfo, BaseOp)

  //-------------------------------------------------------------------
  // Constructor.
  //-------------------------------------------------------------------
  WorldInfo::WorldInfo(): BaseOp(1,0)
  {
    ML_TRACE_IN("WorldInfo::WorldInfo()")

    // Initialize all field pointer to NULL to avoid that
    // on creation handling of fields cause checks of invalid field pointers.
    _sizeXFld        = NULL;
    _sizeYFld        = NULL;
    _sizeZFld        = NULL;
    _sizeCFld        = NULL;
    _sizeTFld        = NULL;
    _sizeUFld        = NULL;
    _sizeFld         = NULL;

    _psizeXFld       = NULL;
    _psizeYFld       = NULL;
    _psizeZFld       = NULL;
    _psizeCFld       = NULL;
    _psizeTFld       = NULL;
    _psizeUFld       = NULL;
    _psizeFld        = NULL;
    
    _minFld          = NULL;
    _maxFld          = NULL;
    _dTypeFld        = NULL;
    _pImgInfoFld     = NULL;

    _matFld          = NULL;

    _x0Fld           = NULL;
    _y0Fld           = NULL;
    _z0Fld           = NULL;
    _a0Fld           = NULL;

    _x1Fld           = NULL;
    _y1Fld           = NULL;
    _z1Fld           = NULL;
    _a1Fld           = NULL;

    _x2Fld           = NULL;
    _y2Fld           = NULL;
    _z2Fld           = NULL;
    _a2Fld           = NULL;

    _x3Fld           = NULL;
    _y3Fld           = NULL;
    _z3Fld           = NULL;
    _a3Fld           = NULL;

    _voxelSizeFld   = NULL;
    
    _originXFld     = NULL;
    _originYFld     = NULL;
    _originZFld     = NULL;
    _originFld      = NULL;

    _extentXFld     = NULL;
    _extentYFld     = NULL;
    _extentZFld     = NULL;
    _extentFld      = NULL;

    _wSizeXFld      = NULL;
    _wSizeYFld      = NULL;
    _wSizeZFld      = NULL;
    _wSizeFld       = NULL;

    _centerXFld     = NULL;
    _centerYFld     = NULL;
    _centerZFld     = NULL;
    _centerFld      = NULL;
    
    _boxFld         = NULL;
    _alignToAxisFld  = NULL;

    ///TB 16.8.2001
    // Fields for C/T/U-Dim infos
    _cDimInfoFld     = NULL;
    _tDimInfoFld     = NULL;
    _uDimInfoFld     = NULL;

    ///TB 15.11.2001: Added table for time points info
    _tDimTableFld    = NULL;

    _updateFld       = NULL;
    _memImgFld       = NULL;
    _memImg2Fld      = NULL;
    _bypassIndexFld  = NULL;
    _inPlaceIndexFld = NULL;
    _opNameFld       = NULL;
    _inConnFld       = NULL;
    _inputValidFld   = NULL;

    (_sizeXFld       = getFieldContainer()->addInt("sizeX"))         ->setIntValue(0);
    (_sizeYFld       = getFieldContainer()->addInt("sizeY"))         ->setIntValue(0);
    (_sizeZFld       = getFieldContainer()->addInt("sizeZ"))         ->setIntValue(0);
    (_sizeCFld       = getFieldContainer()->addInt("sizeC"))         ->setIntValue(0);
    (_sizeTFld       = getFieldContainer()->addInt("sizeT"))         ->setIntValue(0);
    (_sizeUFld       = getFieldContainer()->addInt("sizeU"))         ->setIntValue(0);
    (_sizeFld        = getFieldContainer()->addVector("size"))       ->setStringValue("0 0 0 0 0 0");

    (_psizeXFld      = getFieldContainer()->addInt("pageSizeX"))     ->setIntValue(0);
    (_psizeYFld      = getFieldContainer()->addInt("pageSizeY"))     ->setIntValue(0);
    (_psizeZFld      = getFieldContainer()->addInt("pageSizeZ"))     ->setIntValue(0);
    (_psizeCFld      = getFieldContainer()->addInt("pageSizeC"))     ->setIntValue(0);
    (_psizeTFld      = getFieldContainer()->addInt("pageSizeT"))     ->setIntValue(0);
    (_psizeUFld      = getFieldContainer()->addInt("pageSizeU"))     ->setIntValue(0);
    (_psizeFld       = getFieldContainer()->addVector("pageSize"))   ->setStringValue("0 0 0 0 0 0");

    (_minFld         = getFieldContainer()->addDouble("minValue"))   ->setDoubleValue(0);
    (_maxFld         = getFieldContainer()->addDouble("maxValue"))   ->setDoubleValue(0);

    (_vXFld          = getFieldContainer()->addDouble("voxelSizeX")) ->setDoubleValue(0);
    (_vYFld          = getFieldContainer()->addDouble("voxelSizeY")) ->setDoubleValue(0);
    (_vZFld          = getFieldContainer()->addDouble("voxelSizeZ")) ->setDoubleValue(0);
    (_voxelSizeFld   = getFieldContainer()->addVec3f ("voxelSize"))  ->setStringValue("0 0 0");
    (_VoxelVolFld    = getFieldContainer()->addDouble("voxelVolume"))->setDoubleValue(0);

    //_vVolFld will contain the volume of a voxel in ml (not micro liter!)
    (_TotalVolFld    = getFieldContainer()->addDouble("totalVolume"))->setDoubleValue(0);
    (_MegaVoxelFld   = getFieldContainer()->addDouble("megaVoxels")) ->setDoubleValue(0);

    (_dTypeFld       = getFieldContainer()->addString("dataType"))   ->setStringValue("Unknown");
    (_pImgInfoFld    = getFieldContainer()->addString("pagedImgInfo"))->setStringValue("No Connection");

    (_typeInfoFld    = getFieldContainer()->addString("typeInfo"))   ->setStringValue("No Info");

#ifdef _ML_SHOW_EXTENDED_IMAGE_PROPERTIES
    (_imgPropExtFld  = getFieldContainer()->addString("imgPropExt"))->setStringValue("");
#endif

    mat4 m;
    (_matFld         = getFieldContainer()->addMatrix("worldMatrix"))->setMatrixValue(m);

    (_x0Fld          = getFieldContainer()->addDouble("a00"))        ->setDoubleValue(0);
    (_y0Fld          = getFieldContainer()->addDouble("a01"))        ->setDoubleValue(0);
    (_z0Fld          = getFieldContainer()->addDouble("a02"))        ->setDoubleValue(0);
    (_a0Fld          = getFieldContainer()->addDouble("a03"))        ->setDoubleValue(0);

    (_x1Fld          = getFieldContainer()->addDouble("a10"))        ->setDoubleValue(0);
    (_y1Fld          = getFieldContainer()->addDouble("a11"))        ->setDoubleValue(0);
    (_z1Fld          = getFieldContainer()->addDouble("a12"))        ->setDoubleValue(0);
    (_a1Fld          = getFieldContainer()->addDouble("a13"))        ->setDoubleValue(0);

    (_x2Fld          = getFieldContainer()->addDouble("a20"))        ->setDoubleValue(0);
    (_y2Fld          = getFieldContainer()->addDouble("a21"))        ->setDoubleValue(0);
    (_z2Fld          = getFieldContainer()->addDouble("a22"))        ->setDoubleValue(0);
    (_a2Fld          = getFieldContainer()->addDouble("a23"))        ->setDoubleValue(0);

    (_x3Fld          = getFieldContainer()->addDouble("a30"))        ->setDoubleValue(0);
    (_y3Fld          = getFieldContainer()->addDouble("a31"))        ->setDoubleValue(0);
    (_z3Fld          = getFieldContainer()->addDouble("a32"))        ->setDoubleValue(0);
    (_a3Fld          = getFieldContainer()->addDouble("a33"))        ->setDoubleValue(0);

    (_originXFld     = getFieldContainer()->addDouble("originX"))->setDoubleValue(0.0);
    (_originYFld     = getFieldContainer()->addDouble("originY"))->setDoubleValue(0.0);
    (_originZFld     = getFieldContainer()->addDouble("originZ"))->setDoubleValue(0.0);
    (_originFld      = getFieldContainer()->addVec3f ("origin" ))->setStringValue("0 0 0");

    (_extentXFld     = getFieldContainer()->addDouble("extentX"))->setDoubleValue(0.0);
    (_extentYFld     = getFieldContainer()->addDouble("extentY"))->setDoubleValue(0.0);
    (_extentZFld     = getFieldContainer()->addDouble("extentZ"))->setDoubleValue(0.0);
    (_extentFld      = getFieldContainer()->addVec3f ("extent" ))->setStringValue("0 0 0");

    (_wSizeXFld      = getFieldContainer()->addDouble("worldSizeX"))->setDoubleValue(0.0);
    (_wSizeYFld      = getFieldContainer()->addDouble("worldSizeY"))->setDoubleValue(0.0);
    (_wSizeZFld      = getFieldContainer()->addDouble("worldSizeZ"))->setDoubleValue(0.0);
    (_wSizeFld       = getFieldContainer()->addVec3f ("worldSize" ))->setStringValue("0 0 0");

    (_centerXFld     = getFieldContainer()->addDouble("centerX"))->setDoubleValue(0.0);
    (_centerYFld     = getFieldContainer()->addDouble("centerY"))->setDoubleValue(0.0);
    (_centerZFld     = getFieldContainer()->addDouble("centerZ"))->setDoubleValue(0.0);
    (_centerFld      = getFieldContainer()->addVec3f ("center" ))->setStringValue("0 0 0");

    (_boxFld         = getFieldContainer()->addSubImgBoxf("box")) ->setSubImgBoxfValue( SubImgBoxf() );
    (_alignToAxisFld = getFieldContainer()->addBool("alignToAxis"))->setBoolValue( true );
    (_useVoxelCenterFld = getFieldContainer()->addBool( "useVoxelCenter" ))->setBoolValue( false );
    (_includeAllVoxelsFld  = getFieldContainer()->addBool( "includeAllVoxels"))->setBoolValue( false );


    (_rotationFld    = (RotationField *) getFieldContainer()->addField ("rotation", "Rotation", ""))->setRotationValue( Rotation() );

    ///TB 16.8.2001
    // Fields for C/T/U-Dim infos
    (_cDimInfoFld    = getFieldContainer()->addString("cDimInfo"))    ->setStringValue("");
    (_tDimInfoFld    = getFieldContainer()->addString("tDimInfo"))    ->setStringValue("");
    (_uDimInfoFld    = getFieldContainer()->addString("uDimInfo"))    ->setStringValue("");

    ///TB 15.11.2001: Added table for time points info
    (_tDimTableFld   = getFieldContainer()->addString("tDimTable"))  ->setStringValue("");

    (_updateFld      = getFieldContainer()->addNotify("update"));
    (_memImgFld      = getFieldContainer()->addString("memoryImage"))   ->setStringValue("");
    (_memImg2Fld     = getFieldContainer()->addString("memoryImage2"))  ->setStringValue("");
    (_bypassIndexFld = getFieldContainer()->addInt   ("bypassIndex"))   ->setIntValue(-1);
    (_inPlaceIndexFld= getFieldContainer()->addInt   ("inPlaceIndex"))  ->setIntValue(-1);
    (_opNameFld      = getFieldContainer()->addString("moduleName"))    ->setStringValue("");
    (_inConnFld      = getFieldContainer()->addBool("connectionValid")) ->setBoolValue(false);
    (_inputValidFld  = getFieldContainer()->addBool("dataValid"))       ->setBoolValue(false);


    // Activate maximum support of all input voxel data types.
    setVoxelDataTypeSupport(BaseOp::MINIMUM_OPERATIONAL);
  }

  //-------------------------------------------------------------------------------
  // Update infos if input changes.
  //-------------------------------------------------------------------------------
  void WorldInfo::handleNotification(Field* field)
  {
    ML_TRACE_IN("WorldInfo::handleNotification()")

    // Any change on input or updateButton pressed?
    if ((field == (Field*)getInField(0)) || 
        (field == _updateFld     ) ||
        (field == _alignToAxisFld ) ||
        (field == _useVoxelCenterFld) ||
        (field == _includeAllVoxelsFld) )
      _updateInfos();
  } 

  //----------------------------------------------------------------------
  // Updates all fields to the new values.
  //----------------------------------------------------------------------
  void WorldInfo::_updateInfos()
  {
    ML_TRACE_IN("WorldInfo::_updateInfos()")

    Vector size(0,0,0,0,0,0);
    Vector psize(0,0,0,0,0,0);
    double min = 0;
    double max = 0;
    float  vx  = 0;
    float  vy  = 0;
    float  vz  = 0;
    vec3   imageWorldOrigin(0,0,0);
    vec3   imageWorldExtent(0,0,0);
    vec3   imageWorldSize(0,0,0);
    vec3   imageWorldCenter(0,0,0);
    SubImgBoxf imageWorldBox = SubImgBoxf();
    Rotation imageWorldRotation = Rotation();
    double  voxelvol  = 0;
    double  totalvol  = 0;
    double  megavoxel = 0;
    MLDataType dType = MLuint16Type;
    std::string pImgInfo = "No Connection";
    mat4 zeroMat=mat4::getMat(0.0);   /// TB 16.8.2001
    const mat4 *mat= &zeroMat;

    /// TB 16.8.2001:
    std::string dimInfo = "";
    std::vector<std::string> strVec;
    std::vector<DateTime> dtVec;
    char dtString[64] = "";
    unsigned int i=0;
    DateTime *refTime = 0;

    /// TB 15.11.200
    static const char *tTableHeader = "#@Absolute@Relative@Difference\n";
    std::string tTable = "";

    // Get valid input image if possible.
    PagedImg *inImg = getUpdatedInImg(0);

    // Check connection to input image. If a valid connection exists then update
    // size, data type, minimum and maximum value. Otherwise leave them unchanged.
    if (inImg){
      dType    = inImg->getDataType();
      pImgInfo = inImg->getStateInfoString();
      min      = inImg->getMinVoxelValue();
      max      = inImg->getMaxVoxelValue();
      size     = inImg->getImgExt();
      psize    = inImg->getPageExt();
      inImg->getVoxelSize(&vx, &vy, &vz);
      voxelvol = fabs (inImg->getToWorldMatrix()->det())/1000;
/*
      double vv = vx * vy * vz  / 1000;
      if(fabs(vv-voxelvol) > 0.001*(vv+voxelvol)/2)
        ML_PRINT_WARNING("WorldInfo::_updateInfos()", ML_EMPTY_MESSAGE, "Voxel volume != product of voxel sizes, image is non-orthogonal");
*/
      // Voxel volume in ml, voxels may not be orthogonal, ths use determinant to compute volume
      totalvol = size.x * size.y * size.z *voxelvol;
      // Total data volume in 3D in ml not counting multiple times / colors / modalities
      // = Imaged volume
      megavoxel = size.x * size.y * size.z * size.c * size.t * size.u / pow(1024,2.0);
      // Total number of voxels over all dimensions
      mat   = inImg->getToWorldMatrix();
    }
    else{
      // No valid image could be detected. So look for more details.
      if (getInOp(0)){
        // We have a connection and so we also have a connected PagedImg. Get its
        // state string to see why it is invalidated.
        pImgInfo = getInImg(0)->getStateInfoString();
      }
      else{
        // No connection.
        pImgInfo = MLGetErrorCodeDescription(ML_DISCONNECTED_GRAPH);
      }
    }

    // Update fields only if all fields are defined, i.e. if _inputValidFld
    // is defined, it's the last one created.
    if (_inputValidFld){
      // Update all output fields.
      _sizeXFld     ->setIntValue(size.x);
      _sizeYFld     ->setIntValue(size.y);
      _sizeZFld     ->setIntValue(size.z);
      _sizeCFld     ->setIntValue(size.c);
      _sizeTFld     ->setIntValue(size.t);
      _sizeUFld     ->setIntValue(size.u);
      _sizeFld      ->setVectorValue(size);

      _psizeXFld    ->setIntValue(psize.x);
      _psizeYFld    ->setIntValue(psize.y);
      _psizeZFld    ->setIntValue(psize.z);
      _psizeCFld    ->setIntValue(psize.c);
      _psizeTFld    ->setIntValue(psize.t);
      _psizeUFld    ->setIntValue(psize.u);
      _psizeFld     ->setVectorValue(psize);
      
      _minFld       ->setDoubleValue(min);
      _maxFld       ->setDoubleValue(max);
      
      _vXFld        ->setDoubleValue(vx);
      _vYFld        ->setDoubleValue(vy);
      _vZFld        ->setDoubleValue(vz);
      _voxelSizeFld ->setVec3fValue( vec3(vx,vy,vz) );
      
      _VoxelVolFld  ->setDoubleValue(voxelvol);
      _TotalVolFld  ->setDoubleValue(totalvol);
      _MegaVoxelFld ->setDoubleValue(megavoxel);
      _matFld       ->setMatrixValue(*mat);
      _dTypeFld     ->setStringValue(MLIsValidType(dType) ? MLNameFromDataType(dType) : "unknown");
      _pImgInfoFld  ->setStringValue(pImgInfo);

      // Update field with type information.
      _typeInfoFld->setStringValue((std::string)" @ @ \n" +  // empty line for not shown header
                                   Tools::getTypeInformation(inImg ? dType : -1).c_str());


#ifdef _ML_SHOW_EXTENDED_IMAGE_PROPERTIES
      // Get all names, DLL names and values and extended image properties as strings.
      std::string fullExtPropStr = "";
      if (inImg){
        // Get image property container
        const ImagePropertyExtensionContainer &extProps = inImg->getConstImagePropertyContainer();
        const size_t numEntries = extProps.getNumEntries();
        char numStr[64] = "";
        const int SepSize = 48;
        for (size_t c=0; c < numEntries; ++c){
          // Get type name and DLL name
          std::string name = extProps.getEntry(c)->getTypeId()->getName();
          std::string dllName = extProps.getEntry(c)->getTypeId()->getDllName();
          // A values may be large, thus separate and reformat it if string is 
          // larger than SepSize bytes.
          std::string valueSet = extProps.getEntry(c)->getValueAsString();
          int valSize = (int)valueSet.size();
          valSize = (valSize / SepSize) * SepSize;
          while (valSize > 0){
            valueSet.insert(valSize, "\n @ @");
            valSize -= SepSize;
          }

          snprintf(numStr, 63, "%u", c);
          fullExtPropStr += (std::string)
                            " @ @ \n" +  // empty line for not shown header
                            "Property #" + numStr + ": \n" +
                            "Name       @=@ " + name + "\n" +
                            "Defined in @=@ " + dllName + "\n" +
                            "Value      @=@ " + valueSet + "\n\n";
        }
      }
      _imgPropExtFld->setStringValue(fullExtPropStr);
#endif

      // Set world matrix values.    
      if (mat){
        _x0Fld->setDoubleValue((*mat)[0][0]);
        _y0Fld->setDoubleValue((*mat)[0][1]);
        _z0Fld->setDoubleValue((*mat)[0][2]);
        _a0Fld->setDoubleValue((*mat)[0][3]);

        _x1Fld->setDoubleValue((*mat)[1][0]);
        _y1Fld->setDoubleValue((*mat)[1][1]);
        _z1Fld->setDoubleValue((*mat)[1][2]);
        _a1Fld->setDoubleValue((*mat)[1][3]);

        _x2Fld->setDoubleValue((*mat)[2][0]);
        _y2Fld->setDoubleValue((*mat)[2][1]);
        _z2Fld->setDoubleValue((*mat)[2][2]);
        _a2Fld->setDoubleValue((*mat)[2][3]);

        _x3Fld->setDoubleValue((*mat)[3][0]);
        _y3Fld->setDoubleValue((*mat)[3][1]);
        _z3Fld->setDoubleValue((*mat)[3][2]);
        _a3Fld->setDoubleValue((*mat)[3][3]);
      }

      if (inImg){
        // Get World Info

        double begin = 0.0;
        if ( _useVoxelCenterFld->getBoolValue() ) {begin = 0.5;}
        
        double end = 1.0;
        if ( _includeAllVoxelsFld->getBoolValue() ) { begin = 0.0; end = 0.0; }

        // Get rotation
        mat4 rotationMatrix = *mat;
        Rotation imageWorldRotation;
        vec3 scale;
        // remove scale
        for (int i = 0; i < 3; ++i)
        {
          scale[i] = vec3( rotationMatrix[0][i], rotationMatrix[1][i], rotationMatrix[2][i] ).length();
          if (scale[i]) {
            for (int j = 0; j < 3; ++j) {
              rotationMatrix[j][i] /= scale[i];
            }
          }
        }
        double det =  rotationMatrix.det();
        // Set rotation field only if there is a valid rotation matrix defined
        if ( MLAbs(det - 1) < 0.0001 ){
          imageWorldRotation = Rotation(rotationMatrix);
        } else {
          imageWorldRotation =  Rotation();
        }

        vec3 corner[8];
        inImg->transformToWorldCoord( vec3(begin,begin,begin),                                  corner[0] );
        inImg->transformToWorldCoord( vec3(begin,size.y-end+begin,begin),                       corner[1] );
        inImg->transformToWorldCoord( vec3(begin,begin,size.z-end+begin),                       corner[2] );
        inImg->transformToWorldCoord( vec3(begin,size.y-end+begin,size.z-end+begin),            corner[3] );
        inImg->transformToWorldCoord( vec3(size.x-end+begin,begin,begin),                       corner[4] );
        inImg->transformToWorldCoord( vec3(size.x-end+begin,size.y-1+begin,begin),              corner[5] );
        inImg->transformToWorldCoord( vec3(size.x-end+begin,begin,size.z-end+begin),            corner[6] );
        inImg->transformToWorldCoord( vec3(size.x-end+begin,size.y-end+begin,size.z-end+begin), corner[7] );
        if ( _alignToAxisFld->getBoolValue() ){
          imageWorldOrigin = corner[0];
          imageWorldExtent = corner[0];
          for (unsigned int iCorner=1; iCorner < 8; ++iCorner ){
            imageWorldOrigin.compMin(corner[iCorner]);
            imageWorldExtent.compMax(corner[iCorner]);
          }
          imageWorldSize   = (imageWorldExtent - imageWorldOrigin);
        } else {
          //This works only if there is no rotation in the world matrix
         imageWorldOrigin = corner[0];
         imageWorldExtent = corner[7];
         imageWorldSize[0] = (corner[4]-corner[0]).length();
         imageWorldSize[1] = (corner[1]-corner[0]).length();
         imageWorldSize[2] = (corner[2]-corner[0]).length();
        }
        imageWorldCenter = 0.5*(imageWorldExtent + imageWorldOrigin);
        imageWorldBox = SubImgBoxf( vec6(imageWorldOrigin,0.0,0.0,0.0), 
                                    vec6(imageWorldExtent,0.0,0.0,0.0) );



        _originXFld   ->setDoubleValue( imageWorldOrigin[0] );
        _originYFld   ->setDoubleValue( imageWorldOrigin[1] );
        _originZFld   ->setDoubleValue( imageWorldOrigin[2] );
        _originFld    ->setVec3fValue(  imageWorldOrigin    );
        _extentXFld   ->setDoubleValue( imageWorldExtent[0] );
        _extentYFld   ->setDoubleValue( imageWorldExtent[1] );
        _extentZFld   ->setDoubleValue( imageWorldExtent[2] );
        _extentFld    ->setVec3fValue(  imageWorldExtent    );
        _wSizeXFld    ->setDoubleValue( imageWorldSize[0]   );
        _wSizeYFld    ->setDoubleValue( imageWorldSize[1]   );
        _wSizeZFld    ->setDoubleValue( imageWorldSize[2]   );
        _wSizeFld     ->setVec3fValue(  imageWorldSize      );
        _centerXFld   ->setDoubleValue( imageWorldCenter[0] );
        _centerYFld   ->setDoubleValue( imageWorldCenter[1] );
        _centerZFld   ->setDoubleValue( imageWorldCenter[2] );
        _centerFld    ->setVec3fValue(  imageWorldCenter    );
        _boxFld       ->setSubImgBoxfValue( imageWorldBox );
        _rotationFld  ->setRotationValue ( imageWorldRotation );


        // Get memory image state and size.
        MemoryImg *memImg = &(inImg->getMemoryImg());
        Vector loc =memImg->getImg().getBox().v1;
        Vector size=memImg->getImg().getImgExt();

        // Print 64 bit number of voxels platform independently.
        char str[512]="";
        snprintf(str, 511, "%s, %s=%1.0lf Voxel at %s, ",
                 memImg->isValid() ? "Valid" : "Invalid",
                 size.print("(", ",", ")").c_str(), 
                 (MLdouble)(size.compMul()), 
                 loc.print("(", ",", ")").c_str());
        _memImgFld->setStringValue(str);
        _memImg2Fld->setStringValue(memImg->getManualControl() ? "handled by operator" : "handled by host");
        _bypassIndexFld->setIntValue(inImg->getBypassIndex());
        _inPlaceIndexFld->setIntValue(inImg->getInPlaceIndex());
      }
      else{
        // Reset values
        _originXFld   ->setDoubleValue( 0.0    );
        _originYFld   ->setDoubleValue( 0.0    );
        _originZFld   ->setDoubleValue( 0.0    );
        _originFld    ->setVec3fValue(  vec3() );
        _extentXFld   ->setDoubleValue( 0.0    );
        _extentYFld   ->setDoubleValue( 0.0    );
        _extentZFld   ->setDoubleValue( 0.0    );
        _extentFld    ->setVec3fValue(  vec3() );
        _wSizeXFld    ->setDoubleValue( 0.0    );
        _wSizeYFld    ->setDoubleValue( 0.0    );
        _wSizeZFld    ->setDoubleValue( 0.0    );
        _wSizeFld     ->setVec3fValue(  vec3() );
        _centerXFld   ->setDoubleValue( 0.0    );
        _centerYFld   ->setDoubleValue( 0.0    );
        _centerZFld   ->setDoubleValue( 0.0    );
        _centerFld    ->setVec3fValue(  vec3() );
        _boxFld       ->setSubImgBoxfValue( SubImgBoxf() );

        _memImgFld->setStringValue("Invalid");
        _memImg2Fld->setStringValue("");
        _bypassIndexFld->setIntValue(-1);
        _inPlaceIndexFld->setIntValue(-1);
      }

      // Get name of really connected input operator.
      BaseOp *inOp = getInOp(0, true);
      _opNameFld->setStringValue(inOp ? inOp->getTypeIdName() : "");

      // Get input connector validity.
      _inConnFld->setBoolValue(inOp != NULL);

      // Get input validity.
      _inputValidFld->setBoolValue(getUpdatedInImg(0) != 0);

       /// TB 16.8.2001, 15.11.2001:
      // Set C/T/U-dim info fields, added time table
      if (inImg){
        dimInfo = "";
        strVec = inImg->getCDimInfos();
        for (i = 0; i < strVec.size(); i++)
        {
          if (i)
            dimInfo += " / ";
          dimInfo += strVec[i];
        }
        _cDimInfoFld->setStringValue(dimInfo);

        dimInfo = "";
        tTable = tTableHeader;
        dtVec = inImg->getTDimInfos();
        refTime = 0;
        for (i = 0; i < dtVec.size(); i++)
        {
          if (i)
          {
            dimInfo += " / ";
            tTable += '\n';
          }
          sprintf(dtString, "%d@", i);
          tTable += dtString;
          dtVec[i].get(dtString, "%Y-%m-%d %H:%M:");
          sprintf(dtString+strlen(dtString), "%06.3f", dtVec[i].second());
          (tTable += dtString) += '@';

          if (refTime)
          {
            sprintf(dtString, "+%.3f", dtVec[i].secondsSince(*refTime));
            (tTable += dtString+1) += '@';
            dimInfo += dtString;
          }
          else
          {
            tTable += "-@";
            dimInfo += dtString;
            refTime = &dtVec[i];
          }

          if (i)
          {
            sprintf(dtString, "%.3f", dtVec[i].secondsSince(dtVec[i-1]));
            tTable += dtString;
          }
          else
            tTable += '-';
        }
        _tDimInfoFld->setStringValue(dimInfo);
        _tDimTableFld->setStringValue(tTable);

        dimInfo = "";
        strVec = inImg->getUDimInfos();
        for (i = 0; i < strVec.size(); i++)
        {
          if (i)
            dimInfo += " / ";
          dimInfo += strVec[i];
        }
        _uDimInfoFld->setStringValue(dimInfo);
      }
      else
      {
        _cDimInfoFld->setStringValue("");
        _tDimInfoFld->setStringValue("");
        _tDimTableFld->setStringValue(tTableHeader);
        _uDimInfoFld->setStringValue("");
      }
    }
  }

ML_END_NAMESPACE


