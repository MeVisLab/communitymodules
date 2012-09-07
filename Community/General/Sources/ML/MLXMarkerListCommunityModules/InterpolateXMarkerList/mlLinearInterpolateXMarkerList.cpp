//----------------------------------------------------------------------------------
//! The ML module class LinearInterpolateXMarkerList.
/*!
// \file    mlLinearInterpolateXMarkerList.cpp
// \author  Coert Metz
// \date    2009-09-09
//
// Linearly interpolate an XMarkerList
*/
//----------------------------------------------------------------------------------

/* =================================================================================
   Copyright (c) 2010, Biomedical Imaging Group Rotterdam (BIGR),
   Departments of Radiology and Medical Informatics, Erasmus MC. All
   rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of BIGR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL BIGR BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =================================================================================*/

// Local includes
#include "mlLinearInterpolateXMarkerList.h"

ML_START_NAMESPACE


//! Implements code for the runtime type system of the ML
ML_BASEOP_CLASS_SOURCE(LinearInterpolateXMarkerList, BaseOp);

//----------------------------------------------------------------------------------
//! Constructor
//----------------------------------------------------------------------------------
LinearInterpolateXMarkerList::LinearInterpolateXMarkerList ()
  : BaseOp(0, 0)
{
  ML_TRACE_IN("LinearInterpolateXMarkerList::LinearInterpolateXMarkerList ()");

  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Get reference to the container for parameters/fields.
  FieldContainer &fields = *getFieldContainer();

  // Add fields to the module and set their values.
  _inputXMarkerListFld = fields.addBase("inputXMarkerList");
  _inputXMarkerListFld->addAllowedType<XMarkerList>();
  _outputXMarkerListFld = fields.addBase("outputXMarkerList");
  _outputXMarkerListFld->setBaseValueAndAddAllowedType(&_outputXMarkerList);

  // Mode field: equidistant or add intermediate points
  std::vector< std::string > modes( 4 );
  modes[ 0 ] = "Equidistant";
  modes[ 1 ] = "Intermediate points";
  modes[ 2 ] = "Absolute marker indices";
  modes[ 3 ] = "Equidistant marker indices";
  _modeFld = fields.addEnum( "mode", modes );
  _modeFld->setEnumValue( 0 );
  
  // Sample distance for output XMarkerList
  _sampleDistanceFld = fields.addFloat("sampleDistance");
  _sampleDistanceFld->setFloatValue(0.5f);

  // Number of intermediate points to add
  _numIntermediatePointsFld = fields.addInt( "numberIntermediatePoints" );
  _numIntermediatePointsFld->setIntValue( 3 );

  // Marker indices for third mode
  _absoluteMarkerIndicesFld = fields.addString( "absoluteMarkerIndices" );
  _absoluteMarkerIndicesFld->setStringValue( "" );

  // Number of total point for fourth mode
  _numTotalPointsFld = fields.addInt( "numberTotalPoints" );
  _numTotalPointsFld->setIntValue( 3 );

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();
}

