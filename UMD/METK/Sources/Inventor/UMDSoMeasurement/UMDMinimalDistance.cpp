
#include "UMDMinimalDistance.h"
#include <assert.h>

// #########################################
// #####
// #####  UMDMinimalDistance
// #####
// #########################################



UMDMinimalDistance::UMDMinimalDistance() {
  _pointSet1 = NULL;
  _pointSet2 = NULL;
  _size1 = 0;
  _size2 = 0;
  _sphereIndex = 0;
  _entries = -1;
  _partition = -1;
}


UMDMinimalDistance::~UMDMinimalDistance() {
}


void UMDMinimalDistance::computeDistance(float*& point1, float*& point2) {
  // wenn beide Mengen vorhanden sind
  if (_pointSet1 != NULL && _pointSet2 != NULL) {
    
    // Index initialisieren
    _sphereIndex = 0;
    
    // Kugel-Hierarchie aufbauen
    if (_verbose) {
      std::cout << "+-----------------------------------+" << std::endl;
      std::cout << "Tree One: " << std::endl;
    }
    TileSphere* tileSphere1 = generateTree(_pointSet1, _size1);
    if (_verbose)
      std::cout << "Tree Two: " << std::endl;
    TileSphere* tileSphere2 = generateTree(_pointSet2, _size2);
    
    // Gr�en aller Kugeln bestimmen
    tileSphere1->getDeFactoSize();
    tileSphere2->getDeFactoSize();
    
    // HashTable erstellen
    _hashTable = new HashTable();
    
    // Abstand berechnen
    float distance;
    float* p1 = NULL;
    float* p2 = NULL;
    // the step we're all waiting for:
    // computing the distance
    distance = tileSphere1->computeDistance(tileSphere2, p1, p2);
    assert(p1 != NULL);
    assert(p2 != NULL);
    // creates a copy of the points
    float* tmp = new float[3];
    tmp[0] = p1[0];
    tmp[1] = p1[1];
    tmp[2] = p1[2];
    point1 = tmp;
    tmp = new float[3];
    tmp[0] = p2[0];
    tmp[1] = p2[1];
    tmp[2] = p2[2];
    point2 = tmp;
    
    if (_verbose) {
      int addedPairs, hits, misses;
      _hashTable->getStatistics(addedPairs, hits, misses);
      std::cout << "Hash Table: " << std::endl;
      std::cout << "  Added Pairs:  " << addedPairs << std::endl;
      std::cout << "  Pairs Found:  " << hits << std::endl;
      std::cout << "  Pairs Missed: " << misses << std::endl << std::endl;
      std::cout << "Computed Distance: " << distance << std::endl;
      std::cout << "+-----------------------------------+" << std::endl << std::flush;
    }
    
    if (_displaySpheres) {
      _tileSphere1 = displaySpheres(tileSphere1);
      _tileSphere2 = displaySpheres(tileSphere2);
    }
    
    delete _hashTable;
    delete tileSphere1;
    delete tileSphere2;
  }
}


void UMDMinimalDistance::setPointSets(float* pointSet1, float* pointSet2, long size1, long size2) {
  _pointSet1 = pointSet1;
  _pointSet2 = pointSet2;
  _size1 = size1;
  _size2 = size2;
}


void UMDMinimalDistance::setError(float error) {
  if (error > 1) error = 1;
  if (error < 0) error = 0;
  _error = error;
}

