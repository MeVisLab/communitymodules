//----------------------------------------------------------------------------------
//
// Copyright 2001-2004, MeVis gGmbH
// ALL RIGHTS RESERVED
//
// THE CONTENT OF THIS WORK CONTAINS CONFIDENTIAL AND PROPRIETARY
// INFORMATION OF MEVIS GGMBH. ANY DUPLICATION, MODIFICATION,
// DISTRIBUTION, OR DISCLOSURE IN ANY FORM, IN WHOLE, OR IN PART, IS STRICTLY
// PROHIBITED WITHOUT THE PRIOR EXPRESS WRITTEN PERMISSION OF MEVIS GGMBH
//
//----------------------------------------------------------------------------------
//! The ML module class kAviWriter.
/*!
// \file    mlkAviWriter.cpp
// \author  okonrad, extended version: konrad mühler
// \date    2005-07-21
//
// The kAviWriter takes an arbitrary number of images and compresses them into a movie.
*/
// Last changed by $Author: okonrad $
// $Id: mlkAviWriter.cpp 58874 2007-07-19 12:18:13Z okonrad $
// $Source$
//----------------------------------------------------------------------------------


// Local includes
#ifndef __mlkAviWriter_H
#include "mlkAviWriter.h"
#endif


#include "mlUnicode.h"


ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(kAviWriter, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
kAviWriter::kAviWriter (void) : BaseOp(1, 0)
{
    ML_TRACE_IN("kAviWriter::kAviWriter()")

    handleNotificationOff();

    (_enableRecordingFld = getFieldContainer()->addBool  ("enableRecording"))->setBoolValue(false);

    (_outputFilenameFld  = getFieldContainer()->addString("outputFilename")) ->setStringValue("");
    (_codecCodeFld       = getFieldContainer()->addString("codecCode"))      ->setStringValue("cvid");
    (_framesPerSecondFld = getFieldContainer()->addInt   ("framesPerSecond"))->setIntValue(24);
    (_numFramesFld       = getFieldContainer()->addInt   ("numFrames"))      ->setIntValue(0);

    (_messageFld         = getFieldContainer()->addString("message"))        ->setStringValue("");
    (_availableCodecsFld = getFieldContainer()->addString("availableCodecs"))->setStringValue("");

    _triggerResetFld     = getFieldContainer()->addNotify("triggerReset");
    _triggerMakeAviFld   = getFieldContainer()->addNotify("triggerMakeAvi");

#ifdef WIN32

    _codecName[0] = 'c';
    _codecName[1] = 'v';
    _codecName[2] = 'i';
    _codecName[3] = 'd';

    _avi     = NULL;
    _hbitmap = NULL;
    _bits    = NULL;

    _imgWidth  = 0;
    _imgHeight = 0;

#else
    _messageFld->setStringValue("Module available only under Windows!");	
#endif

    handleNotificationOn();
}


kAviWriter::~kAviWriter()
{
    ML_TRACE_IN("kAviWriter::~kAviWriter()")

#ifdef WIN32
    if (_avi){
        CloseAvi(_avi);
        _avi = NULL;
    }

    if (_hbitmap){
        DeleteObject(_hbitmap);
    }
#endif // ifdef WIN32
}

void kAviWriter::activateAttachments()
{
    BaseOp::activateAttachments();
}


//----------------------------------------------------------------------------------
//! Handle field changes of the field \c field.
//----------------------------------------------------------------------------------
void kAviWriter::handleNotification (Field *field)
{
    ML_TRACE_IN("kAviWriter::handleNotification()")

#ifdef WIN32

    if (field == _codecCodeFld){
        _checkCodec();
    } else
    if (field == getInField(0)){

		//std::cout << "AVI in image changed" << std::endl;
        // check if the input image is valid
        if (getUpdatedInImg(0)){

            Vector imgExt = getUpdatedInImg(0)->getImgExt();

            bool canAddImage = true;

            // expect a colored image
            _isGreyValueImage = false;

            std::string msg;

            // check if everything else is ok
            if (imgExt.z > 1){
                msg.append("Bad image extends. Check z-dimension! (must be 1)\n");
                canAddImage = false;
            }
            if (getUpdatedInImg(0)->getDataType() != MLuint8Type){
                msg.append("Bad image datatype. Datatype must be of uint8!\n");
                canAddImage = false;
            }

            if (imgExt.c == 1){
                _isGreyValueImage = true;
            }

            if (canAddImage) {
                _addImageToMovie();
            } else {
                _messageFld->setStringValue(msg.c_str());
            }

        } else {
            _messageFld->setStringValue("No valid input image.");
            _numFramesFld->setIntValue(0);
            _avi = NULL;
        }

    } else
    if (field == _triggerResetFld){
        _endRecording();

        _messageFld->setStringValue("Recording reset. All frames are discarded.");
        _numFramesFld->setIntValue(0);
    } else
    if (field == _triggerMakeAviFld){
        _endRecording();
    } else
    if (field == _enableRecordingFld){
        if (_enableRecordingFld->getBoolValue()){
            _messageFld->setStringValue("Recording enabled.");
        } else {
            _messageFld->setStringValue("Recording disabled");
        }
    }
	else if (field == _messageFld)
	{
		std::cout << _messageFld->getStringValue() << std::endl;
	}
#endif // ifdef WIN32
}

#ifdef WIN32

void kAviWriter::_checkCodec()
{
    ML_TRACE_IN("kAviWriter::_checkCodec()")

    int i;
    ICINFO info;

    bool codecAvailable = false;

    const std::string proposedCodec = _codecCodeFld->getStringValue();

    std::string msg;

    msg.append("Available codecs on current system:\n\n");

    // test all available codecs on the system against the proposed codec
    for (i = 0; ICInfo(ICTYPE_VIDEO, i , &info); i++){
        char* codec = (char*)(&info.fccHandler);

        if ((proposedCodec.c_str()[0] == codec[0]) &&
            (proposedCodec.c_str()[1] == codec[1]) &&
            (proposedCodec.c_str()[2] == codec[2]) &&
            (proposedCodec.c_str()[3] == codec[3]))
        {
            codecAvailable |= true;
        }

        msg.append(codec);
        msg.append("\n");
    }

    // set available codecs to field, so user can read this information
    _availableCodecsFld->setStringValue(msg.c_str());


    if (!codecAvailable){

        std::string msg;
        msg.append("Codec not available: ");
        msg.append(proposedCodec);
        msg.append("\nSetting default codec: cvid");

        _messageFld->setStringValue(msg.c_str());

        // set standard codec
        _codecCodeFld->setStringValue("cvid");
    } else {
        std::string msg;
        msg.append("Codec set to ");
        msg.append((char*)(_codecCodeFld->getStringValue()).c_str());
        _messageFld->setStringValue(msg.c_str());
    }

    _codecName[0] = proposedCodec.c_str()[0];
    _codecName[1] = proposedCodec.c_str()[1];
    _codecName[2] = proposedCodec.c_str()[2];
    _codecName[3] = proposedCodec.c_str()[3];
}


void kAviWriter::_startRecording(int imgWidth, int imgHeight)
{
    ML_TRACE_IN("kAviWriter::_startRecording()")

    _messageFld->setStringValue("");

    // checks for availability of demanded codec
    // and sets internal codec code variable
    _checkCodec();
	
	if ((imgWidth==0) || (imgHeight==0))
	{
		Vector imgExt = getUpdatedInImg(0)->getImgExt();
		_imgWidth  = imgExt.x;
		_imgHeight = imgExt.y;
	}
	else
	{
		_imgWidth  = imgWidth;
		_imgHeight = imgHeight;
	}

    // check if there is a valid image extension at all
    if ((_imgWidth == 0) ||(_imgHeight == 0)){

        std::string msg;

        if (_imgWidth == 0){
            msg.append("Image width is zero!\n");
        }
        if (_imgHeight == 0){
            msg.append("Image height is zero!\n");
        }

        msg.append("AVI will not be recorded.");
        _messageFld->setStringValue(msg.c_str());
        _avi = NULL;
    } else
    // check if the image width and height is a multiple of 4
    if (((_imgWidth % 4) == 0) && ((_imgHeight % 4) == 0)){

        if (!_outputFilenameFld->getStringValue().length()){
            _messageFld->setStringValue("No file name given!");
        } else {

            MLuint16* unicodeFilename = MLConvertUTF8ToUTF16((_outputFilenameFld->getStringValue()).c_str());

            if (!_avi){
                _avi = CreateAvi((const WCHAR*)unicodeFilename, 1000/_framesPerSecondFld->getIntValue(), NULL);
            }

            MLFree(unicodeFilename);

            // configure header

            BITMAPINFO bitmapInfo;
            HDC        hdc;

            ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));

            BITMAPINFOHEADER &bitmapInfoHeader = bitmapInfo.bmiHeader;
            bitmapInfoHeader.biSize            = sizeof(bitmapInfoHeader);
            bitmapInfoHeader.biWidth           = _imgWidth;
            bitmapInfoHeader.biHeight          = _imgHeight;
            bitmapInfoHeader.biPlanes          =  1;
            bitmapInfoHeader.biBitCount        = 24;
            bitmapInfoHeader.biCompression     = BI_RGB;
            bitmapInfoHeader.biSizeImage       = ((bitmapInfoHeader.biWidth * bitmapInfoHeader.biBitCount / 8 + 3) & 0xFFFFFFFC) * bitmapInfoHeader.biHeight;
            bitmapInfoHeader.biXPelsPerMeter   = 10000;
            bitmapInfoHeader.biYPelsPerMeter   = 10000;
            bitmapInfoHeader.biClrUsed         = 0;
            bitmapInfoHeader.biClrImportant    = 0;

            HDC hdcscreen = GetDC(0);

            hdc = CreateCompatibleDC(hdcscreen);
            ReleaseDC(0, hdcscreen);

            _hbitmap = CreateDIBSection(hdc, (BITMAPINFO*) &bitmapInfoHeader, DIB_RGB_COLORS, &_bits, NULL, NULL);

            AVICOMPRESSOPTIONS opts;
            ZeroMemory(&opts, sizeof(opts));
            opts.fccHandler = mmioFOURCC(_codecName[0], _codecName[1], _codecName[2], _codecName[3]);
            opts.dwFlags   |= AVICOMPRESSF_VALID;

            if (SetAviVideoCompression(_avi, _hbitmap, &opts, false, NULL) != AVIERR_OK){
                std::string msg;
                msg.append("Unable to set AVI compression options. Please check codec!");
                _messageFld->setStringValue(msg.c_str());
            } else{
                _messageFld->setStringValue("");
            }
        }
    } else {
        std::string msg;
        msg.append("Input image's width or height is not a multiple of 4!\n");
        msg.append("AVI will not be recorded.");
        _messageFld->setStringValue(msg.c_str());

        _avi= NULL;
    }
}


