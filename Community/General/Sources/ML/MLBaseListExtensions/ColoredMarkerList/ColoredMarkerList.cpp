//----------------------------------------------------------------------------------
//! This class provides an ColoredMarker(List)
/*!
// \file    ColoredMarkerList.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//
// 
*/
//----------------------------------------------------------------------------------


#ifndef __ColoredMarkerList_H
#include "ColoredMarkerList.h"
#endif


ML_START_NAMESPACE

// ------------------------------------------------------------------
// --- Class ColoredMarker
// ------------------------------------------------------------------

ML_CLASS_SOURCE(ColoredMarker, BaseItem)

//! Assignment operator
ColoredMarker &ColoredMarker::operator = (const ColoredMarker &marker_)
{
    ML_TRACE_IN("ColoredMarker &ColoredMarker::operator = (const ColoredMarker &marker_)");
  
    BaseItem::operator = (marker_);
    pos   = marker_.pos;
    color = marker_.color;
    type  = marker_.type;
    
    return *this;
}

//! Return a string representation of the item object. The string
//! is allocated on the heap and is later disposed by clearPersistentState().
char *ColoredMarker::getPersistentState() const
{
    ML_TRACE_IN("char *ColoredMarker::getPersistentState () const");

    ML_TRY{
      std::ostringstream ost;
      size_t i = 0, posDims= 6;
      while ((posDims > 3) && MLValueIs0WOM(pos[posDims-1]) ){
        posDims--;
      }

      ost << '(';
      for (i = 0; i < posDims-1; i++){
          ost << pos[i] << ' ';
      }
      ost << pos[posDims-1] << ')';
    
      if (color != Vector4(0)){
          ost << '(' << color[0] << ' ' << color[1] << ' ' << color[2] << ' ' << color[3] << ')';
      }
    
      if (type) {
          ost << ' ' << type;
      }
    
      if (getId()){
          ost << " #" << getId();
      }
    
      if (name())
      {
          if (*name()){
              ost << " :" << name();
          }
      }
    
      return newString(ost.str());
    }
    ML_CATCH_RETHROW;
}


//! Initialize the item object from the string \a state
void ColoredMarker::setPersistentState(const char *state)
{
    ML_TRACE_IN("void ColoredMarker::setPersistentState(const char *state)");

    ColoredMarkerParser parser;
    std::string newName;
    
    int parserResult = parser.init(state);
    if (!parserResult){ parserResult = parser.getPosition(pos);                     }  
    if (!parserResult){ parserResult = parser.getColor(color);                      } 
    if (!parserResult){ parserResult = parser.getType(type);                        }
    if (!parserResult){ MLssize_t id=0; parserResult = parser.getId(id); setId(id); }
    if (!parserResult){ parserResult = parser.getName(newName);                     }
    if (!parserResult){ setName(newName.c_str());                                   }
    
    if (parserResult > 0){
        ML_PRINT_ERROR("ColoredMarker::setPersistentState()", ML_EMPTY_MESSAGE,
        parser.getErrorMessage(parserResult));
    }
    
}

//! Add state to tree
void ColoredMarker::addStateToTree(TreeNode *parent) const
{
    ML_TRACE_IN("void ColoredMarker::addStateToTree(TreeNode *parent) const");

    ML_TRY{
      ML_ADDSTATE_VERSION(ColoredMarker);
      ML_ADDSTATE_SUPER(BaseItem);
      parent->addChild(pos, "pos");
      parent->addChild(color, "color");
      if (type) { parent->addChild(type, "type"); }
    }
    ML_CATCH_RETHROW;
}

//! Read state from tree
void ColoredMarker::readStateFromTree(TreeNode *parent)
{
    ML_TRACE_IN("void ColoredMarker::readStateFromTree(TreeNode *parent)");

    ML_TRY{
      int version = parent->getVersion("ColoredMarker");
      if (version > 0) { throw TreeNodeException(TNE_UnsupportedClassVersion); }
    
      ML_READSTATE_SUPER(BaseItem);
    
      parent->readChild(pos, "pos");
      parent->readChild(color, "color");
      ML_READCHILD_OPTIONAL(type, "type", 0);
    }
    ML_CATCH_RETHROW;
}



