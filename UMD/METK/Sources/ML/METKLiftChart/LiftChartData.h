//----------------------------------------------------------------------------------
//! Classes related to Lift Chart modules.
/*!
// \file    LiftChartData.h
// \author  Christian Tietjen
// \date    2007-01-17
*/
//----------------------------------------------------------------------------------

#ifndef __LIFTCHARTDATA_H
#define __LIFTCHARTDATA_H

#include "METKLiftChartSystem.h"

class Structure;
class Bookmark;

typedef std::vector<Structure*>::const_iterator T_ConstStructureIterator;
typedef std::vector<Bookmark*>::const_iterator  T_ConstBookmarkIterator;

typedef std::vector<Structure*>::iterator T_StructureIterator;
typedef std::vector<Bookmark*>::iterator  T_BookmarkIterator;

///////////////////////////////////////////////////////////////////////
//
//! Class LiftChartData
//
///////////////////////////////////////////////////////////////////////

class LiftChartData {
public:

   LiftChartData();
   ~LiftChartData();

   void sortStructures();

   Structure* addStructure();
   Bookmark*  addBookmark();

   Structure* getStructure(const std::string name);
   Bookmark*  getBookmark(const std::string name);

   const bool getStructureVisible(T_ConstStructureIterator structure);

   enum Aggregation {AGGREGATE_ALL, AGGREGATE_STRUCTURE, AGGREGATE_STRUCTUREGROUP, AGGREGATE_NONE};

   void setAggregation(Aggregation aggregation);
   Aggregation getAggregation();

   //! Set/Get minimum allowed importance of a single structure in percent.
   void setMinImportance(const float importance);
   const float getMinImportance() const;

   //! Set/Get maximum allowed extent of a single structure in percent.
   void setMaxExtent(const float extent);
   const float getMaxExtent() const;

   //! Set/Get division by side (Left/Ambilateral/Right). If no tag is set (Unknown), use the function below.
   void setDivideBySide(const bool divideBySide);
   const bool getDivideBySide() const;

   //! Set/Get display of structures with Side tag (Unknown).
   void setHideSideUnkown(const bool hideSideUnkown);
   const bool getHideSideUnkown() const;

   //! Get/Set the extent of the overall dataset. This is needed for the computation of the reletive size of the single structures.
   const float* getMin() const;
   const float* getMax() const;
   void setExtension(const float* start, const float* end);

   void cleanup();

   int numStructures();
   int numBookmarks();

   //! Returns a const_iterator pointing to the beginning of the Column container
   inline T_ConstStructureIterator structureBegin() {
      return _structureVec.begin();
   }
   //! Returns a const_iterator pointing to the end of the Column container
   inline T_ConstStructureIterator structureEnd() {
      return _structureVec.end();
   }

   //! Returns a const_iterator pointing to the beginning of the sorted container
   inline T_ConstStructureIterator structureSortedBegin() {
      return _orderedStructureVec.begin();
   }
   //! Returns a const_iterator pointing to the end of the sorted container
   inline T_ConstStructureIterator structureSortedEnd() {
      return _orderedStructureVec.end();
   }

   //! Returns a const_iterator pointing to the beginning of the Bookmark container
   inline T_ConstBookmarkIterator bookmarkBegin() {
      return _bookmarkVec.begin();
   }
   //! Returns a const_iterator pointing to the end of the Bookmark container
   inline T_ConstBookmarkIterator bookmarkEnd() {
      return _bookmarkVec.end();
   }

private:

   //! vector containing all structures
   std::vector<Structure*> _structureVec;

   //! vector containing all structures in sorted order
   std::vector<Structure*> _orderedStructureVec;

   //! vector containing all bookmarks
   std::vector<Bookmark*> _bookmarkVec;

   Aggregation _aggregation;
   float _start[3], _end[3];

   float _minImportance;
   float _maxExtent;

   bool _divideBySide;
   bool _hideSideUnkown;
};

///////////////////////////////////////////////////////////////////////
//
//! Class LiftChartDataItem used as base class, for example for Structure and Bookmark.
//
///////////////////////////////////////////////////////////////////////

class LiftChartDataItem {
public:

   LiftChartDataItem();
   ~LiftChartDataItem();

   void  setImportance(const float imp);
   float getImportance();
   const bool getVisible() const;

   enum Side {LEFT, RIGHT, AMBILATERAL, UNKNOWN};
   void setSide(Side side);
   Side getSide();

   void  setName(const std::string name);
   std::string getName();

private:
   Side _side;
   float _importance; // 0..1
   std::string _name;
};


///////////////////////////////////////////////////////////////////////
//
//! Class Structure derived from LiftChartDataItem.
//
///////////////////////////////////////////////////////////////////////

class Structure : public LiftChartDataItem {
public:

   Structure();
   ~Structure();

   const float* getMin() const;
   const float* getMax() const;
   void setExtension(const float* start, const float* end);

   const float* getRGBA();
   void setRGBA(float* rgba);
   void setVisible(const bool visible);
   const bool getVisible() const;

   void setStructure(const std::string _structure);
   const std::string getStructure() const;
   void setStructureGroup(const std::string _structure);
   const std::string getStructureGroup() const;
   void setPickStyle(const bool _pickStyle = false);

private:
   float _start[3], _end[3];
   float _rgba[4];

   std::string _structure;
   std::string _structureGroup;
   bool _pickStyle;
   bool _visible;

};


///////////////////////////////////////////////////////////////////////
//
//! Class Bookmark derived from LiftChartDataItem.
//
///////////////////////////////////////////////////////////////////////

class Bookmark : public LiftChartDataItem {
public:

   Bookmark();
   ~Bookmark();

   void setPosition(float* pos);
   const float* getPosition();

private:
   float _position[3];
};

#endif