#include "LiftChartData.h"

///////////////////////////////////////////////////////////////////////
//
//  class LiftChartData
//
///////////////////////////////////////////////////////////////////////

LiftChartData::LiftChartData() {
	_maxExtent = 1;
	_minImportance = 0;
	_divideBySide = false;
	_hideSideUnkown = false;
}


LiftChartData::~LiftChartData() {
	cleanup();
}


void LiftChartData::cleanup() {
	T_StructureIterator strIter;
	T_BookmarkIterator bookIter;

	for (strIter = _structureVec.begin(); strIter != _structureVec.end(); strIter++) {
		delete *strIter;
	}
	_structureVec.clear();
	_orderedStructureVec.clear();
	_bookmarkVec.clear();
}


Bookmark* LiftChartData::addBookmark() {
	Bookmark* book = new Bookmark();
	_bookmarkVec.push_back(book);
	return book;
}


Structure* LiftChartData::addStructure() {
	Structure* str = new Structure();
	_structureVec.push_back(str);
	return str;
}


Structure* LiftChartData::getStructure(const std::string name) {
	T_StructureIterator strIter;
	for (strIter = _structureVec.begin(); strIter != _structureVec.end(); strIter++) {
		if ((*strIter)->getName() == name)
			return *strIter;
	}
	return 0;
}


Bookmark* LiftChartData::getBookmark(const std::string name) {
	T_BookmarkIterator bookIter;
	for (bookIter = _bookmarkVec.begin(); bookIter != _bookmarkVec.end(); bookIter++) {
		if ((*bookIter)->getName() == name)
			return *bookIter;
	}
	return 0;
}


const bool LiftChartData::getStructureVisible(T_ConstStructureIterator structure) {
	if ((*structure)->getVisible() && (*structure)->getImportance() >= _minImportance)
		return true;
	return false;
}

void LiftChartData::setAggregation(Aggregation aggregation) {
	_aggregation = aggregation;
}


LiftChartData::Aggregation LiftChartData::getAggregation() {
	return _aggregation;
}


void LiftChartData::setMinImportance(const float importance) {
	_minImportance = importance;
}


const float LiftChartData::getMinImportance() const {
	return _minImportance;
}


void LiftChartData::setMaxExtent(const float extent) {
	_maxExtent = extent;
}


const float LiftChartData::getMaxExtent() const {
	return _maxExtent;
}

//! Set/Get division by side (Left/Ambilateral/Right). If no tag is set (Unknown), use the function below.
void LiftChartData::setDivideBySide(const bool divideBySide) {
	_divideBySide = divideBySide;
}

const bool LiftChartData::getDivideBySide() const {
	return _divideBySide;
}


//! Set/Get display of structures with Side tag (Unknown).
void LiftChartData::setHideSideUnkown(const bool hideSideUnkown) {
	_hideSideUnkown = hideSideUnkown;
}

const bool LiftChartData::getHideSideUnkown() const {
	return _hideSideUnkown;
}


const float* LiftChartData::getMin() const {
	return _start;
}


const float* LiftChartData::getMax() const {
	return _end;
}


void LiftChartData::setExtension(const float* start, const float* end) {
	_start[0] = *start;
	_start[1] = *++start;
	_start[2] = *++start;

	_end[0] = *end;
	_end[1] = *++end;
	_end[2] = *++end;
}


int LiftChartData::numStructures() {
	return _structureVec.size();
}


int LiftChartData::numBookmarks() {
	return _bookmarkVec.size();
}