TileSphere* UMDMinimalDistance::generateTree(float* pointSet, long size) {
  // gltige Menge?
  if (pointSet != NULL && size != 0) {
    // Parameter fr TileSphere berechnen
    // eine Kugel sollte nicht mehr als 2000 Eintr�e erhalten
    // size sollte kleiner sein als die Anzahl der Unterkugeln * 2000
    // m�lichst wenig Unterkugeln sind am schnellsten. Ist die Gesamtmenge
    // der Punkte kleiner als (2 hoch 3) * 2000, kann die Zahl der Eintr�e
    // entsprechend verkleinert werden.
    // Das sollte, abgesehen von ungnstigen Punktverteilungen, immer die 
    // optimale Berechnungszeit ergeben.
    int partition = 0;
    int entries   = 0;
    if (_partition == -1) {
    partition = 1 + (int)pow(((float)size / (float)_entries), (1.0f / 3.0f));
      if (partition < 2)
        partition = 2;
      entries = size / (int)pow((double)partition, 3);
      if (entries < 3)
        entries = 3;
    }
    else {
      partition = _partition;
      entries = _entries;
    }
    TileSphere* tileSphere = new TileSphere(this, partition, entries, _error);
    float* pointer = pointSet;
    float minX, maxX, minY, maxY, minZ, maxZ;
    minX = maxX = minY = maxY = minZ = maxZ = 0;
    
    // computing Bounding Box
    getBB(pointSet, size, minX, maxX, minY, maxY, minZ, maxZ);
    // and assign it to the main sphere
    tileSphere->setBB(minX, maxX, minY, maxY, minZ, maxZ);
    
    // insert all points
    long tmpSphereCounter = _sphereIndex;
    for (long counter = 0; counter < size; counter++) {
      tileSphere->addPoint(pointer);
      pointer += 3;
    }

    if (_verbose) {
      int treeDepth = 0;
      int usedSpheres = 0;
      int leafSpheres = 0;
      int maxEnt = 0;
      tileSphere->getStatistics(treeDepth, usedSpheres, leafSpheres, maxEnt);
      std::cout << "  Points Overall:       " << size << std::endl;
      std::cout << "  Max Entries Per Sph.: " << entries << std::endl;
      std::cout << "  Max Used Entries:     " << maxEnt << std::endl;
      std::cout << "  Aver Used Entries:    " << (float)size / leafSpheres << std::endl;
      std::cout << "  Subsph. Per Sphere:   " << pow((double)partition, 3) << std::endl;
      std::cout << "  Max Tree Depth:       " << treeDepth << std::endl;
      std::cout << "  Number Of Spheres:    " << _sphereIndex - tmpSphereCounter + 1 << std::endl;
      std::cout << "  Used Spheres:         " << usedSpheres << std::endl;
      std::cout << "  Leaf Spheres:         " << leafSpheres << std::endl << std::endl;

    }
    
    return tileSphere;
  }
  return new TileSphere();
}


void UMDMinimalDistance::getBB(float* pointSet, long size, float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) {
  // return the bounding box of a pointset
  if (pointSet != NULL && size > 0) {
    float* pointer = pointSet;
    minX = maxX = *(pointer++);
    minY = maxY = *(pointer++);
    minZ = maxZ = *(pointer++);
    for (long counter = 1; counter < size; counter++) {
      if (minX > *pointer) minX = *pointer;
      if (maxX < *pointer) maxX = *pointer;
      pointer++;
      if (minY > *pointer) minY = *pointer;
      if (maxY < *pointer) maxY = *pointer;
      pointer++;
      if (minZ > *pointer) minZ = *pointer;
      if (maxZ < *pointer) maxZ = *pointer;
      pointer++;
    } // for
  } // if
}


void UMDMinimalDistance::displaySpheres(bool onOff) {
  _displaySpheres = onOff;
}


SoSeparator* UMDMinimalDistance::displaySpheres(TileSphere* tileSphere) {
  return tileSphere->displaySpheres();
}


void UMDMinimalDistance::verbose(bool onOff) {
  _verbose = onOff;
}


void UMDMinimalDistance::setNumEntries(int entries) {
  _entries = entries;
  _partition = -1;
}


void UMDMinimalDistance::setParams(int entries, int partition) {
  _entries = entries;
  _partition = partition;
}


int UMDMinimalDistance::getUniqueIndex() {
  return _sphereIndex++;
}


HashTable* UMDMinimalDistance::getHashTable() {
  return _hashTable;
}



// #########################################
// #####
// #####  TileSphere
// #####
// #########################################



TileSphere::TileSphere() {
  initSphere(NULL, 2, 2000, 0);
}


TileSphere::TileSphere(UMDMinimalDistance* minimalDistance, int maxPartition, int numElements, float error) {
  initSphere(minimalDistance, maxPartition, numElements, error);
}


TileSphere::~TileSphere() {
  if (_tileSpheres) { delete[] _tileSpheres; _tileSpheres = NULL;}
  if (_subset) { delete[] _subset; _subset = NULL;}
}


void TileSphere::initSphere(UMDMinimalDistance* minimalDistance, int maxPartition, int numElements, float error) {
  _numEntries = 0;
  _hasSubSpheres = false;
  _radius = -1;
  _tileSpheres = NULL;
  
  if (minimalDistance != NULL)
    setParameter(minimalDistance, maxPartition, numElements, error);
}


