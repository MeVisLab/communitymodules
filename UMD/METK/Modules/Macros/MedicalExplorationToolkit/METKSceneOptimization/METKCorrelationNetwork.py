# Python module import
from ObjMgr import *
from VisDefinitions import *  #Konstanten werden definiert statt String z.B. damit das gleich beim compilieren erkannt wird wenn ich mich verschreibe
from CorrlNet_Definitions import *
import pickle, pprint

#=== Global class instances ====================================================================================================#

_cls_info = None  #cls = class
_cls_iter = None

#---  Global Variables --------#  

_distanceDict          = {}
_dictOfStructColors    = {}
_correlationDict       = {} 
_neighbourDict         = {}
_correlationDict_Index = [0,0]
 
_loaded                = False 
_openWindow            = False
 
_dictChanges           = False

_visited               = []
_groupIndex            = 0
_oldGroup              = []

  
#=================================================================================================== Global ====================#


#--- global notations -------------------------------------------------------------
INF_CHARACTER      = "Character"
_STRUCTURE         = "Structure"
_CASE              = "Case"
_CORREL_NEIGHBOURS = "Neighbours"
_TYPE_MALIGNANT    = "Malignant"  # bösartig
_TYPE_BENIGN       = "Benign"     # gutartig
_TYPE_DOUBTFUL     = "Doubtful"   # verdächtig

#*******************************************************************************************************************************



def init():
  
   global _cls_info, _cls_iter
   #--- Create an ObjInfo class instance to modify the database and to handle database events ----#
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   
   #--- Create an ObjIterator class instance to iterate through the database ---------------------#
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   #--- definition of search criteria for structures (LAY_FILES und INF_IVPOINTER)----------------#
   _cls_iter.setSearchCriteria(LAY_FILES, INF_IVPOINTER) 
    
   emptyDictionaryErrorHandling()
   pass
 
 
#*******************************************************************************************************************************#
def handleLoadedEvent(field = 0):
   """handleLoadedEvent() wird automatsich aufgerufen, wenn ein neuer Fall geladen wurde""" 
   
   global _cls_info, _cls_iter
   global _correlationDict, _distanceDict, _neighbourDict
   global _loaded, _dictOfStructColors
    
   # Ausgabe in MeVisLab-Console
   ctx.log("running Correlation-Network-Computation with maxNeighbourDist of " + str(ctx.field("maxNeighbourDist").value)+ " mm")
   
   #=== check for saved files with the _distanceDict and the correlationDicts of this case =====================================#
   fileCorrel            = '/correlations.pkl' 
   fileDist              = '/distances.pkl'
   distFile = correlFile = False
   caseDir        = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR).rpartition('/')[0].rpartition('/')
   dist_corrl_dir = caseDir[0] + '/Distances and Correls/' + caseDir[2]
   ctx.field("fileNameOfCorrelations").value = dist_corrl_dir + fileCorrel
   ctx.field("fileNameOfDistances").value    = dist_corrl_dir + fileDist
   
   if MLABFileManager.exists(dist_corrl_dir):
     
     #--- if files exists load them --------------------------------------------------------------------------------------------#
     if MLABFileManager.exists(dist_corrl_dir + fileDist):
       loadDistFromFile()
       distFile = True 
     if MLABFileManager.exists(dist_corrl_dir + fileCorrel):  
       loadCorrelsFromFile()
       correlFile = True   
  
  #--- if file-dir don't exists creat it ---------------------------------------------------------------------------------------#
   else: MLABFileManager.mkdir(dist_corrl_dir)    