// ------------------------------------------------------------------
// --- Class ColoredMarkerList
// ------------------------------------------------------------------

ML_CLASS_SOURCE(ColoredMarkerList, ListBase)


// Convert to XMarkerList
ml::XMarkerList ColoredMarkerList::toXMarkerList()
{
  ml::XMarkerList result;

  for(ml::ColoredMarkerList::iterator thisElement = this->begin();  thisElement != this->end();  ++thisElement)
  {
    ml::XMarker newMarker(Vector6(thisElement->x(),thisElement->y(),thisElement->z(),thisElement->c(),thisElement->t(),thisElement->u()), (int)thisElement->type);    
    newMarker.setName(thisElement->name());

    result.push_back(newMarker);
  }

  return result;
}

// Convert from XMarkerList
void ColoredMarkerList::fromXMarkerList(ml::XMarkerList importList, Vector4 rgbaColor)
{
  this->clear();

  for(ml::XMarkerList::iterator thisXMarker = importList.begin();  thisXMarker != importList.end();  ++thisXMarker)
  {
    ml::ColoredMarker newMarker(Vector3(thisXMarker->x(),thisXMarker->y(),thisXMarker->z()), thisXMarker->type);
    newMarker.c() = thisXMarker->c();
    newMarker.t() = thisXMarker->t();
    newMarker.u() = thisXMarker->u();
    newMarker.rgba() = rgbaColor;
    newMarker.setName(thisXMarker->name());

    this->push_back(newMarker);
  }

}

// Convert from XMarkerList
void ColoredMarkerList::fromXMarkerList(ml::XMarkerList importList, Vector3 rgbColor, float alpha)
{
  this->fromXMarkerList(importList, Vector4(rgbColor, alpha));
}


// ------------------------------------------------------------------
// --- Class ColoredMarkerListContainer
// ------------------------------------------------------------------

ML_CLASS_SOURCE(ColoredMarkerListContainer, ListContainerBase);


//! Constructor
ColoredMarkerListContainer::ColoredMarkerListContainer()
: ListContainerTemplate<ColoredMarkerList>(0, 0)
{
    ML_TRACE_IN("ColoredMarkerListContainer::ColoredMarkerListContainer(): ListContainerTemplate<ColoredMarkerList>(0, 0)");

    FieldContainer *fields = getFieldContainer();
    
    // Suppress handleNotification()
    _lockNotification = 1;
    
    // Position fields
    (_fldPos3D    = fields->addVector3("posXYZ")) ->setVector3Value(Vector3(0));
    (_fldPosC     = fields->addFloat("posC"))     ->setFloatValue(0);
    (_fldPosT     = fields->addFloat("posT"))     ->setFloatValue(0);
    (_fldPosU     = fields->addFloat("posU"))     ->setFloatValue(0);

    // Color fields
    (_fldColor    = fields->addColor("color"))    ->setColorValue(0,0,0);
    (_fldAlpha    = fields->addFloat("alpha"))    ->setFloatValue(1);
    _fldAlpha->setFloatMinValue(0);
    _fldAlpha->setFloatMaxValue(1);

    // Type field
    (_fldType = fields->addInt("type"))->setIntValue(0);
    

    // Template position fields
    (_fldNewPos3D = fields->addVector3("newPosXYZ")) ->setVector3Value(Vector3(0));
    (_fldNewPosC  = fields->addFloat("newPosC"))     ->setFloatValue(0);
    (_fldNewPosT  = fields->addFloat("newPosT"))     ->setFloatValue(0);
    (_fldNewPosU  = fields->addFloat("newPosU"))     ->setFloatValue(0);

    // Template color fields
    (_fldNewColor = fields->addColor("newColor"))    ->setColorValue(0,0,0);
    (_fldNewAlpha = fields->addFloat("newAlpha"))    ->setFloatValue(1);
    _fldNewAlpha->setFloatMinValue(0);
    _fldNewAlpha->setFloatMaxValue(1);    

    // Template Type field
    (_fldNewType = fields->addInt("newType"))->setIntValue(0);

    // Template Enable handleNotification()
    _lockNotification = 0;
}


