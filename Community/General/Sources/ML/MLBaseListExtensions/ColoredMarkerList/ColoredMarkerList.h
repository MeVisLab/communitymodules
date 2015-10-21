//----------------------------------------------------------------------------------
//! This class provides an ColoredMarker(List)
/*!
// \file    ColoredMarkerList.h
// \author  Axel Newe
// \date    2014-09-01
//
*/
//----------------------------------------------------------------------------------


#ifndef __ColoredMarkerList_H
#define __ColoredMarkerList_H



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

#include <mlXMarkerList.h>

ML_START_NAMESPACE

  
// ------------------------------------------------------------------
//! Base object class ColoredMarker (derived form baseItem) with
//! index1 int , index2 int and type int.
// ------------------------------------------------------------------

class MLBASELISTEXTENSIONS_EXPORT ColoredMarker : public BaseItem
{
public:

  //! \name ColoredMarker properties
  //@{

  Vector6 pos;     //!< Marker position 
  Vector4 color;   //!< RGBA color
  MLint type;      //!< ColoredMarker type

  //@}


  //! \name Constructors
  //@{

  //! Default constructor
  ColoredMarker() : pos(0), color(0), type(0) {}
  //! Position given

  ColoredMarker(const Vector3 &pos_) : pos(pos_,0,0,0), color(0), type(0) {}
  //! Position & type given
  ColoredMarker(const Vector3 &pos_, int type_) : pos(pos_,0,0,0), color(0), type(type_) {}
  //! Position & color given
//  ColoredMarker(const Vector3 &pos_, const Vector3 &color_) : pos(pos_,0,0,0), color(color_[0],color_[1],color_[2],0), type(0) {}
  //! Position, color & alpha given
  ColoredMarker(const Vector3 &pos_, const Vector3 &color_, MLdouble alpha_) : pos(pos_,0,0,0), color(color_[0],color_[1],color_[2],alpha_), type(0) {}
  //! Position, color & type given
  ColoredMarker(const Vector3 &pos_, const Vector3 &color_, int type_) : pos(pos_,0,0,0), color(color_[0],color_[1],color_[2],0), type(type_) {}
  //! Position, color, alpha & type given
  ColoredMarker(const Vector3 &pos_, const Vector3 &color_, MLdouble alpha_, int type_) : pos(pos_,0,0,0), color(color_[0],color_[1],color_[2],alpha_), type(type_) {}
  //! Copy constructor
  ColoredMarker(const ColoredMarker &marker_) : BaseItem() { *this = marker_; }

  //@}


  //! Assignment operator
  virtual ColoredMarker &operator = (const ColoredMarker &marker_);

  //! Dummy "less than operator", required for VC7
  bool operator <(const ColoredMarker &) const { return false; }

  //! Equal operator
  bool operator ==(const ColoredMarker &marker_) const  { return (pos ==  marker_.pos) && (color == marker_.color) && (type == marker_.type); }

  //! \name Direct coordinate access
  //@{

  double  &x() { return pos[ML_VX]; }
  double  &y() { return pos[ML_VY]; }
  double  &z() { return pos[ML_VZ]; }
  double  &c() { return pos[ML_VC]; }
  double  &t() { return pos[ML_VT]; }
  double  &u() { return pos[ML_VU]; }

  double  &r()      { return color[0]; }
  double  &g()      { return color[1]; }
  double  &b()      { return color[2]; }
  double  &alpha()  { return color[3]; }
  Vector4 &rgba()   { return color; }

  const double &x() const { return pos[ML_VX]; }
  const double &y() const { return pos[ML_VY]; }
  const double &z() const { return pos[ML_VZ]; }
  const double &c() const { return pos[ML_VC]; }
  const double &t() const { return pos[ML_VT]; }
  const double &u() const { return pos[ML_VU]; }

  const double  &r()       const { return color[0]; }
  const double  &g()       const { return color[1]; }
  const double  &b()       const { return color[2]; }
  const double  &alpha()   const { return color[3]; }
  const Vector4 &rgba()    const { return color; }

  //@}

  //! \name Persistence
  //@{

  //! Return a string representation of the item object. The string
  //! is allocated on the heap and is later disposed by clearPersistentState().
  virtual char *getPersistentState() const;

  //! Initialize the item object from the string \a state
  virtual void setPersistentState(const char *state);

  //! Add state to tree
  virtual void addStateToTree(TreeNode *parent) const;

  //! Set addState version number
  ML_SET_ADDSTATE_VERSION(0);