void TileSphere::setParameter(UMDMinimalDistance* minimalDistance, int maxPartition, int numElements, float error) {
  _minimalDistance = minimalDistance;
  _maxEntries = numElements;
  _partition = maxPartition;
  _sphereIndex = minimalDistance->getUniqueIndex();
  _error = error;
  
  _subset = new float*[numElements];
  assert(_subset != 0);
}


void TileSphere::setBB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
  // assigns the tilesphere a bounding box
  _minX = minX; 
  _maxX = maxX;
  _minY = minY;
  _maxY = maxY;
  _minZ = minZ;
  _maxZ = maxZ;
}


void TileSphere::getBB(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) {
  // gets the bounding box of the tilesphere
  minX = _minX; 
  maxX = _maxX;
  minY = _minY;
  maxY = _maxY;
  minZ = _minZ;
  maxZ = _maxZ;
}


void TileSphere::addPoint(float* position) {
  // adds a point into the tilesphere
  float minX, maxX, minY, maxY, minZ, maxZ;
  
  // if the sphere has subspheres, add point into the according subsphere
  if (_hasSubSpheres) {
    for (int counter = 0; counter < _partition * _partition * _partition; counter++) {
      _tileSpheres[counter].getBB(minX, maxX, minY, maxY, minZ, maxZ);
      if (*position       >= (minX) && *position       <= (maxX) &&
        *(position + 1) >= (minY) && *(position + 1) <= (maxY) &&
        *(position + 2) >= (minZ) && *(position + 2) <= (maxZ))
        _tileSpheres[counter].addPoint(position);
    }
  }
  // if there is no further sub-division, integrate point into subset
  else {
    _subset[_numEntries] = position;
    _numEntries++;
    // if capacity is achieved, establish subspheres and transfer the containing points
    if (_numEntries == _maxEntries) {
      // establish further subspheres
      if (_hasSubSpheres == false) {
        _tileSpheres = new TileSphere[_partition * _partition * _partition];
        _hasSubSpheres = true;
      }
      
      int counter = 0;
      float stepX = (_maxX - _minX) / _partition;
      float stepY = (_maxY - _minY) / _partition;
      float stepZ = (_maxZ - _minZ) / _partition;
      // constitute BBs for the subspheres
      for (int xCounter = 0; xCounter < _partition; xCounter++) {
        for (int yCounter = 0; yCounter < _partition; yCounter++) {
          for (int zCounter = 0; zCounter < _partition; zCounter++) {
            _tileSpheres[counter].setParameter(_minimalDistance, _partition, _numEntries, _error);
            _tileSpheres[counter].setBB(
              _minX + stepX * xCounter,
              _minX + stepX * xCounter + stepX,
              _minY + stepY * yCounter,
              _minY + stepY * yCounter + stepY,
              _minZ + stepZ * zCounter,
              _minZ + stepZ * zCounter + stepZ);
            counter++;
          }
        }
      }
      // transfer points
      for (counter = 0; counter < _maxEntries; counter++)
        addPoint(_subset[counter]);
    }
  }
}


float TileSphere::getRadius() {
  return _radius;
}


float* TileSphere::getPosition() {
  return _position;
}


void TileSphere::getSubset(float** &subset, int &size) {
  subset = _subset;
  size = _numEntries;
}


bool TileSphere::hasSubSpheres() {
  return _hasSubSpheres;
}


int32_t TileSphere::getSphereIndex() {
  return _sphereIndex;
}


