//----------------------------------------------------------------------------------
//! The ML module class LoadU3D.
/*!
// \file    LoadU3D.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2015-10-16
//
// Loads U3D file and creates WEMs, Linesets and XMarkers
*/
//----------------------------------------------------------------------------------

// Local includes
#include "LoadU3D.h"

// Project includes
#include "../SaveU3D/U3DFileFormat/U3D_Constants.h"


using namespace std;


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(LoadU3D, WEMGenerator);


LoadU3D::LoadU3D (std::string type) : WEMGenerator(type)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  (_boundingBoxCenterFld = addVector3("boundingBoxCenter"))->setVector3Value(Vector3(0));
  (_boundingBoxRadiusFld = addFloat("boundingBoxRadius"))->setFloatValue(0.0f);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}


LoadU3D::~LoadU3D()
{
  // destroy own dynamic data structures here
}


void LoadU3D::handleNotification (Field* field)
{
  // ... field notification handling code

  // if a field change should lead to a new computation, use
  // the following:

  // if (field == 'myField'){
  //    if (_autoApplyFld->getBoolValue()){ // this auto apply field is provided by the base class
  //      ...
  //    }
  // }


  // call parent class and handle apply/autoApply and in/outputs
  WEMGenerator::handleNotification(field);
}


void LoadU3D::activateAttachments()
{
  // ... own initialization routines

  // call parent class
  WEMGenerator::activateAttachments();
}


void LoadU3D::_process()
{
  // for time measurement and mouse cursor setting.
  _startProcessing();

  WEMGenerator::_process();

  // Now call the own processing routine.
  myAlgorithm();

  // stop time measurement and mouse cursor resetting.
  _finishProcessing();

  // notify registered observer modules.
  _notifyObservers();
}


void LoadU3D::myAlgorithm()
{
  // If no other WEM input is explicitly declared,
  // the _outWEM is the WEM to work on.
  if (_outWEM)
  {
    //////////////////////////////////////////////////////////////
    // Please delete these comments below in your working code. //
    //////////////////////////////////////////////////////////////


//      // Example: Iteration over all nodes and translate them by their normal:
//
//      for (unsigned int i = 0; i < _outWEM->getNumWEMPatches(); i++){
//
//        WEMPatch* wemPatch = _outWEM->getWEMPatchAt(i);
//
//        const unsigned int numNodesInPatch = wemPatch->getNumNodes();
//
//        for (unsigned int j = 0; j < numNodesInPatch; j++){
//
//          WEMNode* node = wemPatch->getNodeAt(j);
//
//          node->setPosition(node->getPosition() + node->getNormal());
//        }
//      }
  }
}



void LoadU3D::_parseU3DFile(std::string filename)
{
  ifstream u3dFile;
  u3dFile.open(filename, ios::out | ios::binary);

  if (u3dFile.is_open()) 
  {
    U3DDataBlockFundamental blockType = _readU32(u3dFile);

    if (blockType == U3D_BLOCKTYPE_FILEHEADER)
    {
      // Get data block size
      U3DDataBlockFundamental blockDataSize = _readU32(u3dFile);

      // Get meta data block size
      /*U3DDataBlockFundamental blockMetaDataSize = */_readU32(u3dFile);

      // Calc meta data start position
      MLuint32 dataStartPos = (MLuint32)u3dFile.tellg();
      MLuint32 metaDataStartPos = dataStartPos + blockDataSize;

      if (blockDataSize % 4 != 0)
      {
        metaDataStartPos += (4 - (blockDataSize % 4));
      }

      // Go to meta data start position
      u3dFile.seekg(metaDataStartPos, ios::beg);

      // Get Key/Value Pair Count (9.2.6.1)
      U3DDataBlockFundamental keyValuePairCount = _readU32(u3dFile);

      for (int thisKeyValuePair = 0; thisKeyValuePair < keyValuePairCount; thisKeyValuePair++)
      {
        // Get Key/Value Pair Attributes (9.2.6.2)
        /*U3DDataBlockFundamental keyValuePairAttributes = */_readU32(u3dFile);

        string keyString   = _readString(u3dFile);
        string valueString = _readString(u3dFile);

        if (keyString == "BoundingBoxCenter") 
        {
          _boundingBoxCenterFld->setStringValue(valueString);
        }

        if (keyString == "BoundingBoxRadius") 
        {
          _boundingBoxRadiusFld->setStringValue(valueString);
        }

      }

    }

    u3dFile.close();

  }

}

//----------------------------------------------------------------------------------

MLuint8 LoadU3D::_readU8(ifstream& file)
{
  char* u8Buf = new char[1];

  file.read(u8Buf, 1);

  MLuint8 result = u8Buf[0];

  delete[] u8Buf;

  return result;
}

//----------------------------------------------------------------------------------

MLuint16 LoadU3D::_readU16(ifstream& file)
{
  char* u16Buf = new char[2];

  file.read(u16Buf, 2);

  MLuint16 byte1 = u16Buf[1] << 8;
  MLuint8  byte0 = u16Buf[0];

  MLuint16 result = byte1 | byte0;

  delete[] u16Buf;

  return result;
}

//----------------------------------------------------------------------------------

MLuint32 LoadU3D::_readU32(ifstream& file)
{
  char* u32Buf = new char[4];

  file.read(u32Buf, 4);

  MLuint32 byte3 = u32Buf[3] << 24;
  MLuint32 byte2 = u32Buf[2] << 16;
  MLuint32 byte1 = u32Buf[1] << 8;
  MLuint8  byte0 = u32Buf[0];

  MLuint32 result = byte3 | byte2 | byte1 | byte0;

  delete[] u32Buf;

  return result;
}

//----------------------------------------------------------------------------------

string LoadU3D::_readString(ifstream& file)
{
  string result = "";

  MLuint16 stringLength = _readU16(file);

  for (int i = 0; i < stringLength; i++)
  {
    MLuint8 stringCharacter = _readU8(file);

    result += stringCharacter;
  }

  return result;
}



ML_END_NAMESPACE