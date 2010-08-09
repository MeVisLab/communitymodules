// Copyright (c) 2009, Fraunhofer MEVIS Institute for Medical Image Computing
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Fraunhofer MEVIS nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR THE COPYRIGHT HOLDER BE LIABLE 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
//! Implementation of an example operator for adding
/*!
// \file    mlBarcode.cpp
// \author  Dominik Boehm  
// \date    8/2003
*/
//--------------------------------------------------------------------------------
// Include ml Macros for implementation support.
#include "mlOperatorMacros.h"

// Include your header.
#include "mlBarcode.h"
#include "mlVersion.h"

ML_START_NAMESPACE

  //-----------------------------------------------------------------------------------
  //! This function is not available under non-Windows-Systems. This is
  //! an os-independent replacement.
  //-----------------------------------------------------------------------------------
#ifndef WIN32
  const char* strupr(const std::string &s) {
    std::string rs = s;
    std::string::iterator ptr = rs.begin();
    std::string::iterator end = rs.end();
    for (; ptr != end; ++ptr) {
      unsigned char c = *ptr;
      if (islower(c)) {
        *ptr = (unsigned char) toupper(c);
      }
    }
    return rs.c_str();
  }
#endif

  //--------------------------------------------------------------------------------
  // This macro implements some functions and methods for the runtime system and
  // for the initialization of this class. See mlRuntimeSubClass.h for
  // more information.
  // Be sure that names are written correctly since the compiler cannot check
  // everything.
  //--------------------------------------------------------------------------------
  ML_BASEOP_CLASS_SOURCE(Barcode,BaseOp)

    //--------------------------------------------------------------------------------
    //! Constructor:
    //! -Calls base class constructor to set right number of inputs and outputs.
    //! -Initializes fields for input and output images and algorithm parameters.
    //! -Connects input and parameter fields with output field(s) to propagate
    //!  input changes to output(s) so that appended modules are notified on changes.
    //!  Note that input and output image fields are created automatically when
    //!  the base class constuctor is called.
    //!  Normal algorithm parameters must be created/initialized 'by hand' in
    //!  the constructor.
    //! -Sets algorithm parameters like memory management (inplace calculation) or
    //!  multithreading support (parallelization).
    //--------------------------------------------------------------------------------
    Barcode::Barcode(): BaseOp(0,1) // 0 input and one output image
  {
    // ---- Create and initialize fields
    // Create fields, add them to the field container and set the parameters.
    // Important: Field name must never contain spaces!
    handleNotificationOff();
    _lockNotification = 1;

    _qFld = getFieldContainer()->addInt("margin", &_q);
    _qFld->setIntValue(200);

    _xFld = getFieldContainer()->addInt("barWidth", &_x);
    _xFld->setIntValue(8);

    _ratioFld = getFieldContainer()->addFloat("ratio", &_ratio);
    _ratioFld->setFloatValue(2.0);

    _iFld = getFieldContainer()->addInt("interval", &_i);
    _iFld->setIntValue(8);

    //! Pixelvalue for lightbars
    _foregroundFld = getFieldContainer()->addInt("background", &_background);
    _foregroundFld->setIntValue(4095);

    //! Pixelvalue for darkbars
    _backgroundFld = getFieldContainer()->addInt("foreground", &_foreground);
    _backgroundFld->setIntValue(0);

    _ysizeFld = getFieldContainer()->addInt("height", &_ysize);
    _ysizeFld->setIntValue(300);

    _textFld = getFieldContainer()->addString("text");

    //_statusFld = getFieldContainer()->addString("Status");
    //_statusFld->setStringValue("Ok. ");

    _apply = getFieldContainer()->addNotify("apply");

    _apply->attachField(getOutField());

    _autoApplyFld = getFieldContainer()->addToggle("autoApply");
    _autoApplyFld->setIntValue(true);


    _lockNotification = 0;

    _xsize=0;

    _codeline = NULL;



    // ---- Connect fields.
    // Note: Input- and output images are also represented by fields since
    //       they are only complex algorithm parameters or results.
    // Input or output fields can be accessed with getInField(#InputNum) or
    // getOutField(#InputNum), respectively.

    // ---- Connect fields
    // Connect all input (parameter)fields with the output. So all input changes
    // cause that connected modules are notified. E.g. in the case of an appended
    // display it redraws automatically after parameter changes.
    //    getInField(0) ->attachField(getOutField());
    //    getInField(1) ->attachField(getOutField());

    // ---- Set algorithm properties
    // Memory optimization: Input sub image 0 and output subimage are identical
    // for calcOutSubImage if setOutImgInplace(0) is set. So less allocations
    // are required and the algorithm is faster, and the cpu cache works better
    // because the cpu needs to switch between less memory areas.
    //    setOutImgInplace(0);

    // --- Activate multithreading
    // If your calcOutSubImage implementation is thread save/reentrant, i.e.
    // if it can be executed parallely then you can activate multithreading by
    // calling
    //   setThreadSupport(1);
    // So you can potentially multiply the performance of your algorithm.
    // If not called or by calling   setThreadSupport(0) (which is the default)
    // multithreading remains disabled for your algorithm.
    // IMPORTANT: Please activate multithreading only if you are sure that your
    //            algorithm supports it. Otherwise subtle errors could occur.
    //            Ask experienced programmers if necessary.

    handleNotificationOn();
  }
  //@}

  Barcode::~Barcode() {

    //Check, if an codeline exists and remove this.
    if (_codeline) {
      delete [] _codeline;
    }

  } // End ~Barcode


  //--------------------------------------------------------------------------------
  //! Creates the bar coded pixel line.
  //--------------------------------------------------------------------------------
  void Barcode::makeCodeLine(){

    int _numofchars=0, n=0;  //number of chars to be coded, counter, counter
    int x=0, dx=0; // counter for pixels in codeline
    char _stripcode[9]; //code with 6 small bars (=0) and 3 large bars (=1) for one character
    char* _barcodefull; // text to be coded with '*' at the beginning and the end
    int _color=0; // Color of the current bar


    // One codeline is composed of:
    //  - a light area at the beginning (size: _q pixels)
    //  - the text to be coded with a '*' at the start/end
    //     (Size for a small bar: _x pixels,
    //      size for a large bar: _ratio * _x pixels,
    //      space between two characters: _i pixels)
    //  - a light area at the end (size: _q pixels)


    // Set value for color.
    _color = _background;


    // Number of chars to be coded (plus 2 chars for the '*' at the beginning and the end
    _numofchars = strlen(_textFld->getStringValue().c_str());


    // Calculate the size of the codeline in pixel
    _xsize = (int)((_numofchars+2)*(6*_x + 3*_ratio*_x) + (_numofchars+1) * _i + 2*_q);

    // Check, if an old codeline exists and remove this.
    if (_codeline!=NULL) {
      delete [] _codeline;
    }

    // Allocate memory for a new codeline
    _codeline = new int[_xsize];
    if(!_codeline) {
      mlDebug("Error initializing codeline");
    }
    // Initialize new codeline, set all pixels to white
    for (x=0; x<_xsize; x++) {
      _codeline[x]=_background;
    }

    // Allocate memory for the complete text (including 2x'*')
    _barcodefull = new char [_numofchars+4];
    // Initialize new string
    for(int i2=0;i2<_numofchars+2;i2++) {
      _barcodefull[i2] =0;
    }

    // Build the complete text
    strcat(_barcodefull,"*");                          // '*' at the beginning
    strcat(_barcodefull, strupr(strdup(_textFld->getStringValue().c_str()))); //Given text converted in uppercase
    strcat(_barcodefull,"*");                          //'*' at the end


    // Light area at the beginning
    for(x=0; x<_q; x++) {
      _codeline[x]=_background;
    }

    // Now we can start to build the bars for each character
    _color = _background;


    // Loop through the complete text
    for (unsigned int i3=0; i3<strlen(_barcodefull); i3++)
    {
      // Get the code for the i-th character
      strcpy(_stripcode, Code39(_barcodefull[i3]));



      //Loop through the code of the current character
      for (n=0; n < 9; n++)
      {
        // change color
        if (_color == _background) {
          _color = _foreground;
        }
        else {
          _color = _background;
        }


        switch (_stripcode[n])
        {
          // small bar
        case '0': 
          {
            // fill with color
            for(dx=0;dx<_x;dx++)
            {
              _codeline[x]=_color;
              x++;

            }
          }
          break;
        case '1':  // large bar
          {
            // fill with color
            for(dx=0;dx<(int) (_ratio * _x);dx++)
            {
              _codeline[x]=_color;
              x++;

            }
          }
          break;
        default :
          std::cout << "Barcode: Can't code the character - ERROR" << std::endl;
          break;
        }
      } // end code loop

      // interval between two chars
      _color = _background;

      //fill the interval with color, but not for the last one
      if(i3+1<strlen(_barcodefull)) {

        for(dx=0;dx<_i;dx++)
        {
          _codeline[x]=_color;
          x++;
        }
      }

    } // end character loop


    // After all this the light space at the end of the codeline should be left or
    // in other words: _xsize - x == _q


    //Light area at the end
    while (x<_xsize ) {
      _codeline[x]=_background;
      x++;
    }


    delete [] _barcodefull;

  } 

  //-----------------------------------------------------------------------------
  // Returns the Code 3 of 9 value for a given ASCII character.
  //-----------------------------------------------------------------------------
  char* Barcode::Code39(char Asc)
  {
    switch (Asc)
    {
    case ' ':
      return "011000100";     
    case '$':
      return "010101000";             
    case '%':
      return "000101010"; 
    case '*':
      return "010010100"; // * Start/Stop
    case '+':
      return "010001010"; 
    case '|':
      return "010000101"; 
    case '.':
      return "110000100"; 
    case '/':
      return "010100010"; 
    case '0':
      return "000110100"; 
    case '1':
      return "100100001"; 
    case '2':
      return "001100001"; 
    case '3':
      return "101100000"; 
    case '4':
      return "000110001"; 
    case '5':
      return "100110000"; 
    case '6':
      return "001110000"; 
    case '7':
      return "000100101"; 
    case '8':
      return "100100100"; 
    case '9':
      return "001100100"; 
    case 'A':
      return "100001001"; 
    case 'B':
      return "001001001"; 
    case 'C':
      return "101001000";
    case 'D':
      return "000011001";
    case 'E':
      return "100011000";
    case 'F':
      return "001011000";
    case 'G':
      return "000001101";
    case 'H':
      return "100001100";
    case 'I':
      return "001001100";
    case 'J':
      return "000011100";
    case 'K':
      return "100000011";
    case 'L':
      return "001000011";
    case 'M':
      return "101000010";
    case 'N':
      return "000010011";
    case 'O':
      return "100010010";
    case 'P':
      return "001010010";
    case 'Q':
      return "000000111";
    case 'R':
      return "100000110";
    case 'S':
      return "001000110";
    case 'T':
      return "000010110";
    case 'U':
      return "110000001";
    case 'V':
      return "011000001";
    case 'W':
      return "111000000";
    case 'X':
      return "010010001";
    case 'Y':
      return "110010000";
    case 'Z':
      return "011010000";
    default:
      return "011000100"; 
    }
  }

  //--------------------------------------------------------------------------------
  //! Handle field changes of the field field.
  //--------------------------------------------------------------------------------
  void Barcode::handleNotification (Field *field)
  {
    if (!_lockNotification && field )
    {
      _lockNotification++;


      if ((field ==_apply)|| _autoApplyFld->isOn() )
      {
        makeCodeLine();
        getOutField()->notifyAttachments();
      }

      _lockNotification--;
    }
  }

  //--------------------------------------------------------------------------------
  //! Sets properties of the output image with index outIndex
  //! dependent on the properties of input image(s).
  //--------------------------------------------------------------------------------
  void Barcode::calcOutImageProps(int /*outIndex*/)
  {

    if (_codeline==NULL) {
      makeCodeLine();
    }

    // Output image (barcode) is rotated for better use in mammogramms!

    getOutImg()->setImgExt  ( Vector(_xsize, _ysize, 1, 1, 1, 1));
    getOutImg()->setPageExt ( Vector(_xsize, _ysize, 1, 1, 1, 1));

    // Determine new min/max range.
    long double min = _foreground;
    long double max = _background;
    getOutImg()->setMinVoxelValue( min );
    getOutImg()->setMaxVoxelValue( max );

    // Get data type which can hold the new data range correctly. Use signed types for ambiguous cases
    // where both the unsigned and signed data types could hold the data range. E.g. range is 13 - 97 where
    // unsigned and signed 8 bit integer both are well suited for that range.
    MLDataType dt = MLGetDataTypeForRange(&min, &max, false);


    getOutImg()->setDataType(dt);
  }

  //--------------------------------------------------------------------------------
  //! Calls correctly typed (template) version of calcOutSubImage to calculate page 
  //! outSubImg of output image with index outSubImg.
  //--------------------------------------------------------------------------------
  CALC_OUTSUBIMAGE0_CPP(Barcode);

  //--------------------------------------------------------------------------------
  //! Method template for type-specific page calculation. Called by calcOutSubImage().
  //! \param outSubImg   The typed subimage of output image outIndex loaded form file.
  //! Parameter outIndex is the index of the output the subimage is calculated for and 
  //!                    is not used here.
  //--------------------------------------------------------------------------------
  template <typename DATATYPE>
  void Barcode::calcOutSubImage(TSubImg<DATATYPE> *outImg, int /*outIndex*/)
  {
    // Output image (barcode) is rotated for better use in mammogramms!

    Vector dim = outImg->getImgExt();
    DATATYPE temp_pixelvalue=0;

    #if ML_MAJOR_VERSION >= 2
      // >= MeVisLab 2.1
      TSubImgCursor<DATATYPE> outSubImage(*outImg);
    #else
      // < MeVisLab 2.1
      TSubImg<DATATYPE> &outSubImage = *outImg;
    #endif

    if (_codeline==NULL) {
      makeCodeLine();
    }

    // Loop over whole spatial coordinates of the image.
    Vector v(0,0,0,0,0,0);
    for (v.x=0; v.x<dim.x; v.x++) {
      //v.x=x;
      v.y=0;

      temp_pixelvalue = (DATATYPE) (_codeline[v.x]);
      outSubImage.setCursorSubImgPos(v);
      for (v.y=0; v.y<dim.y; v.y++) {
        *(outSubImage.getCursorPos()) = temp_pixelvalue;
        outSubImage.moveCursorY();
      }
    }

  }

ML_END_NAMESPACE