void TileSphere::getDeFactoSize() {
  // computes the central point of the BB and acquire the farthest 
  // subsphere/point of the subspheres/pointset for the radius
  float minX, maxX, minY, maxY, minZ, maxZ;
  minX = maxX = minY = maxY = minZ = maxZ = 0;
  bool unInit = true;
  if (_hasSubSpheres) {
    // computes the BB of the contained spheres
    float buffer;
    for (int counter = 0; counter < (_partition * _partition * _partition); counter++) {
      _tileSpheres[counter].getDeFactoSize();
      // not every sphere has
      if (_tileSpheres[counter]._radius != -1) {
        if (unInit) {
          minX = _tileSpheres[counter]._position[0] - _tileSpheres[counter]._radius;
          maxX = _tileSpheres[counter]._position[0] + _tileSpheres[counter]._radius;
          minY = _tileSpheres[counter]._position[1] - _tileSpheres[counter]._radius;
          maxY = _tileSpheres[counter]._position[1] + _tileSpheres[counter]._radius;
          minZ = _tileSpheres[counter]._position[2] - _tileSpheres[counter]._radius;
          maxZ = _tileSpheres[counter]._position[2] + _tileSpheres[counter]._radius;
          unInit = false;
        }
        else {
          buffer = _tileSpheres[counter]._position[0] - _tileSpheres[counter]._radius;
          if (minX > buffer) minX = buffer;
          buffer = _tileSpheres[counter]._position[0] + _tileSpheres[counter]._radius;
          if (maxX < buffer) maxX = buffer;
          buffer = _tileSpheres[counter]._position[1] - _tileSpheres[counter]._radius;
          if (minY > buffer) minY = buffer;
          buffer = _tileSpheres[counter]._position[1] + _tileSpheres[counter]._radius;
          if (maxY < buffer) maxY = buffer;
          buffer = _tileSpheres[counter]._position[2] - _tileSpheres[counter]._radius;
          if (minZ > buffer) minZ = buffer;
          buffer = _tileSpheres[counter]._position[2] + _tileSpheres[counter]._radius;
          if (maxZ < buffer) maxZ = buffer;
        }
      }
    }
  }
  else {
    if (_numEntries != 0) {
      unInit = false;
      float* pointer = _subset[0];
      minX = maxX = * pointer;
      minY = maxY = *(pointer + 1);
      minZ = maxZ = *(pointer + 2);
      
      for (int counter = 1; counter < _numEntries; counter++) {
        pointer = _subset[counter];
        if (minX > *pointer) minX = *pointer;
        if (maxX < *pointer) maxX = *pointer;
        pointer++;
        if (minY > *pointer) minY = *pointer;
        if (maxY < *pointer) maxY = *pointer;
        pointer++;
        if (minZ > *pointer) minZ = *pointer;
        if (maxZ < *pointer) maxZ = *pointer;
      }
    }
  }
  if (!unInit) {
    _position[0] = (maxX + minX) / 2;
    _position[1] = (maxY + minY) / 2;
    _position[2] = (maxZ + minZ) / 2;
    if (_hasSubSpheres) {
      bool init = false;
      float tmpDist;
      for (int counter = 0; counter < (_partition * _partition * _partition); counter++) {
        if (_tileSpheres[counter].getRadius() != -1) {
          if (!init) {
            init = true;
            _radius = distance(_position, _tileSpheres[counter].getPosition()) + _tileSpheres[counter].getRadius();
          }
          else {
            tmpDist = distance(_position, _tileSpheres[counter].getPosition()) + _tileSpheres[counter].getRadius();
            if (_radius < tmpDist)
              _radius = tmpDist;
          }
        }
      }
    }
    else { // _numEntries != 0
      float tmpDist;
      _radius = distance(_position, _subset[0]);
      for (int counter = 1; counter < _numEntries; counter++) {
        tmpDist = distance(_position, _subset[counter]);
        if (_radius < tmpDist)
          _radius = tmpDist;
      }
    }
  }
  else
    _radius = -1;
}


// for debbuging only
SoSeparator* TileSphere::displaySpheres() {
  SoSeparator* spheres = new SoSeparator;
  spheres->ref();
  if (_radius != -1) {
    if (_hasSubSpheres) {
      for (int counter = 0; counter < (_partition * _partition * _partition); counter++) {
        spheres->addChild(_tileSpheres[counter].displaySpheres());
      }
    }
    SoTranslation* translate = new SoTranslation;
    spheres->addChild(translate);
    translate->translation.setValue(_position);
    SoSphere* sphere = new SoSphere;
    spheres->addChild(sphere);
    sphere->radius.setValue(_radius);
  }
  spheres->unrefNoDelete();
  return spheres;
}