  //! Read state from tree
  virtual void readStateFromTree(TreeNode *parent);

  //@}


private:

  //! Implements the interface for the runtime type system of the ML.
  ML_CLASS_HEADER(ColoredMarker);

};



// ------------------------------------------------------------------
//! Base object class ColoredMarkerList (derived from BaseListTemplate)
//! specialized for ColoredMarker items
// ------------------------------------------------------------------

/*! List class for ColoredMarker items
*/
class MLBASELISTEXTENSIONS_EXPORT ColoredMarkerList : public BaseListTemplate<ColoredMarker>
{
public:

  //! Constructor, enables persistence
  ColoredMarkerList() : BaseListTemplate<ColoredMarker>(true) {}

  /*! addState is inherited from BaseListTemplate. So the ADDSTATE_VERSION will also
      grow with the version of BaseListTemplate. If a reimplementation of addState
      becomes eventually necessary, the version has to be set to the BaseListTemplate
      version number + 1 (and readState is to be held compatible to the previous
      version files)*/

  // Convert to XMarkerList
  ml::XMarkerList toXMarkerList();

  // Convert from XMarkerList
  void fromXMarkerList(ml::XMarkerList importList, Vector4 rgbaColor);
  void fromXMarkerList(ml::XMarkerList importList, Vector3 rgbColor = Vector3(0), float alpha = 1);

private:

  //! Implements the interface for the runtime type system of the ML.
  ML_CLASS_HEADER(ColoredMarkerList);

};


// ------------------------------------------------------------------
//! Base object class ColoredMarkerListContainer (derived from
//! ListContainerTemplate) for ColoredMarkerList objects
// ------------------------------------------------------------------

/*! Container class for ColoredMarkerList objects
*/
class MLBASELISTEXTENSIONS_EXPORT ColoredMarkerListContainer : public ListContainerTemplate<ColoredMarkerList>
{
public:

  //! Constructor
  ColoredMarkerListContainer();


protected:

  //! \name Handle item property fields
  //@{
  //! Return true if \a field points to an item property field
  virtual bool isPropertyField(Field *field);

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
  virtual bool modifyItem(MLssize_t index, Field *field, bool fromCurrentItem);

  //! Copy values from item at position \a index to the template fields.
  virtual void copyItemToTemplate(MLssize_t index);

  //! Copy the values of the templates fields to \a _currentItem.
  virtual void copyTemplateToCurrent();

  //! Initialize the list item at position \a index. This method is called by
  //! insertItem() if the \a fromCurrentItem argument is false.
  //! Moreover it is called when fldInit is touched.
  virtual void initItem(MLssize_t index);

  //@}


private:

  //! \name Field members
  //@{

  Vector3Field *_fldPos3D;         //!< 3D Position
  FloatField   *_fldPosC,          //!< C-coordinate of position
               *_fldPosT,          //!< T-coordinate of position
               *_fldPosU;          //!< U-coordinate of position
  ColorField   *_fldColor;         //!< Color
  FloatField   *_fldAlpha;         //!< Alpha factor
  IntField     *_fldType;          //!< ColoredMarker type

  // template fields for initialization of a newly inserted item

  Vector3Field *_fldNewPos3D;      //!< New 3D Position
  FloatField   *_fldNewPosC,       //!< New C-coordinate of position
               *_fldNewPosT,       //!< New T-coordinate of position
               *_fldNewPosU;       //!< New U-coordinate of position
  ColorField   *_fldNewColor;      //!< New Color
  FloatField   *_fldNewAlpha;      //!< New Alpha factor
  IntField     *_fldNewType;       //!< New ColoredMarker type

  //@}


  //! Implements the interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(ColoredMarkerListContainer);

};



// ------------------------------------------------------------------
// --- Class ColoredMarkerParser
// ------------------------------------------------------------------
/*! Helper class for parsing the representation of an ColoredMarker object
according to the following syntax:
(<start> <end>)[<type>][#<id>][[:]<name>]
<start>    = integer
<end>      = integer
<type>     = integer
<id>       = integer
<name>     = string
*/
class ColoredMarkerParser : public BaseItemParser
{
public:
    
    //! Constructor
    ColoredMarkerParser() : BaseItemParser() {}
    
    //! Parse and return position
    int getPosition(Vector6 &pos);
    
    //! Parse and return color with alpha
    int getColor(Vector4 &color);
    
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
    int getArray(int nMin, int nMax, double *values);
    
};


ML_END_NAMESPACE

#endif // __ColoredMarkerList_H