//! Return true if \a field points to an item property field
bool ColoredMarkerListContainer::isPropertyField(Field *field)
{
    ML_TRACE_IN("bool ColoredMarkerListContainer::isPropertyField(Field *field)");

    return ( (field == _fldPos3D) || (field == _fldPosC) || (field == _fldPosT) || (field == _fldPosU) || 
             (field == _fldColor) || (field == _fldAlpha) ||  (field == _fldType) || ListContainerBase::isPropertyField(field) );
}


//! Reset all property fields
void ColoredMarkerListContainer::resetPropertyFields()
{
    ML_TRACE_IN("void ColoredMarkerListContainer::resetPropertyFields()");

    _lockNotification++;
    
    // Index fields
    _fldPos3D->setVector3Value(Vector3(0));
    _fldPosC->setFloatValue(0);
    _fldPosT->setFloatValue(0);
    _fldPosU->setFloatValue(0);

    // Color fields
    _fldColor->setColorValue(0,0,0);
    _fldAlpha->setFloatValue(1);
    
    // Type field
    _fldType->setIntValue(0);
    
    // Base class fields
    ListContainerTemplate<ColoredMarkerList>::resetPropertyFields();
    
    _lockNotification--;
}


//! Update property fields from the current list item
void ColoredMarkerListContainer::updatePropertyFields()
{
    ML_TRACE_IN("void ColoredMarkerListContainer::updatePropertyFields()");

    ColoredMarker *marker=NULL;
    MLssize_t index = mlrange_cast<MLssize_t>(_fldIndex->getIntValue());
    
    // Valid list and item?
    if (_listPtr && (index >= 0) && (index < mlrange_cast<MLssize_t>(_listPtr->size())) )
    {
        // Suppress handleNotification()
        _lockNotification++;
        
        // Pointer to list item
        marker = &(*_listPtr)[mlrange_cast<size_t>(index)];
        
        // Update fields from list item if they have changed from the current values
        const Vector3 pos3d(Vector3(marker->pos[0],marker->pos[1],marker->pos[2]));
        
        if(_fldPos3D->getVector3Value() != pos3d) {
          _fldPos3D->setVector3Value(pos3d);
        }

        if(_fldPosC->getFloatValue() != marker->c()) {
          _fldPosC->setFloatValue((float)marker->c());
        }

        if(_fldPosT->getFloatValue() != marker->t()) {
          _fldPosT->setFloatValue((float)marker->t());
        }

        if(_fldPosU->getFloatValue() != marker->u()) {
          _fldPosU->setFloatValue((float)marker->u());
        }

        if(_fldColor->getVector3Value() != Vector3(marker->rgba(), false)) {
          _fldColor->setVector3Value(Vector3(marker->rgba(), false));
        }

        if(_fldAlpha->getFloatValue() != marker->alpha()) {
          _fldAlpha->setFloatValue((float)marker->alpha());
        }

        if(_fldType->getIntValue() != marker->type) {
          _fldType->setIntValue(marker->type);
        }
        
        // Update base class fields
        ListContainerTemplate<ColoredMarkerList>::updatePropertyFields();
        
        // Enable handleNotification()
        _lockNotification--;
    }
}