/*void kAviWriter::_startRecording2(int imgWidth, int imgHeight)
{
    ML_TRACE_IN("kAviWriter::_startRecording2()")

    _messageFld->setStringValue("");

    // checks for availability of demanded codec
    // and sets internal codec code variable
    _checkCodec();

    //Vector imgExt = getUpdatedInImg(0)->getImgExt();

    _imgWidth  = imgWidth;
    _imgHeight = imgHeight;

    // check if there is a valid image extension at all
    if ((_imgWidth == 0) ||(_imgHeight == 0)){

        std::string msg;

        if (_imgWidth == 0){
            msg.append("Image width is zero!\n");
        }
        if (_imgHeight == 0){
            msg.append("Image height is zero!\n");
        }

        msg.append("AVI will not be recorded.");
        _messageFld->setStringValue(msg.c_str());
        _avi = NULL;
    } else
    // check if the image width and height is a multiple of 4
    if (((_imgWidth % 4) == 0) && ((_imgHeight % 4) == 0)){

        if (!_outputFilenameFld->getStringValue().length()){
            _messageFld->setStringValue("No file name given!");
        } else {

            MLuint16* unicodeFilename = MLConvertUTF8ToUTF16((_outputFilenameFld->getStringValue()).c_str());

            if (!_avi){
                _avi = CreateAvi((const WCHAR*)unicodeFilename, 1000/_framesPerSecondFld->getIntValue(), NULL);
            }

            MLFree(unicodeFilename);

            // configure header

            BITMAPINFO bitmapInfo;
            HDC        hdc;

            ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));

            BITMAPINFOHEADER &bitmapInfoHeader = bitmapInfo.bmiHeader;
            bitmapInfoHeader.biSize            = sizeof(bitmapInfoHeader);
            bitmapInfoHeader.biWidth           = _imgWidth;
            bitmapInfoHeader.biHeight          = _imgHeight;
            bitmapInfoHeader.biPlanes          =  1;
            bitmapInfoHeader.biBitCount        = 24;
            bitmapInfoHeader.biCompression     = BI_RGB;
            bitmapInfoHeader.biSizeImage       = ((bitmapInfoHeader.biWidth * bitmapInfoHeader.biBitCount / 8 + 3) & 0xFFFFFFFC) * bitmapInfoHeader.biHeight;
            bitmapInfoHeader.biXPelsPerMeter   = 10000;
            bitmapInfoHeader.biYPelsPerMeter   = 10000;
            bitmapInfoHeader.biClrUsed         = 0;
            bitmapInfoHeader.biClrImportant    = 0;

            HDC hdcscreen = GetDC(0);

            hdc = CreateCompatibleDC(hdcscreen);
            ReleaseDC(0, hdcscreen);

            _hbitmap = CreateDIBSection(hdc, (BITMAPINFO*) &bitmapInfoHeader, DIB_RGB_COLORS, &_bits, NULL, NULL);

            AVICOMPRESSOPTIONS opts;
            ZeroMemory(&opts, sizeof(opts));
            opts.fccHandler = mmioFOURCC(_codecName[0], _codecName[1], _codecName[2], _codecName[3]);
            opts.dwFlags   |= AVICOMPRESSF_VALID;

            if (SetAviVideoCompression(_avi, _hbitmap, &opts, false, NULL) != AVIERR_OK){
                std::string msg;
                msg.append("Unable to set AVI compression options. Please check codec!");
                _messageFld->setStringValue(msg.c_str());
            } else{
                _messageFld->setStringValue("");
            }
        }
    } else {
        std::string msg;
        msg.append("Input image's width or height is not a multiple of 4!\n");
        msg.append("AVI will not be recorded.");
        _messageFld->setStringValue(msg.c_str());

        _avi= NULL;
    }
}*/