void LiftChartData::sortStructures() {
	/*
	Faktoren: Trennung nach Seite, Sichtbarkeit (_minImportance, _hideSideUnkown), Wichtigkeit, Structure(Group)
	*/
	T_StructureIterator strIter, iter;

	// alten sortierten Vector loeschen
	_orderedStructureVec.clear();

	// Kopie des _structureVec anlegen
	std::vector<Structure*> unsortedVec;
	for (strIter = _structureVec.begin(); strIter != _structureVec.end(); strIter++) {
		// Ueberhaupt sichtbar?
		if ((*strIter)->getVisible() && (_hideSideUnkown && (*strIter)->getSide() != Structure::UNKNOWN || _hideSideUnkown == false) && ((*strIter)->getImportance() > _minImportance))
			unsortedVec.push_back((*strIter));
	}
	//std::cout << "----------------------------------------------" << std::endl;
	while (unsortedVec.size()) {
		iter = unsortedVec.begin();
		// Ausgangsstruktur mit Liste vergleichen
		// "kleinste" Struktur suchen, im _orderedStructureVec ablegen und aus unsortedVec loeschen

		for (strIter = ++unsortedVec.begin(); strIter != unsortedVec.end(); strIter++) {
			// Iterator initialisieren

			// hier geht die Sortierung los
			// wenn alle Strukturen in einer Spalte dargestellt werden sollen, ist nur die Rendering-Reihenfolge wichtig, also Importance
			if (_aggregation != AGGREGATE_ALL) {
				// Sortierung nach Seite (wenn aktiviert)
				if (_divideBySide) {
					if ((*iter)->getSide() == Structure::RIGHT) {
						// do nothing
					} else {
						if ((*strIter)->getSide() == Structure::RIGHT) {
							iter = strIter;
						} else {
							if ((*iter)->getSide() == Structure::LEFT) {
								if ((*strIter)->getSide() != Structure::LEFT) {
									iter = strIter;
								}
							}
						}
					}
				}

				// Sortierung vorgenommen? Wenn nicht...
				// Sortierung nach Name (invertiert bei rechter Seite)
				if (_divideBySide) {
					if ((*iter)->getSide() == (*strIter)->getSide()) {
						if ((*strIter)->getSide() == Structure::RIGHT) {
							// keine sonstige Zusammenfassung
							if ((*strIter)->getStructureGroup() < (*iter)->getStructureGroup()) {
								iter = strIter;
							} else {
								if ((*strIter)->getStructureGroup() == (*iter)->getStructureGroup()) {
									if ((*strIter)->getStructure() < (*iter)->getStructure()) {
										iter = strIter;
									} else {
										if ((*strIter)->getStructure() == (*iter)->getStructure()) {
											if ((*strIter)->getName() < (*iter)->getName())
												iter = strIter;
										}
									}
								}
							}
						} else {
							if ((*strIter)->getStructureGroup() > (*iter)->getStructureGroup()) {
								iter = strIter;
							} else {
								if ((*strIter)->getStructureGroup() == (*iter)->getStructureGroup()) {
									if ((*strIter)->getStructure() > (*iter)->getStructure()) {
										iter = strIter;
									} else {
										if ((*strIter)->getStructure() == (*iter)->getStructure()) {
											if ((*strIter)->getName() > (*iter)->getName())
												iter = strIter;
										}
									}
								}
							}
						}
					}
				} else {
					if ((*strIter)->getStructureGroup() < (*iter)->getStructureGroup()) {
						iter = strIter;
					} else {
						if ((*strIter)->getStructureGroup() == (*iter)->getStructureGroup()) {
							if ((*strIter)->getStructure() < (*iter)->getStructure()) {
								iter = strIter;
							} else {
								if ((*strIter)->getStructure() == (*iter)->getStructure()) {
									if ((*strIter)->getName() < (*iter)->getName())
										iter = strIter;
								}
							}
						}
					}
				}
			}

			// Sortierung vorgenommen? Wenn nicht...
			// Sortierung nach Importance
			if ((*iter)->getStructure() == (*strIter)->getStructure()) {
				if ((*iter)->getImportance() > (*strIter)->getImportance())
					iter = strIter;
			}
		}
		_orderedStructureVec.push_back((*iter));
		//std::cout << (*iter)->getName() << " " << (*iter)->getImportance() << " " << (*iter)->getSide() << std::endl;
		unsortedVec.erase(iter);
	}
}


///////////////////////////////////////////////////////////////////////
//
//  class LiftChartItem
//
///////////////////////////////////////////////////////////////////////

LiftChartDataItem::LiftChartDataItem() {
	_importance = 0;
}


LiftChartDataItem::~LiftChartDataItem() {
}


void LiftChartDataItem::setImportance(const float imp) {
	_importance = imp;
}


float LiftChartDataItem::getImportance() {
	return _importance;
}


const bool LiftChartDataItem::getVisible() const {
	return true;
}


void LiftChartDataItem::setSide(Side side) {
	_side = side;
}


LiftChartDataItem::Side LiftChartDataItem::getSide() {
	return _side;
}


void LiftChartDataItem::setName(const std::string name) {
	_name = name;
}

std::string LiftChartDataItem::getName() {
	return _name;
}


///////////////////////////////////////////////////////////////////////
//
//  class Structure
//
///////////////////////////////////////////////////////////////////////


Structure::Structure() {
	_rgba[0] = 0.8f;
	_rgba[1] = 0.8f;
	_rgba[2] = 0.8f;
	_rgba[3] = 1.0f;
	_visible = false;
}


Structure::~Structure() {
}


void Structure::setExtension(const float* start, const float* end) {
	_start[0] = *start;
	_start[1] = *++start;
	_start[2] = *++start;

	_end[0] = *end;
	_end[1] = *++end;
	_end[2] = *++end;
}


const float* Structure::getMin() const {
	return _start;
}


const float* Structure::getMax() const {
	return _end;
}



const float* Structure::getRGBA() {
	return _rgba;
}


void Structure::setRGBA(float* rgba) {
	_rgba[0] = *rgba;
	_rgba[1] = *++rgba;
	_rgba[2] = *++rgba;
	_rgba[3] = *++rgba;
}


void Structure::setVisible(const bool visible) {
	_visible = visible;
}


const bool Structure::getVisible() const {
	return _visible;
}


void Structure::setStructure(const std::string structure) {
	_structure = structure;
}


const std::string Structure::getStructure() const {
	return _structure;
}


void Structure::setStructureGroup(const std::string structureGroup) {
	_structureGroup = structureGroup;
}


const std::string Structure::getStructureGroup() const {
	return _structureGroup;
}


void Structure::setPickStyle(const bool pickStyle) {
	_pickStyle = pickStyle;
}


///////////////////////////////////////////////////////////////////////
//
//  class Bookmark
//
///////////////////////////////////////////////////////////////////////


Bookmark::Bookmark() {
}


Bookmark::~Bookmark() {
}


void Bookmark::setPosition(float* pos) {
	_position[0] = *pos;
	_position[1] = *++pos;
	_position[2] = *++pos;
}


const float* Bookmark::getPosition() {
	return _position;
}