#   distFile   =  MLABFileDialog.getOpenFileName(caseDir, "pickle files (*.pkl)" , "Open distance file")
#   correlFile =  MLABFileDialog.getOpenFileName(caseDir, "pickle files (*.pkl)" , "Open Correlations file")
   
   #=== Build empty Stringcontainer for all structures of the current case =====================================================#
   structureString = ""
   
   #=== if bothe files exist fill the Structure-Stringcontainer with structureIds contained in the loaded files ================#
   if distFile and correlFile:
     for structure in _dictOfStructColors.iterkeys():
       structureString += ", "+ structure
       checkAndSetCharacterinfoToCase(structure)
   else:
     #=== search for all struktures in database ================================================================================#      
     #--- search first entry with the specified search criteria (in init) -------------------------#     
     _cls_iter.searchFirst()
   
     #--- as long as entrys be found --------------------------------------------------------------#
     while _cls_iter.success():
       
       structure = _cls_iter.object()
  
       #=== collect all structurs in a string to deliver them to the modules field =============================================#
       structureString += ", "+ structure 
       
       #=== if no correlFile was loaded build _correlDict _neighbourDict and colorDict =========================================#
       if not correlFile:
         #=== remember original color of structure =============================================================================#
         _dictOfStructColors[structure] = _cls_info.get(structure, LAY_APPEARANCE, INF_COLOR)
         
         #--- check for strukture's character information resp. set this information (Character = Benign , Doubtful, Malignant)-----#
         checkAndSetCharacterinfoToCase(structure) 
         
         #=== traverse througout Layer Description and compute a correlation dictionary with its included Infos ================#
         #--- activate the Description-Layer of the current Object and search first entry --------------------------------------#
         _cls_info.activateObject(structure)
         _cls_info.activateLayer( LAY_DESCRIPTION) 
         _cls_info.firstInfo(structure) 
         
         while _cls_info.success():
           correlation = _cls_info.activeInfo()
           # -- takeover each INFO of the structure except Name ----------------------------------------------------------------# 
           # Dict[correlationname(e.g. Side):[correlation (e.g. Left):[coressp. structures (e.g. "structure1, structure3, structure7")]]]
           if correlation != INF_NAME: 
             if _correlationDict.has_key(correlation):
               if  _correlationDict[correlation].has_key(_cls_info.get(structure, LAY_DESCRIPTION, correlation)):
                 _correlationDict[correlation][_cls_info.get(structure, LAY_DESCRIPTION, correlation)].append(structure)  #+= ", "+ structure
               else:
                 _correlationDict[correlation][_cls_info.get(structure, LAY_DESCRIPTION, correlation)]= [structure]              
             else:           
               _correlationDict[correlation] = {_cls_info.get(structure, LAY_DESCRIPTION, correlation): [structure]}            
           
           #--- search next info-entry in the Description-Layer ----------------------------------------------------------------#
           _cls_info.nextInfo()   
           pass  # //while _cls_info.success()   
       
       #=== make out a Distance-Dictionary between all structures and compile a Neighbourhood-Dictionary via maxNeighbourDist ==#     
       if distFile:
         if not correlFile:
           buildNeighbourCorrelation()
       else:            
         _neighbourDict[structure]= {_TYPE_MALIGNANT: [], _TYPE_DOUBTFUL: [], _TYPE_BENIGN : []}   
             
         ctx.field("UMDPointerToInventor.objectID").value = structure
         MLAB.processEvents()
         
         for structure2 in _neighbourDict.iterkeys():
           
           if structure2 != structure: # no neighbour to itself
              
             ctx.field("UMDPointerToInventor1.objectID").value = structure2
             MLAB.processEvents()  # Warten, bis alle Berechnungen durchgeführt wurden, sonst ueberholt man sich selbst
             
             #--- activate minimal distance calculate between structure1 and structure2-------------------------#
             ctx.field("UMDSoMinimalDistance.computeMinimalDistance").touch()            
             MLAB.processEvents() 
             
             #--- Distance-Dictionary --------------------------------------------------------------------------#
             _distanceDict[(structure, structure2)] = ctx.field("UMDSoMinimalDistance.distance").value
    
             #--- Neighbour-Dictionary including all neighbours of a structure within a certain distance -------#
             if ctx.field("UMDSoMinimalDistance.distance").value <= ctx.field("maxNeighbourDist").value:
               _neighbourDict[structure][_cls_info.get( structure2,  LAY_DESCRIPTION, INF_CHARACTER)].append(structure2)
               _neighbourDict[structure2][_cls_info.get( structure,  LAY_DESCRIPTION, INF_CHARACTER)].append(structure)                          
             pass # // for
            
       #--- search next structure entry in database -----------------------------------------------# 
       _cls_iter.searchNext()
       pass # //while _cls_iter.success()
     sort_nearestNeighbours() 
     
   #=== save Dictionarys if not allready existens ==============================================================================#
   if not distFile: saveDistToFile()
   if not correlFile: saveCorrelsToFile()

   _loaded = True 
   if _openWindow: openWindow()
   
   #=== assign the list of all structures and all correlations of the loaded case to the modules corresponding fields ==========#    
   ctx.field("success").value = True 
   ctx.field("allStructures").value = structureString 
   loadAllCorrelations()
   ctx.log("finish Correlation-Calculation ;O)") 
   pass # //handleLoadedEvent()


def checkAndSetCharacterinfoToCase(structure):
  """check for strukture's character information resp. set this information (Character = Benign , Doubtful, Malignant)"""
  global _cls_info
  
  if  not _cls_info.existInfo( structure,  LAY_DESCRIPTION, INF_CHARACTER):
    if _cls_info.existInfo( structure,  LAY_DESCRIPTION, INF_STRUCTURE) and _cls_info.get( structure,  LAY_DESCRIPTION, INF_STRUCTURE) == 'Tumor': # bösartig
        _cls_info.typedSet( structure,  LAY_DESCRIPTION,  INF_CHARACTER, _TYPE_MALIGNANT, INFOTYPE_STRING)
    elif _cls_info.existInfo( structure,  LAY_DESCRIPTION, INF_IMPORTANCE ) and float(_cls_info.get( structure,  LAY_DESCRIPTION, INF_IMPORTANCE)) >= 1:  # verdächtig
      _cls_info.typedSet( structure,  LAY_DESCRIPTION, INF_CHARACTER, _TYPE_DOUBTFUL, INFOTYPE_STRING) 
    else:
      _cls_info.typedSet( structure,  LAY_DESCRIPTION, INF_CHARACTER, _TYPE_BENIGN, INFOTYPE_STRING) # gutartig
  pass