void kAviWriter::_addImageToMovie()
{
    ML_TRACE_IN("kAviWriter::_addImageToMovie()")

    if (_enableRecordingFld->getBoolValue()){

        // make header if this is the first frame
        if (_numFramesFld->getIntValue() == 0){
            _startRecording();
        }

        if (_avi){

            Vector imgExt = getUpdatedInImg(0)->getImgExt();

            if ((imgExt.x == _imgWidth) && (imgExt.y == _imgHeight)){
                if (((imgExt.x % 4) == 0) && ((imgExt.y % 4) == 0)){

                    // read out input image and write output image into avi

                    int x, y;

                    // get input image as an array
                    void* imgData = NULL;

                    SubImgBox imageBox(imgExt); // get the whole image

                    getTile(getInOp(0), getInOpIndex(0), imageBox, MLuint8Type, &imgData);

                    MLuint8* iData = (MLuint8*)imgData;

                    // since we have only images with
                    // a z-ext of 1, we can compute the c stride as follows
                    int cStride = _imgWidth * _imgHeight;

                    int offset  = 0;

                    MLuint8 r=0, g=0, b=0;

                    // pointer into the bitmap that is
                    // used to write images into the avi
                    UCHAR* dst = (UCHAR*)_bits;

                    for (y = _imgHeight-1; y >= 0; y--){ // reversly scan the image. if y-rows of DIB are set in normal order, no compression will be available.

                        offset = _imgWidth * y;

                        for (x = 0; x < _imgWidth; x++){


                            if (_isGreyValueImage){
                                r = iData[offset + x];

                                *dst++ = (UCHAR)r;
                                *dst++ = (UCHAR)r;
                                *dst++ = (UCHAR)r;

                            } else {
                                b = iData[offset + x]; // windows bitmap need reverse order: bgr instead of rgb
                                g = iData[offset + x + cStride          ];
                                r = iData[offset + x + cStride + cStride];

                                *dst++ = (UCHAR)r;
                                *dst++ = (UCHAR)g;
                                *dst++ = (UCHAR)b;
                            }
                            // alpha channel in input image is ignored
                        }
                    }

                    if (AddAviFrame(_avi, _hbitmap) != AVIERR_OK){
                        CloseAvi(_avi);
                        _avi = NULL;
                        _messageFld->setStringValue("Could not add frame to AVI. Closing avi");
                        _numFramesFld->setIntValue(0);
                    } else {
                        _numFramesFld->setIntValue(_numFramesFld->getIntValue() + 1);
                        _messageFld->setStringValue("Frame added to AVI.");
                    }

                    if (imgData){
                        freeTile(imgData);
                    }
                }else {
                    std::string msg;
                    msg.append("Input image's width or height is not a multiple of 4!\n");
                    msg.append("Skipping this frame.");
                    _messageFld->setStringValue(msg.c_str());
                }
            } else {

                std::string msg;
                msg.append("Input image's size has differed!\n");
                msg.append("Skipping this frame.");
                _messageFld->setStringValue(msg.c_str());
            }
        }
    }
}