//! Modify item at position \a index. If \a fromCurrentItem is true, the
//! item is copied from _currentItem, otherwise the item property corresponding
//! to the property field \a field is modified.
//! Return true if item has been modified.
bool ColoredMarkerListContainer::modifyItem(MLssize_t index, Field *field, bool fromCurrentItem)
{
    ML_TRACE_IN("bool ColoredMarkerListContainer::modifyItem(MLssize_t index, Field *field, bool fromCurrentItem)");

    ML_TRY{
      bool modified = true;
      ColoredMarker *marker=NULL;
    
      // Valid list and item?
      if (_listPtr && (index >= 0) && (index < mlrange_cast<MLssize_t>(_listPtr->getSize())) )
      {
          // Modify from property field?
          if (!fromCurrentItem)
          {
              // Pointer to list item
              marker = &(*_listPtr)[mlrange_cast<size_t>(index)];
            
              // Position fields
              if ((field == _fldPos3D) || (field == _fldPosC) || (field == _fldPosT) || (field == _fldPosU))
              {
                  marker->pos = Vector6(_fldPos3D->getVector3Value(),0,0,0);
                  marker->c() = _fldPosC->getFloatValue();
                  marker->t() = _fldPosT->getFloatValue();
                  marker->u() = _fldPosU->getFloatValue();

              } 
              else if (field == _fldColor  || (field == _fldAlpha))
              {
                  // ImageVector field
                  marker->rgba()          = Vector4(_fldColor->getVector3Value(),_fldAlpha->getFloatValue());

              } 
              else if (field == _fldType)
              {
                  // Type field
                  marker->type = _fldType->getIntValue();
                
              } 
              else 
              {
                  // Not an ColoredMarker-specific field, pass call to base class
                  modified = ListContainerTemplate<ColoredMarkerList>::modifyItem(index, field, fromCurrentItem);
              }

          } 
          else 
          {
              // Modify from _currentItem, handled in base class
              modified = ListContainerTemplate<ColoredMarkerList>::modifyItem(index, field, fromCurrentItem);
          }

      } else 
      {
          modified = false;
      }
    
      return modified;
    }
    ML_CATCH_RETHROW;
}

//! Copy values from item at position \a index to the template fields.
void ColoredMarkerListContainer::copyItemToTemplate(MLssize_t index)
{
  ML_TRACE_IN("void ColoredMarkerListContainer::copyItemToTemplate(MLssize_t index)");

  ML_TRY{
    ColoredMarker *marker = &(*_listPtr)[mlrange_cast<size_t>(index)];

    // ColoredMarker-specific fields
    _fldNewPos3D  ->setVector3Value( Vector3(marker->pos[0], marker->pos[1], marker->pos[2]) );
    _fldNewPosC   ->setFloatValue( (float)marker->pos[3] );
    _fldNewPosT   ->setFloatValue( (float)marker->pos[4] );
    _fldNewPosU   ->setFloatValue( (float)marker->pos[5] );
    _fldNewColor  ->setVector3Value( Vector3(marker->color, false) );
    _fldNewAlpha  ->setFloatValue( (float)marker->color[3] );

    _fldNewType   ->setIntValue( marker->type );

    // Base class fields
    ListContainerTemplate<ColoredMarkerList>::copyItemToTemplate(index);
  }
  ML_CATCH_RETHROW;
}

//! Copy the values of the templates fields to \a _currentItem.
void ColoredMarkerListContainer::copyTemplateToCurrent()
{
  ML_TRACE_IN("void ColoredMarkerListContainer::copyTemplateToCurrent()");

  ML_TRY{
    // ColoredMarker-specific fields
    _currentItem.pos   = Vector6(_fldNewPos3D->getVector3Value(), _fldNewPosC->getFloatValue(), _fldNewPosT->getFloatValue(), _fldNewPosU->getFloatValue());
    _currentItem.color = Vector4(_fldNewColor->getVector3Value(), _fldNewAlpha->getFloatValue());
    _currentItem.type  = _fldNewType->getIntValue();

    // Base class fields
    ListContainerTemplate<ColoredMarkerList>::copyTemplateToCurrent();
  }
  ML_CATCH_RETHROW;
}

//! Initialize the list item at position \a index. This method is called by
//! insertItem() if the \a fromCurrentItem argument is false.
//! Moreover it is called when fldInit is touched.
void ColoredMarkerListContainer::initItem(MLssize_t index)
{
    ML_TRACE_IN("void ColoredMarkerListContainer::initItem(MLssize_t index)");

    ML_TRY{
      ColoredMarker *marker = &(*_listPtr)[mlrange_cast<size_t>(index)];

      // ColoredMarker-specific fields
      marker->pos   = Vector6(0);
      marker->color = Vector4(0);
      marker->type  = 0;
    
      // Base class fields
      ListContainerTemplate<ColoredMarkerList>::initItem(index);
    }
    ML_CATCH_RETHROW;
}