def saveDistToFile():
  ''' save Distances  in  Files'''
  global _distanceDict

#  ctx.log( 'saveDist')
  filesDir = ctx.field("fileNameOfDistances").value
  MLABFileManager.remove(filesDir)   
  openFile = open(filesDir, 'wb')
  # Pickle dictionary using protocol 0.  w writable b binär
  pickle.dump(_distanceDict, openFile) 
  openFile.close()
  pass
 
def loadDistFromFile():
  global _distanceDict
  filesDir = ctx.field("fileNameOfDistances").value
  pkl_file = open( filesDir, 'rb')
  _distanceDict = pickle.load(pkl_file)   
  pkl_file.close()
  pass 
 
def saveCorrelsToFile():
  global _correlationDict, _neighbourDict, _dictOfStructColors
  filesDir = ctx.field("fileNameOfCorrelations").value
  MLABFileManager.remove(filesDir) 
  openFile = open(filesDir, 'wb')
  pickle.dump(_correlationDict, openFile, 0)    
  pickle.dump(_neighbourDict,   openFile, 1)    
  pickle.dump(_dictOfStructColors, openFile, 2)    
  openFile.close()   
  pass

def loadCorrelsFromFile():
  global _correlationDict,  _neighbourDict, _dictOfStructColors
  filesDir = ctx.field("fileNameOfCorrelations").value
  pkl_file = open(filesDir, 'rb')
  _correlationDict    = pickle.load(pkl_file)  
  _neighbourDict      = pickle.load(pkl_file)   
  _dictOfStructColors = pickle.load(pkl_file)   
  pkl_file.close()
  pass 

def openWindow():      
  global _openWindow

  _selectedItem = None   
  _openWindow   = True 
  updateListView()
  pass 
 
def closeWindow():
  global _openWindow
  _openWindow   = False 
  pass 
  
def updateListView():
 
  global _correlationDict, _neighbourDict, _loaded, _openWindow
  
#  emptyDictionaryErrorHandling()

  if _loaded and _openWindow:
    correlList   = ctx.control("objects")
    correlList.setRootIsDecorated(True)
    root = correlList.insertItem()
    root.setText(0, "All Correlations")
    root.setOpen(True)
    
    for correlation in _correlationDict.iterkeys():
      correl = correlList.insertItem(root, 0)
      correl.setOn(True)
      correl.setText(0, correlation)
#      correl.setText(1, "GROUP")
     
      for correlType in _correlationDict[correlation].iterkeys():              
        item = correlList.insertItem(correl, 0)
        item.setText(0, correlType)
        
        for structure in _correlationDict[correlation][correlType]:
          child = correlList.insertItem(item, 0) 
          child.setText(0, structure) 
          
    correl = correlList.insertItem(root, 0)
    correl.setOn(True)
    correl.setText(0, _CORREL_NEIGHBOURS)
    
    for main_structure in _neighbourDict.iterkeys():
      item = correlList.insertItem(correl, 0)
      item.setText(0, main_structure)
      
      for type in _neighbourDict[main_structure].iterkeys():
        typeItem = correlList.insertItem(item, 0)
        typeItem.setText(0, type)
        
        for neighbours in _neighbourDict[main_structure][type]:
          child = correlList.insertItem( typeItem, 0) 
          child.setText(0, neighbours)        
  else:
    pass #mach das Ding leer
  pass
 
 
def buildNeighbourCorrelation():
  """ find neighbours of each structure within a certain distance and store them in a dictionary """ 
  
  global _distanceDict, _neighbourDict 
  
  # >>> Ausgabe in MeVisLab-Console >>>
  ctx.log("computing Neighbour-Correlation with maxNeighbourDist " + str(ctx.field("maxNeighbourDist").value))  
  
  emptyDictionaryErrorHandling() 
  #--- delete all neigbour-values in the Neighbourhood-Dict ---#
  for structure in _neighbourDict.iterkeys():     
    _neighbourDict[structure]= {_TYPE_MALIGNANT: [], _TYPE_DOUBTFUL: [], _TYPE_BENIGN : []}  
#  pprint.pprint(_neighbourDict) 
  #=== check in the distance-dictionary for structurepairs with a minDistance beneath maxNeighbourDist ========================#            
  for structPair, distance in _distanceDict.iteritems():        
    if distance <= ctx.field("maxNeighbourDist").value: 
      #=== ... and put them into the Neighbourhood-Dict ========================================================================#
      _neighbourDict[structPair[1]][_cls_info.get( structPair[0],  LAY_DESCRIPTION, INF_CHARACTER)].append(structPair[0])
      _neighbourDict[structPair[0]][_cls_info.get( structPair[1],  LAY_DESCRIPTION, INF_CHARACTER)].append(structPair[1]) 
      
  sort_nearestNeighbours() 
  _dictChanges = True 
  handleSaveEvent() # noch nicht so ganz ausgereift