void kAviWriter::_endRecording()
{
    ML_TRACE_IN("kAviWriter::_endRecording()")

    if (_numFramesFld->getIntValue() == 0){
        _messageFld->setStringValue("Empty AVI, nothing saved.");
    } else {
        _messageFld->setStringValue("AVI written to disk.");
        _numFramesFld->setIntValue(0);
    }

    if (_hbitmap){ DeleteObject(_hbitmap); }

    CloseAvi(_avi);
    _avi = NULL;
}



void kAviWriter::addImageToMovieByData(unsigned char* data, int imgWidth, int imgHeight)
{
    ML_TRACE_IN("kAviWriter::addImageToMovieByData()")

    // make header if this is the first frame
    if (_numFramesFld->getIntValue() == 0){
        _startRecording(imgWidth, imgHeight);
    }

    if (_avi)
	{
        if ((imgWidth == _imgWidth) && (imgHeight == _imgHeight))
		{
            if (((imgWidth % 4) == 0) && ((imgHeight % 4) == 0))
			{
                // read out input image and write output image into avi

                int x, y;
                // get input image as an array
                //void* imgData = NULL;

                // pointer into the bitmap that is
                // used to write images into the avi
                UCHAR* dst = (UCHAR*)_bits;
				
				int iIter = 0;
                
				for (y = 0; y < _imgHeight; y++)
				{
                    for (x = 0; x < _imgWidth; x++)
					{
						//val = ((long)data[iIter+2] << 16) | ((long)data[iIter+1] << 8) | ((long)data[iIter]);

/*                            b = data[offset + x]; // windows bitmap need reverse order: bgr instead of rgb
                            g = data[offset + x + cStride          ];
                            r = data[offset + x + cStride + cStride];

                            *dst++ = (UCHAR)r;
                            *dst++ = (UCHAR)g;
                            *dst++ = (UCHAR)b;*/
							
							/**dst++ = (long)data[iIter+2] << 16;
                            *dst++ = (long)data[iIter+1] << 8;
                            *dst++ = (long)data[iIter];*/
							*dst++ = (UCHAR)data[iIter+2];							
							*dst++  = (UCHAR)data[iIter+1];
							*dst++ = (UCHAR)data[iIter];

						//*dst++ = (data[iIter] & 0xff); // Red						
						//*dst++ = (data[iIter] & 0xff00) >> 8; // Green
						//*dst++ = (data[iIter] & 0xff0000) >> 16; // Blue
							/*if (y==200 && x>100)
							{
								std::cout << "data[iIter+2]=" << (long)data[iIter+2] << ",";
								std::cout << "data[iIter+1]=" << (long)data[iIter+1] << ",";
								std::cout << "data[iIter+0]=" << (long)data[iIter+0] << "  ";
							}*/
							


						iIter=iIter+3;
                    }
                }

                if (AddAviFrame(_avi, _hbitmap) != AVIERR_OK){
                    CloseAvi(_avi);
                    _avi = NULL;
                    _messageFld->setStringValue("Could not add frame to AVI. Closing avi");
                    _numFramesFld->setIntValue(0);
                } else {
                    _numFramesFld->setIntValue(_numFramesFld->getIntValue() + 1);
                    _messageFld->setStringValue("Frame added to AVI.");
                }                
            }else {
                std::string msg;
                msg.append("Input image's width or height is not a multiple of 4!\n");
                msg.append("Skipping this frame.");
                _messageFld->setStringValue(msg.c_str());
            }
        } else {

            std::string msg;
            msg.append("Input image's size has differed!\n");
            msg.append("Skipping this frame.");
            _messageFld->setStringValue(msg.c_str());
        }
    }
}

#endif // ifdef WIN32

ML_END_NAMESPACE