//! Parse and return position
int ColoredMarkerParser::getPosition(Vector6 &pos)
{
    ML_TRACE_IN("int ColoredMarkerParser::getPosition(Vector6 &pos)");

    int result=0;
    size_t i=0;

    double values[6];
    
    for (i = 0; i < 6; i++){
        values[i] = 0;
    }
    
    result = getArray(3, 6, values);
    
    for (i = 0; i < 6; i++){
        pos[i] = values[i];
    }
    
    return result;
}

//! Parse and return color with alpha
int ColoredMarkerParser::getColor(Vector4 &color)
{
    ML_TRACE_IN("int XMarkerParser::getColor(Vector4 &color)");

    int result=0;
    size_t i=0;
    double values[4];
    
    for (i = 0; i < 3; i++){
        values[i] = 0;
    }
    
    result = getArray(3, 4, values);
    if (result == kMissingLeftParenthesis){
        result = 0;
    }
    
    for (i = 0; i < 4; i++){
        color[i] = values[i];
    }
    
    return result;
}

//! Parse and return type
int ColoredMarkerParser::getType(MLint &type)
{
    ML_TRACE_IN("int ColoredMarkerParser::getType(int &type)");

    ML_TRY{
      char *endPtr = NULL;
    
      type = 0;
      if (!_eos)
      {
          if (!*_pNext){
              _eos = true;
          } else {
              type = strtol(_pNext, &endPtr, 10);
              _pNext = endPtr;
          }
      }
    
      return 0;
    }
    ML_CATCH_RETHROW;
}

//! Parse n-tupel with a minimum of \a nMin and a maximum of \a nMax
//! float numbers, separated by whitespace and enclosed in parenthesis.
//! Values are stored in \a values[0..].
int ColoredMarkerParser::getArray(int nMin, int nMax, double *values)
{
    ML_TRACE_IN("int ColoredMarkerParser::getArray(int nMin, int nMax, double *values)");

    ML_TRY{
      int result = 0, n=0;
      double x=0;
      char *endPtr=NULL;
    
      if (_eos){
          result = kEndOfSource;
      } else {
          if (!*_pNext){
              _eos = true;

          } else {
              // Skip leading whitespace
              skipWhitespace();
            
              // Test for opening parenthesis
              if (*_pNext == '(')
              {
                  // Start parsing numbers
                  for (n =0 , ++_pNext; n < nMax; n++)
                  {
                      // Convert string to number, exit loop if conversion not possible
                      x = strtod(_pNext, &endPtr);
                      if (_pNext == endPtr){
                          break;

                      } else {
                          // Proceed behind number and store number array
                          _pNext = endPtr;
                          values[n] = x;
                      }
                  }
                  // Skip whitespace and test for closing parenthesis
                  skipWhitespace();
                  if (*_pNext == ')')
                  {
                      // Proceed behind n-tupel
                      ++_pNext;
                      // Minimum number of values parsed?
                      if (n < nMin){
                          result = kTooFewNumbers;
                      }
                  } else {
                      // No closing parenthesis
                      result = kMissingRightParenthesis;
                  }
              } else {
                  // No opening parenthesis
                  result = kMissingLeftParenthesis;
              }
          }
      }
    
      return result;
    }
    ML_CATCH_RETHROW;
}

//! Get error string for \a errorCode
const char *ColoredMarkerParser::getErrorMessage(int errorCode)
{
    ML_TRACE_IN("const char *ColoredMarkerParser::getErrorMessage(int errorCode)");

    ML_TRY{
      static const char *errorStrings[kNumErrorCodes-kNumBaseItemErrorCodes] =
      { "Missing left parenthesis",
      "Missing right parenthesis",
      "Too few numbers in position/vector",
      };
      const char *msg;
    
      if (errorCode < kNumBaseItemErrorCodes || errorCode >= kNumErrorCodes){
          msg = BaseItemParser::getErrorMessage(errorCode);
      } else {
          msg = errorStrings[errorCode-kNumBaseItemErrorCodes];
      }
    
      return msg;
    }
    ML_CATCH_RETHROW;
}


ML_END_NAMESPACE