//----------------------------------------------------------------------------------
//! Handle field changes of the field field.
//----------------------------------------------------------------------------------
void LinearInterpolateXMarkerList::handleNotification ( Field * field )
{
  ML_TRACE_IN("LinearInterpolateXMarkerList::handleNotification ()");

  // Clear output XMarkerList
  _outputXMarkerList.clear();

  Base * baseValue = _inputXMarkerListFld->getBaseValue();
  if (baseValue && BASE_IS_A(baseValue, XMarkerList)) 
  {
    XMarkerList markers = *(XMarkerList*)baseValue;
    
    if ( markers.size() > 1 && ( markers.isModified() || field == _modeFld || field == _numIntermediatePointsFld || field == _sampleDistanceFld ) ) 
    {
      if ( _modeFld->getEnumValue() == 0 )
      {
        // Compute length XMarkerList
        double length = 0.0;
        for (size_t i=0; i<markers.size()-1; ++i) {
          length += (markers[i].pos-markers[i+1].pos).length();
        }

        // Check sample distance setting
        if (_sampleDistanceFld->getFloatValue()<=0.0f) {
          _sampleDistanceFld->setFloatValue(1.0f);
        }
        // Get sample distance setting
        const float sampleDistance = _sampleDistanceFld->getFloatValue();

        // Add first point
        _outputXMarkerList.appendItem(markers[0]);

        // The stepsize is the sampledistance
        double stepSize = sampleDistance;
        // Centerline length already processed
        double curLength = 0.0;
        // Current line segment position
        unsigned int segmentPos = 0;

        // Get first position
        vec3 currentPos = markers[0].pos.getVec3();
        while (curLength < length && segmentPos<markers.size()-1) {
          // Determine direction
          vec3 direction = (vec6(markers[segmentPos+1].pos-markers[segmentPos].pos)).getVec3();
          direction.normalize();
          // Determine segment length
          double segmentLength=(markers[segmentPos+1].pos-markers[segmentPos].pos).length();
          double curSegmentLength=0.0;
          // Step in direction until next input point
          while (curSegmentLength+stepSize<segmentLength) {
            currentPos = currentPos + stepSize * direction;
            curSegmentLength += stepSize;
            curLength += stepSize;
            _outputXMarkerList.appendItem(XMarker (currentPos));
            stepSize = sampleDistance;
          }
          // Determine rest length
          stepSize = segmentLength-curSegmentLength;
          // Update currentpos
          currentPos = currentPos + stepSize * direction;
          curLength+=stepSize;
          stepSize = sampleDistance - stepSize;

          // Increment current line segment position
          ++segmentPos;
        }
      }
      else if ( _modeFld->getEnumValue() == 1 )
      {
        for ( size_t i = 0; i < markers.size() - 1; ++i )
        {
          const float step = 1.0f / static_cast< float >( _numIntermediatePointsFld->getIntValue() + 1 );
          _outputXMarkerList.appendItem( markers.at( i ) );
          for ( float s = step; s <= 1.0f - step; s += step )
          {
            XMarker marker( ( 1.0f - s ) * markers.at( i ).pos + s * markers.at( i + 1 ).pos );
            _outputXMarkerList.appendItem( marker );
          }
          _outputXMarkerList.appendItem( markers.at( i + 1 ) );
        }
      }
      else if ( _modeFld->getEnumValue() == 2 )
      {
        // Retrieve locations
        std::stringstream timePoints ( _absoluteMarkerIndicesFld->getStringValue() );
        float timePoint;
        std::vector< float > positions;
        while ( timePoints >> timePoint ) 
        {
          positions.push_back( timePoint );
        }
        for ( size_t i = 0; i < positions.size(); ++i )
        {
          const size_t prevIndex = static_cast< size_t >( positions[ i ] );
          const size_t nextIndex = static_cast< size_t >( positions[ i ] + 1 );
          const float fraction = positions[ i ] - static_cast< float >( prevIndex );
          if ( prevIndex >= 0 && nextIndex >= 0 && prevIndex < markers.size() && nextIndex < markers.size() )
          {
            XMarker marker( ( 1.0f - fraction ) * markers.at( prevIndex ).pos + fraction * markers.at( nextIndex ).pos );
            marker.pos[ 0 ] = i;
            _outputXMarkerList.appendItem( marker );
          }
          else
          {
            std::cerr << "Position " << positions[ i ] << " outside marker range. Skipping point." << std::endl;
          }
        }
      }
      else if ( _modeFld->getEnumValue() == 3 )
      {
        const float stepSize = static_cast< float >( markers.size() ) / ( static_cast< float >( _numTotalPointsFld->getIntValue() - 1 ) );
        int i = 0;
        for ( float f = 0; f < static_cast< float >( markers.size() ) + stepSize / 10.0f; f += stepSize, ++i )
        {
          const size_t prevIndex = std::min( markers.size() - 1, std::max( size_t( 0 ), static_cast< size_t >( f ) ) );
          const size_t nextIndex = std::min( markers.size() - 1, static_cast< size_t >( f + 1 ) );
          const float fraction = f - static_cast< float >( prevIndex );
          XMarker marker( ( 1.0f - fraction ) * markers.at( prevIndex ).pos + fraction * markers.at( nextIndex ).pos );
          marker.pos[ 0 ] = i;
          _outputXMarkerList.appendItem( marker );
        }
      }
    }
  }

  // Notify attached modules
  _outputXMarkerListFld->notifyAttachments();
}

ML_END_NAMESPACE