#  deleteSettings()#toDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDOtoDO#         
  pass # //buildNeighbourCorrelation()


def update_NeigbourCorrelation(): 
   """update the neighbour correlleration with the entered maximal neighbour distance """ #**************************************   
   global _dictChanges   
   buildNeighbourCorrelation()
   updateListView()
   _dictChanges = True 
   pass  


def sort_nearestNeighbours():  
  global _neighbourDict, _distanceDict
   
  for structure in _neighbourDict.iterkeys():       
    for type in _neighbourDict[structure].iterkeys():
      sortedDistances = []       
      for neighbour in _neighbourDict[structure][type]:
        
        #=== check in the distance-dictionary for structurepairs with the given correlationtype ================================#     
        for structPair, distance in _distanceDict.iteritems(): 
          if   structPair[0] == neighbour and structPair[1] == structure \
            or structPair[0] == structure and structPair[1] == neighbour:   
            i = 0    
            while i < len(sortedDistances) and distance > sortedDistances[i][1]: i += 1        
            else: sortedDistances.insert(i, [neighbour, distance]) 
            break 
    
      #sortedList = [structure]
      sortedList = []
      for sorted in sortedDistances:
        sortedList.append(sorted[0]) 
      _neighbourDict[structure][type] = sortedList
  pass


def sort_nearestRelatedStruct(structure = None):
  
  global _correlationDict, _distanceDict
  
  if not structure:
    structure = ctx.field("structure").value
  correlation     = ctx.field("correlation").value
  correlType      = ctx.field("correlationType").value  
  sortedDistances = []
   
  if _correlationDict.has_key(correlation) and _correlationDict[correlation].has_key(correlType):
    for correl in _correlationDict[correlation][correlType]:
      if correl != structure:
        for structPair, distance in _distanceDict.iteritems(): 
          if   structPair[0] == correl    and structPair[1] == structure \
            or structPair[0] == structure and structPair[1] == correl:
            
            distance = _distanceDict[structPair]
            i = 0    
            while i < len(sortedDistances) and distance > sortedDistances[i][1]:
              i += 1
            else: sortedDistances.insert(i, [correl, distance]) 
            break   
    sortedList = [structure]
    
    for sorted in sortedDistances:
      sortedList.append(sorted[0])      
    _correlationDict[correlation][correlType] = sortedList
    
#  else: emptyDictionaryErrorHandling()
  pass


def existsStructure():
  global _neighbourDict
  testStruct = ctx.field("existsStructure").value
  if testStruct in _neighbourDict.keys(): 
    ctx.field("structureExists").value = True
  else: ctx.field("structureExists").value = False  
    
def existsCorrelation( arg = "None" , testCorrel = "None"):  

  if testCorrel == "None":
    testCorrel = ctx.field("existsCorrel").value
  existingCorrels = ctx.field("allCorrelations").value
  if testCorrel in existingCorrels: 
    ctx.field("correlationExists").value = True
    return True
  else: 
    ctx.field("correlationExists").value = False
    return False
     
def existsCorrelType(arg = "None" , testCorrelType = "None"):
  if testCorrelType == "None":
    testCorrelType    = ctx.field("existsCorrelType").value
  existingCorrelTypes = ctx.field("allCorrelationType").value
  if testCorrelType in existingCorrelTypes:
    ctx.field("correlTypeExists").value = True
    return True 
  else: 
    ctx.field("correlTypeExists").value = False 
    return False 
  

def loadAllCorrelations():
    
  global _correlationDict, _oldGroup   
  
  if ctx.field("correl_selection").value == _CASE: # and _loaded: 
#    ctx.log( "loadAllCorrelations()")
      
    if len(_correlationDict) != 0:  #== 0:    
      allCorrelations = ", " + _CORREL_NEIGHBOURS               
      for correlation in _correlationDict.iterkeys():
        allCorrelations += ", " + correlation        
      ctx.field("allCorrelations").value    = allCorrelations
      ctx.field("success").value            = True 
    if ctx.field("correlation").value != "":
      ctx.field("correlation").value = "" 
#    else: 
#      ctx.field("allCorrelationType").value = ""
#      ctx.field("correlationType").value    = ""
#      ctx.field("relatedStructures").value  = ""
  pass

def loadStructuresCorrelations():
  
  global _cls_info, _correlationDict, _neighbourDict
  
  emptyDictionaryErrorHandling()
  
  if ctx.field("correl_selection").value == _STRUCTURE: # and _loaded:  
#    ctx.log( "loadStructuresCorrelations()")
      
#    correlExists      = False
    structure         = ctx.field("structure").value
    correlation_fld   = ctx.field("correlation").value
    structuresCorrels = ""
    ctx.field("success").value = False 
    
    if structure != "": 
      
