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
//! ML module class kAviWriter taking an arbitrary number of images and compresses them into a movie.
/*!
// \file    mlkAviWriter.h
// \author  okonrad, konrad muehler
// \date    2005-07-21
//
// The kAviWriter takes an arbitrary number of images and compresses them into a movie.
*/
// Last changed by $Author: okonrad $
// $Id: mlkAviWriter.h 27276 2005-07-28 11:59:40Z okonrad $
// $Source$
//----------------------------------------------------------------------------------


#ifndef __mlkAviWriter_H
#define __mlkAviWriter_H


// Local includes
#ifndef __MLkAviWriterSystem_H
#include "MLkAviWriterSystem.h"
#endif

// ML includes
#ifndef __mlOperatorIncludes_H
#include "mlOperatorIncludes.h"
#endif

// inclusion of Windows API stuff
#ifdef WIN32
#include <netsrc/aviUtils.h>
#endif




ML_START_NAMESPACE


//! ML module class kAviWriter taking an arbitrary number of images and compresses them into a movie.
class MLKAVIWRITER_EXPORT kAviWriter : public BaseOp
{
public:

    //! Constructor.
    kAviWriter (void);

    //! Handle field changes of the field \c field.
    virtual void handleNotification (Field *field);

    //! Method facilitates parameter loading on module creation.
    virtual void activateAttachments();


#ifdef WIN32 // not available under Linux, Unix or Apple due to the usage of the Windows vfw-API
    //! Adds the an image to the movie.
    void addImageToMovieByData(unsigned char* data, int imgWidth, int imgHeight);
    //! Ends the recording of the movie. Closes avi stream.
    void _endRecording();
#endif // ifdef WIN32


protected:
    //! Default destructor
    virtual ~kAviWriter();

private:

    /// FIELDS

    //! Enables the module to record a movie
    BoolField*   _enableRecordingFld;

    //! The name of the output file
    StringField* _outputFilenameFld;
    //! The codec as a four letter word.
    StringField* _codecCodeFld;

    //! The number of frames per second.
    IntField*    _framesPerSecondFld;
    //! Triggers all parametes to reset and the last avi to discard.
    NotifyField* _triggerResetFld;
    //! Triggers the compression of the movie. The movie is written to disc then.
    NotifyField* _triggerMakeAviFld;
    //! The current number of images.
    IntField*    _numFramesFld;

    //! Field for displaying system messages
    StringField* _messageFld;

    //! Field for displaying available codecs on the current system.
    StringField* _availableCodecsFld;


#ifdef WIN32 // not available under Linux, Unix or Apple due to the usage of the Windows vfw-API


    /// METHODS

    //! Checks the codecCodeFld for a valid codec code.
    void _checkCodec();

    //! Starts the recodring of a movie. Configures the movie header.
    void _startRecording(int imgWidth=0, int imgHeight=0);
  //void _startRecording2(int imgWidth, int imgHeight);

    //! Adds the current image to the movie.
    void _addImageToMovie();

    /// MEMBER VARIABLES

    //! Pointer to the avi
    HAVI _avi;
    //! Pointer to the currently added image frame (bitmap type)
    HBITMAP _hbitmap;
    //! void pointer to the bitmap to be written into the avi.
    void * _bits;


    //! codec name (four chars)
    char _codecName[4];

    //! The input image's width
    int _imgWidth;
    //! The input image's height
    int _imgHeight;

    //! Is the input image a grey-value image (or is it colored?)
    bool _isGreyValueImage;

#endif // ifdef WIN32

    //! Implements interface for the runtime type system of the ML.
    ML_BASEOP_CLASS_HEADER(kAviWriter)
};


ML_END_NAMESPACE

#endif // __mlkAviWriter_H