// for verbose mode only
void TileSphere::getStatistics(int &_treeDepth, int &_usedSpheres, int &_leafSpheres, int &_maxEnt) {
  int treeDepth = 0, treeDepthTmp = 0;
  float** dummy;
  int subsetSize;
  // hat Unterkugeln
  if (_hasSubSpheres) {
    for (int counter = 0; counter < (_partition * _partition * _partition); counter++) {
      _tileSpheres[counter].getSubset(dummy, subsetSize);
      if (subsetSize != 0 || _tileSpheres[counter].hasSubSpheres()) {
        _tileSpheres[counter].getStatistics(treeDepthTmp, _usedSpheres, _leafSpheres, _maxEnt);
        if (treeDepthTmp > treeDepth) treeDepth = treeDepthTmp;
      }
    }
  }
  // hat Punktmenge
  else {
    if (_numEntries > _maxEnt) _maxEnt = _numEntries;
    _leafSpheres++;
  }
  treeDepth++;
  _usedSpheres++;

  _treeDepth = treeDepth;
}


float TileSphere::computeDistance(TileSphere* tileSphere, float*& point1, float*& point2) {
  float distanz = -1;
  // aktuelle Distanz bestimmen
  if ((point1 != NULL) && (point2 != NULL))
    distanz = distance(point1, point2);
  
  if (tileSphere != NULL) {
    
    // diesen Aufruf im Hashtable registrieren
    _minimalDistance->getHashTable()->addPair(this, tileSphere);
    
    // weniger geht nicht
    if (distanz == 0)
      return 0;
    
    // in beiden Mengen rekursiv hinabsteigen bis zur Punktebene
    // dabei jeweils die Kugel mit dem gr�eren Radius teilen
    if ((_radius > tileSphere->getRadius()) && _hasSubSpheres) {
      distanz = tileSphere->computeDistance(this->getPreciserSphere(tileSphere, distanz), point2, point1);
    }
    else {
      if ((_radius < tileSphere->getRadius()) && tileSphere->hasSubSpheres()) {
        distanz = this->computeDistance(tileSphere->getPreciserSphere(this, distanz), point1, point2);
      }
      else {
        // dieser Teil wird abgearbeitet, wenn die eine Sphere zwar
        // kleiner ist, aber die andere bereits keine Subkugeln mehr enth�t
        if (_hasSubSpheres) {
          distanz = tileSphere->computeDistance(this->getPreciserSphere(tileSphere, distanz), point2, point1);
        }
        if (tileSphere->hasSubSpheres()) {
          distanz = this->computeDistance(tileSphere->getPreciserSphere(this, distanz), point1, point2);
        }
      }
    }
    if (!_hasSubSpheres && !tileSphere->hasSubSpheres()) {
      // direkten Abstand zwischen beiden Punktmengen bestimmen,
      float** otherSubset;
      int otherSize = 0;
      tileSphere->getSubset(otherSubset, otherSize);
      float tmpDistance, tmpDistance2;
      float *tmpPoint1, *tmpPoint2;
      
      if (distanz == -1) {
        distanz = distance(_subset[0], otherSubset[0]);
        point1 = _subset[0];
        point2 = otherSubset[0];
      }

      tmpDistance2 = fastDistance(_subset[0], otherSubset[0]);
      tmpPoint1 = _subset[0];
      tmpPoint2 = otherSubset[0];

      // jeden Punkt gegen jeden testen
      for (int otherCounter = 0; otherCounter < otherSize; otherCounter++) {
        for (int counter = 0; counter < _numEntries; counter++) {
          tmpDistance = fastDistance(_subset[counter], otherSubset[otherCounter]);
          if (tmpDistance2 > tmpDistance) {
            tmpDistance2 = tmpDistance;
            tmpPoint1 = _subset[counter];
            tmpPoint2 = otherSubset[otherCounter];
          }
        }
      }
      tmpDistance2 = sqrtf(tmpDistance2);
      if (distanz > tmpDistance2) {
        distanz = tmpDistance2;
        point1 = tmpPoint1;
        point2 = tmpPoint2;
      }
    }
    if (_hasSubSpheres) {
      // Abstand mit Abst�den zu allen Kugeln vergleichen und prfen,
      // ob andere Kugeln diese Entfernung ebenfalls abdecken
      TileSphere* tmp = this->getPreciserSphere(tileSphere, distanz);
      while (tmp != NULL) {
        distanz = tileSphere->computeDistance(tmp, point2, point1);
        tmp = this->getPreciserSphere(tileSphere, distanz);
      }
    }
    if (tileSphere->hasSubSpheres()) {
      // Abstand mit Abst�den zu allen Kugeln vergleichen und prfen,
      // ob andere Kugeln diese Entfernung ebenfalls abdecken
      TileSphere* tmp = tileSphere->getPreciserSphere(this, distanz);
      while (tmp != NULL) {
        distanz = this->computeDistance(tmp, point1, point2);
        tmp = tileSphere->getPreciserSphere(this, distanz);
      }
    }
  }
  return distanz;
}