#      if correlation_fld == "" or correlation_fld == _CORREL_NEIGHBOURS: correlExists = True       
      #=== traverse througout Layer Description and find Correlation of given structure ========================================#
      #--- activate the Description-Layer of the current structure -------------------------------------------------------------#
      _cls_info.activateObject(structure)
      _cls_info.activateLayer(LAY_DESCRIPTION) 
      _cls_info.firstInfo(structure) 
      
      while _cls_info.success():
        correlation = _cls_info.activeInfo()       
        # -- takeover each INFO of the structure except Name -------------------------------------------------------------------# 
        if correlation != "Name": 
          structuresCorrels += ", " + correlation
#          if correlation == correlation_fld: correlExists = True           
        _cls_info.nextInfo()   
        pass  # //while _cls_info.success()
     
      structuresCorrels += ", " + _CORREL_NEIGHBOURS
    
    ctx.field("allCorrelations").value = structuresCorrels 
    if ctx.field("correlation").value != "": ctx.field("correlation").value = ""
#      correlExists                       = True            
#    ctx.field("allCorrelations").value = structuresCorrels    
#    if correlExists == True: 
#      ctx.field("success").value = True 
#      loadCorrelationTypes()
#    else: 
#      ctx.field("correlation").value = ""     
  pass 


def loadCorrelationTypes():
  
  global _correlationDict, _neighbourDict  
    
  correlation     = ctx.field("correlation").value
  fld_correlType  = ctx.field("correlationType")
  fld_allTypes    = ctx.field("allCorrelationType")
#  correction      = False
  
#  emptyDictionaryErrorHandling()
#  ctx.log( "loadCorrelationType()")
  if correlation != "": 
    
#    if not ctx.field("success").value and existsCorrelation("", correlation): 
#      ctx.field("success").value = True
#      correction = True     
#    if ctx.field("success").value:
#      if ctx.field("correl_selection").value == _CASE: ctx.field("success").value = False    
      allCorrelTypes = ""
    
      if correlation == _CORREL_NEIGHBOURS:
        if ctx.field("correl_selection").value == _STRUCTURE:
          fld_allTypes.value = allCorrelTypes = ", " + _TYPE_BENIGN + ", " + _TYPE_DOUBTFUL + ", " + _TYPE_MALIGNANT
#          ctx.field("success").value = True 
#          if fld_correlType.value not in fld_allTypes.value:
          fld_correlType.value = ""
#            if not correction: ctx.field("success").value = False 
          return
        else: #_Case
          for main_struct in _neighbourDict.iterkeys():
            allCorrelTypes += ", " + main_struct
#          ctx.field("success").value = True   
          fld_allTypes.value = allCorrelTypes 
          if fld_correlType.value != "": fld_correlType.value = ""
#          if fld_correlType.value not in fld_allTypes.value:
#            fld_correlType.value = _neighbourDict.keys()[0]
#            if not correction: ctx.field("success").value = False
          return
      
      elif _correlationDict.has_key(correlation):         
#        ctx.field("success").value = True
        if ctx.field("correl_selection").value == _STRUCTURE:
          fld_correlType.value = fld_allTypes.value = _cls_info.get(ctx.field("structure").value, LAY_DESCRIPTION, correlation)
#          correlType = fld_allTypes.value = _cls_info.get(ctx.field("structure").value, LAY_DESCRIPTION, correlation)
#          if correlType != fld_correlType.value:
#            fld_correlType.value =  correlType
#            if not correction: ctx.field("success").value = False
          return         
        else: #_Case
#          allCorrelTypes = ""
          for correlType in _correlationDict[correlation].iterkeys(): 
            allCorrelTypes += ", " + correlType  
          fld_allTypes.value   = allCorrelTypes 
          if fld_correlType.value != "": fld_correlType.value = "" 
#          if fld_correlType.value not in fld_allTypes.value:
#            fld_correlType.value      = "" 
#            ctx.field("success").value = False 
          return 
      else: 
        fld_correlType.value     = fld_allTypes.value = ""
#        ctx.field("success").value = False
        return 
  else:
    if fld_correlType.value != "": fld_correlType.value = ""
    fld_allTypes.value = "" 
  pass


def loadRelatedStructures():

  global _correlationDict, _neighbourDict
  
  structure   = ctx.field("structure").value
  correlation = ctx.field("correlation").value
  correlType  = ctx.field("correlationType").value 
  correction  = False 
#  emptyDictionaryErrorHandling()
#  ctx.log( "loadRelatedStructures()")
  if correlation != "" and correlType != "":    
#    if not ctx.field("success").value and existsCorrelType("", correlation) : 
#      ctx.field("success").value = True
#      correction = True  
  
