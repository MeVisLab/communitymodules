//----------------------------------------------------------------------------------
//! This class provides an IndexPair(List)
/*!
// \file    IndexPairList.cpp
// \author  Axel Newe (axel.newe@fau.de)
// \date    2014-09-01
//
// 
*/
//----------------------------------------------------------------------------------



#ifndef __IndexPairList_H
#include "IndexPairList.h"
#endif


ML_START_NAMESPACE

// ------------------------------------------------------------------
// --- Class IndexPair
// ------------------------------------------------------------------

ML_CLASS_SOURCE(IndexPair, BaseItem)

//! Assignment operator
IndexPair &IndexPair::operator = (const IndexPair &pair_)
{
    ML_TRACE_IN("IndexPair &IndexPair::operator = (const IndexPair &pair_)");
  
    BaseItem::operator = (pair_);
    index1  = pair_.index1;
    index2  = pair_.index2;
    type    = pair_.type;
    
    return *this;
}

//! Return a string representation of the item object. The string
//! is allocated on the heap and is later disposed by clearPersistentState().
char *IndexPair::getPersistentState() const
{
    ML_TRACE_IN("char *IndexPair::getPersistentState() const");

    ML_TRY{
      std::ostringstream ost;

      ost << '(' << index1 << ' ' << index2 << ')';
       
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
void IndexPair::setPersistentState(const char *state)
{
    ML_TRACE_IN("void IndexPair::setPersistentState(const char *state)");

    IndexPairParser parser;
    std::string newName;
    
    int parserResult = parser.init(state);
    if (!parserResult){ parserResult = parser.getIndices(index1, index2);           }  
    if (!parserResult){ parserResult = parser.getType(type);                        }
    if (!parserResult){ MLssize_t id=0; parserResult = parser.getId(id); setId(id); }
    if (!parserResult){ parserResult = parser.getName(newName);                     }
    if (!parserResult){ setName(newName.c_str());                                   }
    
    if (parserResult > 0){
        ML_PRINT_ERROR("IndexPair::setPersistentState()", ML_EMPTY_MESSAGE,
        parser.getErrorMessage(parserResult));
    }
    
}

//! Add state to tree
void IndexPair::addStateToTree(TreeNode *parent) const
{
    ML_TRACE_IN("void IndexPair::addStateToTree(TreeNode *parent) const");

    ML_TRY{
      ML_ADDSTATE_VERSION(IndexPair);
      ML_ADDSTATE_SUPER(BaseItem);
      parent->addChild(index1, "index1");
      parent->addChild(index2, "index2");
      if (type) { parent->addChild(type, "type"); }
    }
    ML_CATCH_RETHROW;
}

//! Read state from tree
void IndexPair::readStateFromTree(TreeNode *parent)
{
    ML_TRACE_IN("void IndexPair::readStateFromTree(TreeNode *parent)");

    ML_TRY{
      int version = parent->getVersion("IndexPair");
      if (version > 0) { throw TreeNodeException(TNE_UnsupportedClassVersion); }
    
      ML_READSTATE_SUPER(BaseItem);
    
      parent->readChild(index1, "index1");
      parent->readChild(index2, "index2");
      ML_READCHILD_OPTIONAL(type, "type", 0);
    }
    ML_CATCH_RETHROW;
}



// ------------------------------------------------------------------
// --- Class IndexPairList
// ------------------------------------------------------------------

ML_CLASS_SOURCE(IndexPairList, ListBase)

// Nothing else to do here.



// ------------------------------------------------------------------
// --- Class IndexPairListContainer
// ------------------------------------------------------------------

ML_CLASS_SOURCE(IndexPairListContainer, ListContainerBase);


//! Constructor
IndexPairListContainer::IndexPairListContainer()
: ListContainerTemplate<IndexPairList>(0, 0)
{
    ML_TRACE_IN("IndexPairListContainer::IndexPairListContainer(): ListContainerTemplate<IndexPairList>(0, 0)");

    FieldContainer *fields = getFieldContainer();
    
    // Suppress handleNotification()
    _lockNotification = 1;
    
    // Index fields
    (_fldIndex1 = fields->addInt("index1"))   ->setIntValue(0);
    (_fldIndex2   = fields->addInt("index2")) ->setIntValue(0);
    
    // Type field
    (_fldType = fields->addInt("type"))->setIntValue(0);
    
    // Template index fields
    (_fldNewIndex1 = fields->addInt("newIndex1"))   ->setIntValue(0);
    (_fldNewIndex2   = fields->addInt("newIndex2")) ->setIntValue(0);

    // Template Type field
    (_fldNewType = fields->addInt("newType"))->setIntValue(0);

    // Template Enable handleNotification()
    _lockNotification = 0;
}


//! Return true if \a field points to an item property field
bool IndexPairListContainer::isPropertyField(Field *field)
{
    ML_TRACE_IN("bool IndexPairListContainer::isPropertyField(Field *field)");

    return ( (field == _fldIndex1) || (field == _fldIndex2) || (field == _fldType) || ListContainerBase::isPropertyField(field) );
}


//! Reset all property fields
void IndexPairListContainer::resetPropertyFields()
{
    ML_TRACE_IN("void IndexPairListContainer::resetPropertyFields()");

    _lockNotification++;
    
    // Index fields
    _fldIndex1 ->setIntValue(0);
    _fldIndex2   ->setIntValue(0);
    
    // Type field
    _fldType->setIntValue(0);
    
    // Base class fields
    ListContainerTemplate<IndexPairList>::resetPropertyFields();
    
    _lockNotification--;
}


//! Update property fields from the current list item
void IndexPairListContainer::updatePropertyFields()
{
    ML_TRACE_IN("void IndexPairListContainer::updatePropertyFields()");

    IndexPair *pair=NULL;
    MLssize_t index = mlrange_cast<MLssize_t>(_fldIndex->getIntValue());
    
    // Valid list and item?
    if (_listPtr && (index >= 0) && (index < mlrange_cast<MLssize_t>(_listPtr->size())) )
    {
        // Suppress handleNotification()
        _lockNotification++;
        
        // Pointer to list item
        pair = &(*_listPtr)[mlrange_cast<size_t>(index)];
        
        if(_fldIndex1->getIntValue() != pair->index1) {
          _fldIndex1->setIntValue(pair->index1);
        }

        if(_fldIndex2->getIntValue() != pair->index2) {
          _fldIndex2->setIntValue(pair->index2);
        }

        if(_fldType->getIntValue() != pair->type) {
          _fldType->setIntValue(pair->type);
        }
        
        // Update base class fields
        ListContainerTemplate<IndexPairList>::updatePropertyFields();
        
        // Enable handleNotification()
        _lockNotification--;
    }
}


//! Modify item at position \a index. If \a fromCurrentItem is true, the
//! item is copied from _currentItem, otherwise the item property corresponding
//! to the property field \a field is modified.
//! Return true if item has been modified.
bool IndexPairListContainer::modifyItem(MLssize_t index, Field *field, bool fromCurrentItem)
{
    ML_TRACE_IN("bool IndexPairListContainer::modifyItem(MLssize_t index, Field *field, bool fromCurrentItem)");

    ML_TRY{
      bool modified = true;
      IndexPair *pair=NULL;
    
      // Valid list and item?
      if (_listPtr && (index >= 0) && (index < mlrange_cast<MLssize_t>(_listPtr->getSize())) )
      {
          // Modify from property field?
          if (!fromCurrentItem)
          {
              // Pointer to list item
              pair = &(*_listPtr)[mlrange_cast<size_t>(index)];
            
              // Position fields
              if ( (field == _fldIndex1) || (field == _fldIndex2) )
              {
                  pair->index1 = _fldIndex1->getIntValue();
                  pair->index2 = _fldIndex2->getIntValue();

              } else if (field == _fldType){
                  // Type field
                  pair->type = _fldType->getIntValue();
                
              } else {
                  // Not an IndexPair-specific field, pass call to base class
                  modified = ListContainerTemplate<IndexPairList>::modifyItem(index, field, fromCurrentItem);
              }
          } else {
              // Modify from _currentItem, handled in base class
              modified = ListContainerTemplate<IndexPairList>::modifyItem(index, field, fromCurrentItem);
          }
      } else {
          modified = false;
      }
    
      return modified;
    }
    ML_CATCH_RETHROW;
}

//! Copy values from item at position \a index to the template fields.
void IndexPairListContainer::copyItemToTemplate(MLssize_t index)
{
  ML_TRACE_IN("void IndexPairListContainer::copyItemToTemplate(MLssize_t index)");

  ML_TRY{
    IndexPair *pair = &(*_listPtr)[mlrange_cast<size_t>(index)];

    // IndexPair-specific fields
    _fldNewIndex1 ->setIntValue( pair->index1 );
    _fldNewIndex2 ->setIntValue( pair->index2 );
    _fldNewType   ->setIntValue( pair->type );

    // Base class fields
    ListContainerTemplate<IndexPairList>::copyItemToTemplate(index);
  }
  ML_CATCH_RETHROW;
}

//! Copy the values of the templates fields to \a _currentItem.
void IndexPairListContainer::copyTemplateToCurrent()
{
  ML_TRACE_IN("void IndexPairListContainer::copyTemplateToCurrent()");

  ML_TRY{
    // IndexPair-specific fields
    _currentItem.index1 = _fldNewIndex1->getIntValue();
    _currentItem.index2 = _fldNewIndex2->getIntValue();
    _currentItem.type   = _fldNewType->getIntValue();

    // Base class fields
    ListContainerTemplate<IndexPairList>::copyTemplateToCurrent();
  }
  ML_CATCH_RETHROW;
}

//! Initialize the list item at position \a index. This method is called by
//! insertItem() if the \a fromCurrentItem argument is false.
//! Moreover it is called when fldInit is touched.
void IndexPairListContainer::initItem(MLssize_t index)
{
    ML_TRACE_IN("void IndexPairListContainer::initItem(MLssize_t index)");

    ML_TRY{
      IndexPair *pair = &(*_listPtr)[mlrange_cast<size_t>(index)];

      // IndexPair-specific fields
      pair->index1 = 0;
      pair->index2 = 0;
      pair->type   = 0;
    
      // Base class fields
      ListContainerTemplate<IndexPairList>::initItem(index);
    }
    ML_CATCH_RETHROW;
}

//! Parse and return position
int IndexPairParser::getIndices(MLint &index1, MLint &index2)
{
    ML_TRACE_IN("int IndexPairParser::getIndices(MLint &index1, MLint &index2)");

    MLint values[2];
    values[0] = 0;
    values[1] = 0;
    
    int result = getIntArray(2, 2, values);
    
    index1 = static_cast<MLint>(values[0]);
    index2 = static_cast<MLint>(values[1]);

    return result;
}

//! Parse and return type
int IndexPairParser::getType(MLint &type)
{
    ML_TRACE_IN("int IndexPairParser::getType(int &type)");

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
int IndexPairParser::getIntArray(int nMin, int nMax, MLint *values)
{

    ML_TRACE_IN("int IndexPairParser::getArray(int nMin, int nMax, MLint *values)");

    ML_TRY{
      int result = 0, n=0;
      unsigned long x=0;
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
                      x = strtoul(_pNext, &endPtr, 10);
                      if (_pNext == endPtr){
                          break;

                      } else {
                          // Proceed behind number and store number array
                          _pNext = endPtr;
                          values[n] = static_cast<MLint>(x);
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
const char *IndexPairParser::getErrorMessage(int errorCode)
{
    ML_TRACE_IN("const char *IndexPairParser::getErrorMessage(int errorCode)");

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