TileSphere* TileSphere::getPreciserSphere(TileSphere* referenceSphere, float closestDistance) {
  bool unInit = true;
  bool firstRun = false;
  if (closestDistance == -1)
    firstRun = true;
  
  TileSphere *closestTileSphere = NULL, *tmpSphere = NULL;
  float tmpDistance;
  
  // s�tliche Unterkugeln abklappern
  for (int counter = 0; counter < (_partition * _partition * _partition); counter++) {
    
    // nur betrachten, wenn die Kugel weitere Details beinhaltet
    if ((_tileSpheres[counter].getRadius() != -1) && 
      !_minimalDistance->getHashTable()->existPair(&_tileSpheres[counter], referenceSphere)) {
      if (unInit) {
        // wenn die Variablen noch nicht initialisiert sind, einmalig initialisieren
        tmpDistance = distance(&_tileSpheres[counter], referenceSphere);
        if (firstRun) {
          unInit = false;
          closestTileSphere = &_tileSpheres[counter];
          closestDistance = tmpDistance;
        }
        else 
          if ((tmpDistance * (1 + _error)) < closestDistance) {
          unInit = false;
          closestTileSphere = &_tileSpheres[counter];
          closestDistance = tmpDistance;
        }
      }
      else {
        tmpSphere = &_tileSpheres[counter];
        tmpDistance = distance(tmpSphere, referenceSphere);
        if (firstRun) {
          if (tmpDistance < closestDistance) {
            closestDistance = tmpDistance;
            closestTileSphere = tmpSphere;
          }
        } 
        else {
          if ((tmpDistance * (1 + _error)) < closestDistance) {
            closestDistance = tmpDistance;
            closestTileSphere = tmpSphere;
          }
        }
      }
    }
  }
  return closestTileSphere;
}




float TileSphere::distance(TileSphere* tileSphere1, TileSphere* tileSphere2) {
  return 
    distance(tileSphere1->getPosition(), tileSphere2->getPosition()) - 
    (tileSphere1->getRadius() + tileSphere2->getRadius());
}




// #########################################
// #####
// #####  HashTable
// #####
// #########################################



HashTable::HashTable() {
  _hashSize = 1000;
  _hashTable = new std::vector<int64_t>[_hashSize];
  _addedPairs = _hits = _misses = 0;
}


HashTable::~HashTable() {
  if (_hashTable) { delete[] _hashTable; _hashTable = NULL; }
}


void HashTable::addPair(TileSphere* tileSphere1, TileSphere* tileSphere2) {
  int64_t index1 = tileSphere1->getSphereIndex();
  int64_t index2 = tileSphere2->getSphereIndex();
  int hashKey;
  int64_t entry;
  
  // immer den kleineren Index zuerst
  if (index1 < index2)
    entry = (index1 << 32) | index2;
  else
    entry = (index2 << 32) | index1;
  
  hashKey = (int)entry % _hashSize;
  _hashTable[hashKey].push_back(entry);

  _addedPairs++;
}


bool HashTable::existPair(TileSphere* tileSphere1, TileSphere* tileSphere2) {
  int64_t index1 = tileSphere1->getSphereIndex();
  int64_t index2 = tileSphere2->getSphereIndex();
  int hashKey;
  int64_t entry;
  
  // immer den kleineren Index zuerst
  if (index1 < index2)
    entry = (index1 << 32) | index2;
  else
    entry = (index2 << 32) | index1;
  
  hashKey = (int)entry % _hashSize;
  
  for(std::vector<int64_t>::iterator i = _hashTable[hashKey].begin(); i < _hashTable[hashKey].end(); i++) {
    if (*i == entry) {
      _hits++;
      return true;
    }
  }
  _misses++;
  return false;
}


void HashTable::getStatistics(int &addedPairs, int &hits, int &misses) {
  addedPairs = _addedPairs;
  hits = _hits;
  misses = _misses;
}