#    if ctx.field("success").value == True:  
#      ctx.field("success").value = False 
        
      if correlation == _CORREL_NEIGHBOURS: 
        if ctx.field("correl_selection").value == _STRUCTURE: # and structure != "":
          neighbourList = _neighbourDict[structure][correlType]
          if len(neighbourList) == 0:
            ctx.field("relatedStructures").value = ""
          else:
            ctx.field("relatedStructures").value = repr(_neighbourDict[structure][correlType])
          allRelatedStructures_HC()
#         ctx.field("success").value           = True 
          return           
        else:
          try: 
            ctx.field("relatedStructures").value = [correlType] + _neighbourDict[correlType][_TYPE_BENIGN] + _neighbourDict[correlType][_TYPE_DOUBTFUL] + _neighbourDict[correlType][_TYPE_MALIGNANT]
            allRelatedStructures_HC()
#            ctx.field("success").value           = True
            return
          except KeyError(correlType) : "correlationType does not exist in this case (given structure and correlation)"
      else:      
        try: 
          if ctx.field("correl_selection").value == _CASE: sort_nearestRelatedStruct(_correlationDict[correlation][correlType][0])
          elif structure != "": sort_nearestRelatedStruct()       
          ctx.field("relatedStructures").value = repr(_correlationDict[correlation][correlType])
          allRelatedStructures_HC()
#          ctx.field("success").value = True
          return     
        except KeyError(correlType) : "correlation" + correlation + " or Type " + correlType + " does not exist in this case (given structure and correlation)"                   
#    else:
#      ctx.field("relatedStructures").value = ""
#      allRelatedStructures_HC()
  else: 
    ctx.field("relatedStructures").value = ""
    allRelatedStructures_HC()
  pass


def getFirstCorrelation():
  
  global _correlationDict_Index 
  
  ctx.log( "getFirstCorrelation")
  
  _correlationDict_Index[0]  = 1
  ctx.field("success").value = True
  first = ctx.field("allCorrelations").value.split(', ') 
  
  if len(first) > 1:
    ctx.field("correlation").value = first[1]
    return
  else:
    ctx.field("correlation").value = ""
    ctx.field("success").value     = False 
  pass


def getNextCorrelation():
  
  ctx.log( "getNextCorrelation")
  
  global _correlationDict_Index  
  next = ctx.field("allCorrelations").value.split(', ') 
  _correlationDict_Index[0] += 1 
  if len(next) > 1 and _correlationDict_Index[0] < len(next):
    ctx.field("correlation").value = next[_correlationDict_Index[0]] 
    ctx.field("success").value     = True 
    return
  else:
    ctx.field("correlation").value = ""   
    ctx.field("success").value     = False 
  pass 


def getFirstCorrelationType():
  
  global _correlationDict_Index 
  
  first = ctx.field("allCorrelationType").value.split(', ') 
  size  = len(first)
  if first[0] == _TYPE_BENIGN:  _correlationDict_Index[1] = 0
  else: _correlationDict_Index[1] = 1   
    
  if size == 0:
    ctx.field("correlationType").value = ""
    ctx.field("success").value         = False 
    return
  elif size == 1 or first[0] == _TYPE_BENIGN:
    ctx.field("correlationType").value = first[0]
    ctx.field("success").value         = True 
    return
  else:
    ctx.field("correlationType").value = first[1]
    ctx.field("success").value         = True 
  pass


def getNextCorrelationType():
  
  global _correlationDict_Index  
  next                       = ctx.field("allCorrelationType").value.split(', ') 
  _correlationDict_Index[1] += 1
  
  if len(next) > 1 and _correlationDict_Index[1] < len(next):
    ctx.field("correlationType").value = next[_correlationDict_Index[1]] 
    ctx.field("success").value         = True 
    return
  else:
    ctx.field("correlationType").value = ""   
    ctx.field("success").value         = False 
  pass 

#=== all related Structures and the current selected member for Highlighting and traversing ====================================#
def allRelatedStructures_HC():
  
  global _visited, _groupIndex, _oldGroup
  
#  ctx.log( "allRelatedStructures_HC")
#  ctx.log('direction: ' + str(direction))
  correlation   = ctx.field("correlation").value
  correlType    = ctx.field("correlationType").value   
  currentMember = ""  
  _visited      = group = []
  
  if ctx.field("relatedStructures").value != "":          
    group         = eval(ctx.field("relatedStructures").value)
    currentMember = group[0] 
  ctx.field("csRelatedStructure").value = currentMember
  _visited    = [currentMember]
  _groupIndex = 0 
  
  if ctx.field("enableColering").value:
    deHighlighting_relatedStructures()
    if currentMember != "":
      highlight_relatedStructures(group)      
      highlight_relatedStructures(_visited, "csm")# current Member    
      _oldGroup =[ctx.field("structure").value, correlation, correlType] 
    else: _oldGroup = [] 
  pass

