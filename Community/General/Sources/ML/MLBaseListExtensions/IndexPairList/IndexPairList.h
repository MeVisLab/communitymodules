//----------------------------------------------------------------------------------
//! This class provides an IndexPair(List)
/*!
// \file    IndexPairList.h
// \author  Axel Newe
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __IndexPairList_H
#define __IndexPairList_H



// ML includes
#ifndef __MLBaseListExtensionsSystem_H
#include "MLBaseListExtensionsSystem.h"
#endif
#ifndef __mlModuleIncludes_H
#include "mlModuleIncludes.h"
#endif
#ifndef __mlLinearAlgebra_H
#include "mlLinearAlgebra.h"
#endif
#ifndef __mlTreeNode_H
#include "mlTreeNode.h"
#endif


// Local includes
#ifndef __mlListBase_H
#include "mlListBase.h"
#endif
#ifndef __mlListContainer_H
#include "mlListContainer.h"
#endif



ML_START_NAMESPACE

  
// ------------------------------------------------------------------
//! Base object class IndexPair (derived form baseItem) with
//! index1 int , index2 int and type int.
// ------------------------------------------------------------------

class MLBASELISTEXTENSIONS_EXPORT IndexPair : public BaseItem
{
public:

  //! \name IndexPair properties
  //@{

  MLint index1;  //!< Index 1
  MLint index2;  //!< Index 2
  MLint type;    //!< IndexPair type

  //@}


  //! \name Constructors
  //@{

  //! Default constructor
  IndexPair () : index1(0), index2(0), type(0) {}
  //! Indices given
  IndexPair (const int &start_, const int &end_) : index1(start_), index2(end_), type(0) {}
  //! Indices & type given
  IndexPair (const int start_, const int end_, int type_) : index1(start_), index2(end_), type(type_) {}
  //! Indices, type and name given
  IndexPair (const int start_, const int end_, int type_, const char *name_) : index1(start_), index2(end_), type(type_) { setName(name_); }

  //! Copy constructor
  IndexPair (const IndexPair &pair_) : BaseItem() { *this = pair_; }

  //@}


  //! Assignment operator
  virtual IndexPair &operator = (const IndexPair &pair_);

  //! Dummy "less than operator", required for VC7
  bool operator <(const IndexPair &) const { return false; }

  //! Equal operator
  bool operator ==(const IndexPair &pair_) const { return (index1 ==  pair_.index1) && (index2 == pair_.index2) && (type == pair_.type); }

  //! \name Persistence
  //@{

  //! Return a string representation of the item object. The string
  //! is allocated on the heap and is later disposed by clearPersistentState().
  virtual char *getPersistentState () const;

  //! Initialize the item object from the string \a state
  virtual void setPersistentState (const char *state);

  //! Add state to tree
  virtual void addStateToTree(TreeNode *parent) const;

  //! Set addState version number
  ML_SET_ADDSTATE_VERSION(0);

  //! Read state from tree
  virtual void readStateFromTree(TreeNode *parent);

  //@}


private:

  //! Implements the interface for the runtime type system of the ML.
  ML_CLASS_HEADER(IndexPair);

};



// ------------------------------------------------------------------
//! Base object class IndexPairList (derived from BaseListTemplate)
//! specialized for IndexPair items
// ------------------------------------------------------------------

/*! List class for IndexPair items
*/
class MLBASELISTEXTENSIONS_EXPORT IndexPairList : public BaseListTemplate<IndexPair>
{
public:

  //! Constructor, enables persistence
  IndexPairList () : BaseListTemplate<IndexPair>(true) {}

  /*! addState is inherited from BaseListTemplate. So the ADDSTATE_VERSION will also
      grow with the version of BaseListTemplate. If a reimplementation of addState
      becomes eventually necessary, the version has to be set to the BaseListTemplate
      version number + 1 (and readState is to be held compatible to the previous
      version files)*/

private:

  //! Implements the interface for the runtime type system of the ML.
  ML_CLASS_HEADER(IndexPairList);

};


// ------------------------------------------------------------------
//! Base object class IndexPairListContainer (derived from
//! ListContainerTemplate) for IndexPairList objects
// ------------------------------------------------------------------

/*! Container class for IndexPairList objects
*/
class MLBASELISTEXTENSIONS_EXPORT IndexPairListContainer : public ListContainerTemplate<IndexPairList>
{
public:

  //! Constructor
  IndexPairListContainer ();


protected:

  //! \name Handle item property fields
  //@{
  //! Return true if \a field points to an item property field
  virtual bool isPropertyField (Field *field);

  //! Reset all property fields
  virtual void resetPropertyFields();

  //! Update property fields from the current list item
  virtual void updatePropertyFields();

  //@}


  //! \name List modifications
  //@{
  //! Modify item at position \a index. If \a fromCurrentItem is true, the
  //! item is copied from _currentItem, otherwise the item property corresponding
  //! to the property field \a field is modified.
  //! Return true if item has been modified.
  virtual bool modifyItem (MLssize_t index, Field *field, bool fromCurrentItem);

  //! Copy values from item at position \a index to the template fields.
  virtual void copyItemToTemplate (MLssize_t index);

  //! Copy the values of the templates fields to \a _currentItem.
  virtual void copyTemplateToCurrent ();

  //! Initialize the list item at position \a index. This method is called by
  //! insertItem() if the \a fromCurrentItem argument is false.
  //! Moreover it is called when fldInit is touched.
  virtual void initItem (MLssize_t index);

  //@}


private:

  //! \name Field members
  //@{

  IntField *_fldIndex1;   //!< Index 1
  IntField *_fldIndex2;   //!< Index 2
  IntField *_fldType;     //!< IndexPair type

  // template fields for initialization of a newly inserted item

  IntField *_fldNewIndex1;  //!< New Index 1
  IntField *_fldNewIndex2;  //!< New Index 2
  IntField *_fldNewType;    //!< New IndexPair type

  //@}


  //! Implements the interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(IndexPairListContainer);

};



// ------------------------------------------------------------------
// --- Class IndexPairParser
// ------------------------------------------------------------------
/*! Helper class for parsing the representation of an IndexPair object
according to the following syntax:
(<start> <end>)[<type>][#<id>][[:]<name>]
<start>    = integer
<end>      = integer
<type>     = integer
<id>       = integer
<name>     = string
*/
class IndexPairParser : public BaseItemParser
{
public:
    
    //! Constructor
    IndexPairParser () : BaseItemParser() {}
    
    //! Parse and return start and end indices
    int getIndices(MLint &index1, MLint &index2);

    //! Parse and return type
    int getType(MLint &type);
    
    //! Get error string for \a errorCode
    virtual const char *getErrorMessage (int errorCode);
    
    //! Error codes
    enum {
        kMissingLeftParenthesis = kNumBaseItemErrorCodes,
            kMissingRightParenthesis,
            kTooFewNumbers,
            kNumErrorCodes
    };
    
    
protected:
    
    //! Parse n-tupel with a minimum of \a nMin and a maximum of \a nMax
    //! float numbers, separated by whitespace and enclosed in parenthesis.
    //! Values are stored in \a values[0..].
    int getIntArray (int nMin, int nMax, MLint *values);
    
};


ML_END_NAMESPACE

#endif // __IndexPairList_H