def nextRelatedStructure():
  global _visited, _groupIndex, _oldGroup, _neighbourDict, _correlationDict
  ctx.log( "nextRelatedStructure" + str(_groupIndex))
  if ctx.field("relatedStructures").value != "":
    _groupIndex = (_groupIndex+1) % len(eval(ctx.field("relatedStructures").value))
    traverseRelatedStructure()      
  pass

def prevRelatedStructure():
  global _visited, _groupIndex, _oldGroup, _neighbourDict, _correlationDict
  ctx.log( "prevRelatedStructure" + str(_groupIndex))
  if ctx.field("relatedStructures").value != "":
    _groupIndex = (_groupIndex-1) % len(eval(ctx.field("relatedStructures").value))
    traverseRelatedStructure() 
  pass 


def traverseRelatedStructure():
  global _visited, _groupIndex, _oldGroup, _neighbourDict, _correlationDict

#  ctx.log('direction: ' + str(direction))
  demandStruct  = ctx.field("structure").value
  correlation   = ctx.field("correlation").value
  correlType    = ctx.field("correlationType").value
  currentMember = ctx.field("csRelatedStructure").value
 
  if ctx.field("enableColering").value:
    deHighlighting_relatedStructures([currentMember])
    highlight_relatedStructures([currentMember])
  if correlation == "" or correlType == "": 
#    ctx.field("success").value = False
    return     
  elif correlation != _CORREL_NEIGHBOURS :
    sort_nearestRelatedStruct(currentMember)
    groupList = _correlationDict[correlation][correlType]    
  else:
    if ctx.field("correl_selection").value == _STRUCTURE:
      groupList = _neighbourDict[demandStruct][correlType] 
    else: 
#!!!!!! #>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>NOCH SORTIERUNG MACHEN >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      groupList = [demandStruct] + _neighbourDict[demandStruct][_TYPE_BENIGN] + _neighbourDict[demandStruct][_TYPE_DOUBTFUL] + _neighbourDict[demandStruct][_TYPE_MALIGNANT] 

# ctx.field("success").value = True     
  if len(_visited) < len(groupList) and not (0 <= (_groupIndex) < len(_visited)):
#    ctx.field("structure").value = ctx.field("csRelatedStructure").value
  #  ctx.field("METKCorrelationNetwork.correlation").value = ctx.field("activeCorrel").value
    i = 0
    currentMember = groupList[i]    
    while currentMember in _visited:
      i += 1
      currentMember = groupList[i]
    if _groupIndex > len(_visited):
      _visited.insert(0, currentMember)
      _groupIndex = 0
    else:  
      _visited.append(currentMember) 
  else:
    currentMember = _visited[_groupIndex]   
  ctx.field("csRelatedStructure").value = currentMember
  if ctx.field("enableColering").value:
    highlight_relatedStructures([currentMember], "csm") 
  pass

def switchHighlightmode():
  global _oldGroup, _visited, _groupIndex
  
  if ctx.field("enableColering").value: 
    if ctx.field("relatedStructures").value != "":
      _oldGroup = [ctx.field("structure").value, ctx.field("correlation").value, ctx.field("correlationType").value]
      highlight_relatedStructures(_oldGroup)
#      ctx.log(str(_visited) + ", " + str(_groupIndex))
      if len(_visited) > 0:
        highlight_relatedStructures([_visited[_groupIndex]], "csm")
  else:
    deHighlighting_relatedStructures()
  pass

#*** highlighting whole group by coloring **************************************************************************************#
def highlight_relatedStructures(group, colorType = "group"):
  
  global _cls_info
  if len(group) != 0:
    if colorType == "group":
      color = str(ctx.field("color_group").value)[1:-1].replace(', ',' ')
    else:
      color = str(ctx.field("color_CSM").value)[1:-1].replace(', ',' ')
      
    for member in group:
      _cls_info.typedSet(member, LAY_APPEARANCE, INF_COLOR, color, INFOTYPE_VEC3)    # highlighting with group marking color
    _cls_info.notify()
  pass 

#*** convert back group's appearance (into original appearance) ****************************************************************#
def deHighlighting_relatedStructures(group = None):
  
  global _cls_info, _dictOfStructColors, _oldGroup
  
  if ctx.field("enableColering").value:
    ctx.log('deHighlighting_relatedStructures(group = None)')
    if group == None:
      if len(_oldGroup) != 0:
        if _oldGroup[1] != "" and _oldGroup[2] != "":
          if _oldGroup[1] == _CORREL_NEIGHBOURS:
            if ctx.field("correl_selection").value == _STRUCTURE:
              group = [_oldGroup[0]] + _neighbourDict[_oldGroup[0]][_oldGroup[2]]          
            else:
              group = [_oldGroup[2]] + _neighbourDict[_oldGroup[0]][_TYPE_BENIGN] + _neighbourDict[_oldGroup[0]][_TYPE_DOUBTFUL]+_neighbourDict[_oldGroup[0]][_TYPE_MALIGNANT]
          else: 
            group = _correlationDict[_oldGroup[1]][_oldGroup[2]]
        else: group = [] 
      else: group = []  
  
    if len(group) > 0:
      for member in group: 
        _cls_info.typedSet(member, LAY_APPEARANCE, INF_COLOR, _dictOfStructColors[member], INFOTYPE_VEC3)    
      _cls_info.notify()
  pass

def deHighlighting_ALL():
  
  group = ctx.field("allStructures").value[2:].split(', ')
  if len(group) > 0:
    deHighlighting_relatedStructures(group)
    _oldGroup = []
  else:
    pass
  pass

def emptyDictionaryErrorHandling():
  
  global _correlationDict, _cls_info, _loaded, _groupIndex
  
  if len(_correlationDict) == 0 and _cls_info.get(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG) == "XML file loaded.":
    ctx.log("unintended deletion of the correlations >>> start recomputation of correlations <<< probable cause: Changes in scripts")
    loadDistFromFile() 
    loadCorrelsFromFile()
    _loaded = True 
    
    deHighlighting_ALL()
    if ctx.field("enableColering").value:
      highlight_relatedStructures(ctx.field("relatedStructures").value)
      highlight_relatedStructures(ctx.field("csRelatedStructure").value, "csm")
   
    if _openWindow: updateListView()
    return True 
  else: return False
  pass
  

def deleteSettings():
  """ delete the content of the modules correlation fields und structure field """ #*********************************************
  ctx.field("structure").value          = ""
  ctx.field("relatedStructures").value  = ""
  ctx.field("allCorrelationType").value = ""
  ctx.field("correlationType").value    = ""
  ctx.field("correlation" ).value       = "" 
  ctx.field("structure").value          = ""
  pass
   
 
def handleSaveEvent():
  #--- speicherdialog zum Speichern der Distance und Correlation (Correl Neighbour Color) -  Dicts
  #--- determine the dir for saving Files --------------------------------------------------------------------------------------#
  global _dictChanges
  
  ctx.log( 'handleSaveEvent()')
  if _dictChanges:
    MLAB.processInventorQueue()
    ctx.field("MessageBox.show").touch()
    MLAB.processEvents()
    MLAB.processInventorQueue()
    if ctx.field("MessageBox.returnCode").value == 1:
      saveDistToFile()
      saveCorrelsToFile()
      _dictChanges = False
    
#  filesDir = MLABFileDialog.getSaveFileName( dist_corrl_dir + '/distances.pkl', "Pickle files (*.pkl)","Save Distances") 

#  if filesDir != "":
#    print filesDir
#  saveDistToFile(filesDir)
#  filesDir = MLABFileDialog.getSaveFileName( dist_corrl_dir + '/correlations.pkl', "Pickle files (*.pkl)","Save Correlations") 
#  if filesDir != "":
#    print filesDir
#    saveCorrelsToFile(filesDir)  
  pass
 

def handleCleanupEvent():
 
  global _correlationDict, _loaded, _groupIndex, _visited, _oldGroup, _cls_info
  global _neighbourDict, _distanceDict, _dictOfStructColors, _correlChanges
  
  ctx.log("clean METKCorrelationNetwork") 
  
  loadCorrelsFromFile()
  loadDistFromFile()
  handleSaveEvent()  
  
  deHighlighting_ALL()
  _loaded       = False 
  _groupIndex   = 0 
  _visited      = []
  _oldGroup     = []
  _correlationDict.clear()
  _correlationDict_Index = None 
  _distanceDict.clear()
  _neighbourDict.clear()
  _dictOfStructColors.clear()
   
  ctx.field("fileNameOfDistances").value     = ""
  ctx.field("fileNameOfCorrelations").value  = ""
  ctx.field("allStructures").value           = "" 
  ctx.field("structure").value               = ""
  if ctx.field("correl_selection").value == _CASE:
    ctx.field("allCorrelations").value = ""
    ctx.field("correlation").value     = ""
  ctx.field("existsStructure").value   = ""
  ctx.field("existsCorrel").value      = ""
  ctx.field("existsCorrelType").value  = ""
  ctx.field("structureExists").value   = False
  ctx.field("correlationExists").value = False
  ctx.field("correlTypeExists").value  = False

  ctx.log("METKCorrelationNetwork is clean")  
  pass
   

def handleObjectCreatedEvent():
  # To Do  
  global _dictChanges
  ctx.log( 'handleObjectCreatedEvent()')
  pass


def handleObjectRemovedEvent():
  # To Do  
  global _dictChanges
  ctx.log('handleObjectRemovedEvent()')
  pass


def handleAttributeCreatedEvent():
  # To Do  
  global _dictChanges
  ctx.log('handleAttributeCreatedEvent()')
  pass

def handleAttributeRemovedEvent():
  # To Do  
  global _dictChanges  
  ctx.log( "handleAttributeRemovedEvent()")
  pass


def handleAttributeModifiedEvent():
  # To Do  
  global _dictChanges  
  ctx.log( "handleAttributeModifiedEvent()")
  pass

