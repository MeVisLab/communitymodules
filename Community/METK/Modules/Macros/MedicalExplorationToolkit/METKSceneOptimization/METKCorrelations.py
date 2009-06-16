#*** Python module import ******************************************************************************************************#
from __future__ import with_statement # für beim fileladen erspart file.close()
from ObjMgr import *
from VisDefinitions import *          # Konstanten werden definiert statt String z.B. damit das gleich beim compilieren erkannt wird wenn ich mich verschreibe
from struct import *                  # hier für byte umwandlung - http://docs.python.org/library/struct.html?highlight=coding%20byte
import math, operator                 # --> operator.itemgetter(itemindex)
import pickle, pprint                 # pickle zum abspeichern in files (byte coded python intern) 

#*******************************************************************************************************************************#
#*** Globals *******************************************************************************************************************#
#*******************************************************************************************************************************#
#--- class instances ---------------------------------#
_cls_info = None  #cls = class
_cls_iter = None   

#--- global notations --------------------------------#
_FILENAME_GEOMCORREL = "GeometricCorrelations.dat"    # filename of geometric - correlations

_STRUCTURE           = "D-Structure"                  # flag for computation of meta-info correlation of demand-structure (enumItem)
_CASE                = "Case"                         # flag for computation of meta-info correlation of whole case (enumItem)

INF_CHARACTER        = "Character"
INF_DEGREEofSEVERITY = "DegreeOfSeverity"             # Schweregrad für verdächtige und patholog Structuren (definiert durch Criterias im Panel)
INF_HAZARDSTATE      = "InfiltrationHazard"           # Risikograd, dass die Struktur infiltriert ist, bzw. Stärke der infiltration 

_TYPE_PATHOLOGIC     = "Pathologic"                   # pathologisch
_TYPE_BENIGN         = "Benign"                       # gutartig
_TYPE_DOUBTFUL       = "Doubtful"                     # verdächtig

#--- global variables --------------------------------#
_correlList              = None                       # for listView 
_calcOfStructCorrelType  = False                      # True if current calculation of related Structs (correlType) is structure dependend (else case)
_indice_MetaCorrelation  = 0                          # Indice for traversation of all Meta_correlations of the case
_indice_MCorrelsOfStruct = 0                          # Indice for traversation of Meta_correlations of the demand structure
_indice_MetaCorrelType   = 0                          # Indice for traversation of Types of current Meta_correlation
_epsilon                 = 0.00001                    # adjustment-value for thresholds to avert >= and <= compairisons (instead of < or >)

#--- temporary for testing --see possible functions etc. for this datatype -----------------------------------------------------#
_testDict   = {}
_testList   = []
_testString = ""
#*******************************************************************************************************************************#
#*** Init **********************************************************************************************************************#
#*******************************************************************************************************************************#
def init():
  """ initialze this module - create module-class instances of METKObjInfo and ObjIterator
      and define search criteria for structures within a case. """
  global _cls_info, _cls_iter
  
#  ctx.log("init")
  #--- Create an ObjInfo class instance to modify the database and to handle database events ----#
  _cls_info = METKObjInfo(ctx.module("ObjInfo"))
  _cls_info.setEventReceiveMode(ERM_NONE)
  
  #--- Create an ObjIterator class instance to iterate through the database ---------------------#
  _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
  #--- definition of search criteria for structures (LAY_FILES und INF_IVPOINTER)----------------#
  _cls_iter.setSearchCriteria( LAY_IMAGE, INF_IMAGETYPE, "Segmentation") #(LAY_FILES, INF_IVPOINTER) 
  
  test = open_GeometricCorrelFile()
  test.close()
  pass


#*******************************************************************************************************************************# 
#*** General Functions *********************************************************************************************************# 
#*******************************************************************************************************************************#

def saveToFile(objectToSave):
  ''' save object to File'''  

#  ctx.log( 'saveToFile')
  filesDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)+"/allCorrelations"
  MLABFileManager.remove(filesDir)   
  openFile = open(filesDir, 'wb')
  # Pickle dictionary using protocol 0.  w writable b binär
  pickle.dump(objectToSave, openFile) 
  openFile.close()
  pass
 
def loadFromFile():
  filesDir     = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)+"/allCorrelations"
  pkl_file     = open( filesDir, 'rb')
  loadedObject = pickle.load(pkl_file)   
  pkl_file.close()
  return loadedObject
  pass 


def open_GeometricCorrelFile():
  filesDir = _cls_info.getString(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _FILENAME_GEOMCORREL
  openFile = open( filesDir, 'rb')
  return openFile
  pass


def close_GeometricCorrelFile(openFile):
  openFile.close()
  pass

def factorial(number):
  result = 0
  while number > 0:
    result += number
    number -= 1
  return result
  pass


#=== Sort a list of structures by the TSP (Travelling Salesman Problem) by the insert-methode to get the shortest tour =========#
def sort_structures_TSP_Insert( listOfStructs, searchCriterias = None):
  """ sort a list of structures by the TSP (Travelling Salesman Problem) via insert-methode to get the shortest tour 
      Attribute: the list, the list of geometric Coorelation (minimal Distances) and the total number of Structures in this case """
  global _cls_info
#  ctx.log("sort_structures_TSP_Insert("+ str( listOfStructs) +")")
  sortedList           = []                  # list with the sorted structures

  if listOfStructs:
    
    listOfAllStructs   = eval("[" + ctx.field("allStructures").value[3:] + "']")    # to get index of StructureID
    nextInsert         = []                          # the structure that will be inserted next into the sorted List ['structure', distance to its counterpart at indiceInsertStruct in sortedList] ]
    indiceInsertStruct = 0                           # indice of the choosen structure in the sorted list which nearest neighbour will be inserted next                                  
    openGC_File        = open_GeometricCorrelFile()  # to get the minimal Distances open that file    
        
    sortedNeighbours   = [sorted(get_Structs_with_minDist_fullfillCriterias(openGC_File, listOfAllStructs.index(listOfStructs[0]), searchCriterias = searchCriterias))]
    sortedList         = [listOfStructs[0]]    
    searchCriterias.append([INF_NAME, "!=", str(_cls_info.getString(listOfStructs[0], LAY_DESCRIPTION, INF_NAME))]) 
    listOfStructs      = listOfStructs[1:]
    
    #=== As long as not all structures of the given list are inserted into tho sorted list ... ===================================#
    while listOfStructs:
      nextInsert = [ctx.field("gc_NeighbThreshFar").maxValue, None ]  # default

      #=== get for each already inserted the nearst neighbour which is also in the given structurelist but not already inserted. =# 
      for index, insertedStruct in enumerate(sortedList):
          
        for cStruct_dist in sortedNeighbours[index]:
          if cStruct_dist[0] < nextInsert[0]:     #cStruct_dist[0] in listOfStructs and cStruct_dist[1] not in sortedList and     
            nextInsert         = cStruct_dist
            indiceInsertStruct = index
            break
                
      #=== calculate minimal distance of the structure to be inserted and the neighbours of its Counterpart in sortedList ========# 
      distNeighbourL = get_minimalDistance(openGC_File , listOfAllStructs.index(nextInsert[1]), listOfAllStructs.index(sortedList[(indiceInsertStruct-1)% len(sortedList)]))
      distNeighbourR = get_minimalDistance(openGC_File , listOfAllStructs.index(nextInsert[1]), listOfAllStructs.index(sortedList[(indiceInsertStruct+1)% len(sortedList)]))      

      #=== insert nextStruct between its Counterpart at indiceInsertStruct and its neighbour with the lower distance to nextStruct#  
      if distNeighbourR < distNeighbourL: indiceInsertStruct += 1
      sortedList.insert(indiceInsertStruct, nextInsert[1])
      try: listOfStructs.remove(nextInsert[1]) 
      except ValueError: print nextInsert[1] + '....................................................................' + str(listOfStructs)
      
#      print nextInsert[1]
      
      for neighbourlist in sortedNeighbours: 
        try: del neighbourlist[map(operator.itemgetter(1), neighbourlist).index(nextInsert[1])] 
        except ValueError: print neighbourlist    
      
      dist_struct  = get_Structs_with_minDist_fullfillCriterias(openGC_File, listOfAllStructs.index(nextInsert[1]), searchCriterias = searchCriterias)      
      searchCriterias.append([INF_NAME, "!=", _cls_info.getString(nextInsert[1], LAY_DESCRIPTION, INF_NAME)])
      sortedNeighbours.insert(indiceInsertStruct, sorted(dist_struct))
#      print sortedNeighbours[indiceInsertStruct]

    openGC_File.close()  
  return sortedList  
  pass


 
 
#*******************************************************************************************************************************# 
#*** Handle New-Case-is-loaded-Event *******************************************************************************************# 
#*******************************************************************************************************************************#

#== Calculation of Correlations ================================================================================================# 
def handleLoadedEvent(field = 0):
  """ (called if a case will be loaded) - Calculates the correlations between all structures of the current case (preprocesseing)
      -( 0) pathologic character) -1) correlations within the structures' meta-informations -2) geometric correlations""" 
  
  global _cls_info, _cls_iter
   
  # readout in MeVisLab-Console
  ctx.log("Computatuion of Structures' Correlations")  
  
  MLAB.processEvents() # if segmentation or somthing else has to be done wait till all computations are finished 
  
  #--- preparatory-steps -------------------------------------------------------------------------------------------------------#
  #--- Build empty Listcontainer for all structures of the current case -------------------------------------------#
  listOfStructures          = []
  dictOfMetaCorrelations    = {}
  dict_pathologicCharacter = {_TYPE_PATHOLOGIC: {}, _TYPE_DOUBTFUL: {}, _TYPE_BENIGN: []}
  maxMinimalDistance        = 0    # for fild in panel (slider)
  geomCorrelFileExists      = True # Flag for computing Geometric-Correlations with no existing file

  
  #--- compute maximum extension of all patholog. and doubtfoul Structs for patholog Correls-calculates  ----------#
  ctx.field("METKMeasures.descriptionTag").value = INF_STRUCTURE
  for malignantCriteria in eval(ctx.field("pc_malignantCriteria").value):
    ctx.field("METKMeasures.structure").value = malignantCriteria[0]
    ctx.field("METKMeasures.start").touch()
  MLAB.processEvents()  
  for doubtfulCriteria in eval(ctx.field("pc_doubtfulCriteria").value):
    ctx.field("METKMeasures.structure").value = doubtfulCriteria[0]
    ctx.field("METKMeasures.start").touch()

  #=== look for already existing file with case's geometric Correlations and flag for eventualy further calculations ===========#
  if not MLABFileManager.exists(_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _FILENAME_GEOMCORREL):
    geomCorrelFileExists = False 
    filesDir             = _cls_info.getString(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _FILENAME_GEOMCORREL
    openGeomCorrelFile   = open( filesDir, 'wb') # open file for writing (binary)
    
    #--- write defalut header-infos --------------------------------------------------------------------------------------------#
    openGeomCorrelFile.write( pack('L', 100))   # DefaultValue real value will be writen after calculation 
    openGeomCorrelFile.write( pack('L', 20))    # startIndex of minimal Distances in file           
    openGeomCorrelFile.write( pack('L', 100))   # DefaultValue real value will be writen after calculation  # startIndex of intersections in file
    openGeomCorrelFile.write( pack('L', 0))     # DefaultValue real value will be writen after calculation  # startIndex of occlusions in file or False (0) if no file camera_data.dat 
    openGeomCorrelFile.write( pack('L', 100))   # DefaultValue real value will be writen after calculation  # maximal minmalDistance between two structs

  
  #--- search for all structures in database -----------------------------------------------------------------------------------#      
  #--- search first entry with the specified structure-search-criteria (in init) -----------------------------#     
  _cls_iter.searchFirst()
 
  #--- as long as structure-entrys are found -----------------------------------------------------------------#
  while _cls_iter.success():
    
    structure = str(_cls_iter.object())
    
    #===(0) patholog part: check for strukture's character-information(Benign, Doubtful, Malignant) resp. set them =============#
    dict_pathologicCharacter = get_pathologCharacter(structure, dict_pathologicCharacter) 
        
    #===(1) traverse throughout Layer Description of current structure and store the meta-infos in the correlation dictionary ==#
    #--- activate the Description-Layer of the current Object and search first entry --------------------------#
    _cls_info.activateObject(structure)
    _cls_info.activateLayer( LAY_DESCRIPTION) 
    _cls_info.firstInfo(structure) 
    
    #--- as long as entrys in the Layer Descriptin are found --------------------------------------------------#
    while _cls_info.success():
      correlation = str(_cls_info.activeInfo()) 
      
      #--- capitalize the Infovalue Side cause of the existens of both variants: Lower Case and Upper Case in some cases
      #    else there are 4 types (Left, Right, left, right) instead of 2. Tz Tz Tz ...------------------------# 
      if correlation == INF_SIDE:
        sideinfo = _cls_info.get(structure, LAY_DESCRIPTION, correlation)
        sideinfo = _cls_info.get(structure, LAY_DESCRIPTION, correlation).capitalize()
        _cls_info.set(structure, LAY_DESCRIPTION, correlation, sideinfo)
      
      # -- takeover each INFO of the structure except Name ----------------------------------------------------#
      # Dict[correlationname(e.g. Side):[correlation (e.g. Left):[coressp. structures (e.g. "structure1, structure3, structure7")]]]
      if correlation != INF_NAME: 
        if dictOfMetaCorrelations.has_key(correlation):
          if  dictOfMetaCorrelations[correlation].has_key(_cls_info.get(structure, LAY_DESCRIPTION, correlation)):
            dictOfMetaCorrelations[correlation][str(_cls_info.get(structure, LAY_DESCRIPTION, correlation))].append(structure)  #+= ", "+ structure
          else:
            dictOfMetaCorrelations[correlation][str(_cls_info.get(structure, LAY_DESCRIPTION, correlation))]= [structure]  
        else:           
          dictOfMetaCorrelations[correlation] = {str(_cls_info.get(structure, LAY_DESCRIPTION, correlation)): [structure]}            
      
      #--- search next info-entry in the Description-Layer ----------------------------------------------------#
      _cls_info.nextInfo()   
      pass  # //while _cls_info.success()       

    #===(3) with no existing GeometricCorrelations-File save minimal Distances to such file ====================================#
    if not geomCorrelFileExists: 
      
#      listOfMinDistances.append([])    #raus
    
      #--- pass current structure to the minimal-Distance-Calculation-Process (1st PointToInventor) -------------#        
      ctx.field("UMDPointerToInventor.objectID").value = structure
  
      #--- pass successive the already detected structures in the second PointerToInventor 
      #    and calculate for each the minimal Distance to the current structure ---------------------------------#
  
      for structure2 in listOfStructures:
         
        ctx.field("UMDPointerToInventor1.objectID").value = structure2
        MLAB.processEvents()  # wait till all calculations are done, else you would overtake yourself  
        
        #--- activate minimal distance calculation between structure1 and structure2-----------------------------#
        ctx.field("UMDSoMinimalDistance.computeMinimalDistance").touch()            
        MLAB.processEvents()
        
        minDistance = ctx.field("UMDSoMinimalDistance.distance").value
        
        openGeomCorrelFile.write( pack('f', minDistance))
        #listOfMinDistances[index_struct1].append(minDistance) # gleich in file speichern 
        
        #--- find maximal minimal-distance as intiger of all structurepairings ---------------------#
        if minDistance > maxMinimalDistance:
          maxMinimalDistance = math.ceil(minDistance) 
                

    #=== make out a list of all structures within the loaded case ==============================================================#
    listOfStructures.append(structure) 

    #--- search next structure entry in database -------------------------------------------------------------# 
    _cls_iter.searchNext()
    pass # //while _cls_iter.success()
  
  #=== With no existing GeometricCorrelations-File compute the necessary data-structures and save the results to the file ------#
  if not geomCorrelFileExists:
    
    currentFilePos = openGeomCorrelFile.tell()                       # current index in File = startposition of Occlusions in file
    
    #--- write header: numStructs, startDistance, startOcclusions, startIntersections -----------------------------------------#        
    openGeomCorrelFile.seek(0)
    openGeomCorrelFile.write( pack('L', len(listOfStructures)))  # abs number of structures
    openGeomCorrelFile.write( pack('L', 5*4))                    # Start minimal-distances    
    openGeomCorrelFile.write( pack('L', currentFilePos))               # Start intersections
    
    #--- get the "diagonal half-matrix" of structure's intersections and store them in the geometric Correlations-File ---------#    
    nextGeomCorrelMatrix = build_Intersection_Matrix(listOfStructures)
    openGeomCorrelFile.seek(currentFilePos)
    for intersection_row in nextGeomCorrelMatrix:
      for intersectInfo in intersection_row:
        openGeomCorrelFile.write( pack('f', intersectInfo))
    
    #--- compute the Occlusion-'Matrix' for geometric correlations with file camera_data.dat -----------------------------------#
    nextGeomCorrelMatrix = build_OcclusionMatrix()
    if nextGeomCorrelMatrix:
      currentFilePos = openGeomCorrelFile.tell()             # current index in file = startposition of Intersections in file
      
      for occlusion_row in nextGeomCorrelMatrix :
        for occlusionInfo in occlusion_row:
          openGeomCorrelFile.write( pack('L', occlusionInfo[0]))
          openGeomCorrelFile.write( pack('L', occlusionInfo[1])) 
    else:
      currentFilePos = 0                                     # means no occlusion-infos avaiable
                  
    nextGeomCorrelMatrix = []                                # clean up container
    openGeomCorrelFile.seek(3*4)                             
    openGeomCorrelFile.write( pack('L', currentFilePos))     # write startposition of occlusion to file at position 4
    openGeomCorrelFile.write( pack('L', maxMinimalDistance)) # write maximal minimal distance between two structs to file (pos. 5)
    openGeomCorrelFile.close()     
  
  else:
    #--- with an already existing geometric-correlation-file read out the maximal minimalDistance between two structures--------#
    openGC_File = open_GeometricCorrelFile()
    openGC_File.seek(4*4)
    maxMinimalDistance = unpack('L', openGC_File.read(4))[0]
    openGC_File.close()
      
  #=== assign the list of all structures and all correlations of the loaded case to the modules corresponding fields ===========#    
  ctx.field("caseLoaded").value    = True 
  ctx.field("allStructures").value = "', " + str(listOfStructures)[1:-2] 
  
  ctx.field("gc_NeighbThreshFar").setMaxValue(maxMinimalDistance) 

  #--- compute the infiltration hazard of benign Structs (Dict{hazard:[structs]} and --> into the MetaCorrelation-Dictionary ---#
  dict_benignStruct = get_InfiltrationHazard(dictOfMetaCorrelations[INF_CHARACTER][_TYPE_BENIGN])
  if dict_benignStruct:
    dictOfMetaCorrelations[INF_HAZARDSTATE] = {}
    for degree_structs in dict_benignStruct.iteritems():
      dictOfMetaCorrelations[INF_HAZARDSTATE][str(degree_structs[0])] = degree_structs[1]
        
  #--- sort related Structures of meta-correls by TSP (insert) -----------------------------------------------------------------#  
#  print 'sort all correlations by TSP'
  for correlation in dictOfMetaCorrelations.iterkeys():
    for correlType in dictOfMetaCorrelations[correlation].iterkeys():
  #    print correlation + " + " + correlType
      dictOfMetaCorrelations[correlation][correlType] = sort_structures_TSP_Insert(dictOfMetaCorrelations[correlation][correlType], searchCriterias = [[correlation, "==", correlType]])   
      pass 
  
  ctx.field("mc_DictContainer").value   = str(dictOfMetaCorrelations)
#  get_AllMetaCorrels()  
  
  ctx.field("pc_malignantStructures").value = str(dict_pathologicCharacter[_TYPE_PATHOLOGIC])[1:-1]
  ctx.field("pc_doubtfulStructures").value  = str(dict_pathologicCharacter[_TYPE_DOUBTFUL])[1:-1]
  ctx.field("pc_benignStructures").value    = str(dict_benignStruct)[1:-1]               # single Lists are ordered by the min Distance to the worst neighbour
  
  ctx.log("finish Correlation-Calculation ;O)") 
  pass # //handleLoadedEvent()


#*******************************************************************************************************************************#
#*** Handle Fieldlistner-Calls for demand and compare Structures ***************************************************************#
#*******************************************************************************************************************************#

#~~~ Traversing the List of Structures of Current Case ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#=== Returns the first structure of the structurelist of the current loaded case to the field 'demandStructure' ================#
def update_firstDemandStructure():
  """Returns the first structure of the structurelist of the current case to the field 'demandStructure' of the METKCorrelations- 
     Module if the trigger 'first_dStructure' in the same module will be touched. """
  
  listOfStructures = eval("["+ ctx.field("allStructures").value[3:] + "']")
  
  ctx.field("indice_csdStruct").value = 0
  
  if listOfStructures:
    ctx.field("success").value         = True
    ctx.field("demandStructure").value = listOfStructures[0]     
  else:
    ctx.field("success").value         = False
    ctx.field("demandStructure").value = ""
  pass 

#=== Returns the next structure (indice csd) in the structurelist of the current case to the field 'demandStructure' ===========#
def update_nextDemandStructure():
  """ Returns the next structure in the structurelist for indice csd ( indice - current selected demand - structure) of the 
      current case to the field 'demandStructure' of the METKCorrelations- Module if the trigger 'next_dStructure' in the same 
      module will be touched. \n\
      !!! Before this method could be used the trigger 'first_dStructure' in this module has to be triggered!!!"""
     
  listOfStructures = eval("["+ ctx.field("allStructures").value[3:] + "']")
  
  if listOfStructures:
    indice        = ctx.field("indice_csdStruct").value + 1
    
    if indice < len(listOfStructures):
      ctx.field("success").value          = True 
      ctx.field("demandStructure").value  = listOfStructures[indice]
      ctx.field("indice_csdStruct").value = indice
    else:
      ctx.field("success").value          = False
      ctx.field("demandStructure").value  = ""
  else:
    ctx.field("success").value          = False
    ctx.field("demandStructure").value  = ""
  pass 

#=== Returns the first structure of the structurelist of the current loaded case to the field 'compareStructure' ===============#
def update_firstCompareStructure():
  """Returns the first structure of the structurelist of the current case to the field 'compareStructure' of the 
     METKCorrelations-Module if the trigger 'firstStructure' in the same module will be touched. """

  ctx.field("indice_cscStruct").value = 0
  listOfStructures                    = eval("["+ ctx.field("allStructures").value[3:] + "']")  
  
  if listOfStructures:
    ctx.field("success").value          = True
    ctx.field("compareStructure").value = listOfStructures[0]     
  else:
    ctx.field("success").value          = False
    ctx.field("demandStructure").value  = ""
  pass 

#=== Returns the next structure (indice csc) in the structurelist of the current case to the field 'compareStructure' ==========#
def update_nextCompareStructure():
  """Returns the next structure in the structurelist for indice cscd ( indice - current selected compare - structure ) of the 
    current case to the field 'comparestructure' of the METKCorrelations- Module if the trigger 'nextStructure' in the same module 
    will be touched. \n\ ! Before this method could be used, the trigger 'firstStructure' in this module has to be triggered!"""
  
  listOfStructures = eval("["+ ctx.field("allStructures").value[3:] + "']")  

  if listOfStructures:
    indice        = ctx.field("indice_cscStruct").value + 1    
    if indice < len(listOfStructures):
      ctx.field("success").value          = True 
      ctx.field("compareStructure").value = listOfStructures[indice]
      ctx.field("indice_cscStruct").value = indice
    else:
      ctx.field("success").value           = False
      ctx.field("compareStructure").value  = ""
  else:
    ctx.field("success").value           = False
    ctx.field("compareStructure").value  = ""
  pass 

def handle_changeDemandStructure():
#  ctx.log("handle_changeDemandStructure()" ) 
  demandStruct = ctx.field("demandStructure").value
  if demandStruct:
    listOfStructures                    = eval("["+ ctx.field("allStructures").value[3:] + "']")      
    ctx.field("indice_csdStruct").value = listOfStructures.index(demandStruct)
  else: 
    ctx.field("indice_csdStruct").value = -1
  
  if ctx.field("calcEverythingWithSelection").value: 

    ctx.field("compareStructure").value = ""    
    update_MetaInfoCorrelations()
    update_GeometricCorrelationsOfD_Structure()
    update_pathologicCorrelations()     
  pass

def handle_changeCompareStructure():
#  ctx.log("handle_changeCompareStructure()")
  compareStructure = ctx.field("compareStructure").value
  if compareStructure:
    listOfStructures                    = eval("["+ ctx.field("allStructures").value[3:] + "']")      
    ctx.field("indice_cscStruct").value = listOfStructures.index(compareStructure)
  else: 
    ctx.field("indice_cscStruct").value = -1
    
  if ctx.field("calcEverythingWithSelection").value:          
    update_jointMetaCorrels_DC()
    update_GeometricCorrelsBetweenDemand_Compare()
  pass



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~ CORRELATIONS within the META-INFORMATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#~~~ ListView of all Meta-Info -Collerations; updated with opened window  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

def openWindow():        
#  ctx.log("openWindow()")
  global _correlList
  if ctx.field("caseLoaded").value:
    _correlList = ctx.control("correlations")
    update_MetaCorrelListView()
  pass 

def closeWindow():
   global _correlList
   _correlList  = None
   return
 
  
def update_MetaCorrelListView():
  """ update the list of all correlations and their associated types with the coressponding structures in the modules panel"""
  global _correlList, _cls_info
#  ctx.log("update_MetaCorrelListView()")

  if _correlList:
#    correlList = ctx.control("correlations")
    _correlList.clearItems()
    
    _correlList.setRootIsDecorated(True)
    root = _correlList.insertItem()
    root.setText(0, "All Correlations")
    root.setOpen(True) 
    
    dictOfMetaCorrelations = eval(ctx.field("mc_DictContainer").value)    
        
    for correlation in dictOfMetaCorrelations.iterkeys():
      correl = _correlList.insertItem(root, 0)
      correl.setOn(True)
      correl.setText(0, correlation)
     
      for correlType in dictOfMetaCorrelations[correlation].iterkeys():              
        item = _correlList.insertItem(correl, 0)
        item.setText(0, correlType)
        
        for structure in dictOfMetaCorrelations[correlation][correlType]:
          child = _correlList.insertItem(item, 0) 
          child.setText(0, structure)    
  pass


#~~~ GET-METHODES for META-Info Correlations, Correlationtypes, and Coressponding Structures ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#=== get all Meta-Correlations which are contained in the case-Information (LAY_DISCIRIPTION) ==================================#
def get_MetaCorrels(demandStruct):
  global _cls_info
  
#  ctx.log("get_MetaCorrels_OfAStructure()")
        
  structCorrels = []
  if demandStruct:
    #=== traverse througout Layer Description of demand structure and find its Correlations ==================================#
    #--- activate the Description-Layer of the current structure -------------------------------------------------------------#
    _cls_info.activateObject(demandStruct)
    _cls_info.activateLayer(LAY_DESCRIPTION) 
    _cls_info.firstInfo(demandStruct) 
    
    while _cls_info.success():
      correlation = str(_cls_info.activeInfo())
      # -- takeover each INFO of the structure except Name -------------------------------------------------------------------# 
      if correlation != INF_NAME: 
        structCorrels.append(correlation)
      _cls_info.nextInfo()             
  return structCorrels
  pass 


#=== get all join correlations of the demand-structure and the compare-structure ===============================================#
def get_jointMetaCorrels(struct1, struct2):
  """returns all join correlations of the demand-structure and the compare-structure to the field 'mc_CorrelationsD_C'. """
  global _cls_info
  
#  ctx.log("get_MetaCorrelsBetweenTwoStructs()")
  joinCorrels = [] 
    
  if struct1 and struct2:
    struct2 = ctx.field("compareStructure").value
        
    #===(1) traverse throughout Metacorrelations of struct1 and check and compare these with struct2 coresponding correls ====#
    allCorrelsOfStruct1 = get_MetaCorrels(struct1)
    for str1_correl in allCorrelsOfStruct1:      
      
      #--- store the joint correlations and their specific type in a list -------------------------------------#
      if _cls_info.existInfo(struct2, LAY_DESCRIPTION, str1_correl) \
        and _cls_info.get(struct2, LAY_DESCRIPTION, str1_correl) == _cls_info.get(struct1, LAY_DESCRIPTION, str1_correl):
        joinCorrels.append((str1_correl, str(_cls_info.get(struct1, LAY_DESCRIPTION, str1_correl))))                    
  
  return joinCorrels
  pass


def update_jointMetaCorrels_DC():
     
  demandStructure = ctx.field("demandStructure").value
  compareStructure = ctx.field("compareStructure").value
  if demandStructure and compareStructure:    
    ctx.field("mc_CorrelationsD_C").value = str(get_jointMetaCorrels(demandStructure, compareStructure))
  else:
    ctx.field("mc_CorrelationsD_C").value = ""
  pass


def update_MetaInfoCorrelations():
  
  if ctx.field("caseLoaded").value:
    if ctx.field("mc_SelectionOption").value == _CASE:
      dictOfMetaCorrelations                = eval(ctx.field("mc_DictContainer").value)
      ctx.field("mc_AllCorrelations").value = str(dictOfMetaCorrelations.keys())[2:-2]
      ctx.field("mc_Correlation").value     = dictOfMetaCorrelations.keys()[0]
    else: # correls of demand-structure
  
      demandStructure  = ctx.field("demandStructure").value
      if demandStructure:
        
        structCorrels = get_MetaCorrels(demandStructure)
        if structCorrels:
          ctx.field("success").value            = True
          ctx.field("mc_Correlation").value     = structCorrels[0]
          ctx.field("mc_AllCorrelations").value = str(structCorrels)[2:-2]
        else:
          ctx.field("success").value            = False 
          ctx.field("mc_Correlation").value     = ""
          ctx.field("mc_AllCorrelations").value = ""        
  pass 


def update_allTypes_OfAMetaCorrel(): 
  global _calcOfStructCorrelType
#  ctx.log("update_allTypes_OfAMetaCorrel()")
  if ctx.field("mc_SelectionOption").value == _CASE:
    correlation = ctx.field("mc_Correlation").value  
    
    if  correlation:
      ctx.field("success").value           = True
      dictOfMetaCorrelations               = eval(ctx.field("mc_DictContainer").value)
      ctx.field("mc_AllCorrelTypes").value = str(dictOfMetaCorrelations[correlation].keys())[2:-2]      
      ctx.field("mc_CorrelType").value     = dictOfMetaCorrelations[correlation].keys()[0]  
    else:
      ctx.field("success").value           = False 
      ctx.field("mc_AllCorrelTypes").value = ""
      ctx.field("mc_CorrelType").value     = ""    
  else:
    update_CorrelTyp_OfDStructsCSCorrel()
  pass


#=== get type of the demand structure's curently selected Correlation ==========================================================#
def update_CorrelTyp_OfDStructsCSCorrel():
  global _cls_info,_calcOfStructCorrelType
#  ctx.log("update_CorrelTyp_OfDStructsCSCorrel()")
  
  _calcOfStructCorrelType = True
  demandStruct            = ctx.field("demandStructure").value
  
  if demandStruct:
    correlType = _cls_info.get(demandStruct, LAY_DESCRIPTION, ctx.field("mc_Correlation").value)
    ctx.field("success").value       = True   
    ctx.field("mc_CorrelType").value = ctx.field("mc_AllCorrelTypes").value = correlType    
  else:
    ctx.field("success").value       = False 
    ctx.field("mc_CorrelType").value = ctx.field("mc_AllCorrelTypes").value = ""
  
  _calcOfStructCorrelType = False 
  pass

def update_RelatedStructures_OfAMetaCorrelType():
#  ctx.log("update_RelatedStructures_OfAMetaCorrelType()")

  correlation = ctx.field("mc_Correlation").value
  correlType  = ctx.field("mc_CorrelType").value
  
  if correlation and correlType:
    dictOfMetaCorrelations                   = eval(ctx.field("mc_DictContainer").value)
    listOfRelatedStructs                     = dictOfMetaCorrelations[correlation][correlType]
    ctx.field("mc_RelatedStructures").value  = str(listOfRelatedStructs)[2:-2]
    indice = 0
    if _calcOfStructCorrelType:
      indice = listOfRelatedStructs.index(ctx.field("demandStructure").value)
       
    ctx.field("mc_csRelatedStructure").value = listOfRelatedStructs[indice] 
    ctx.field("mc_IndiceCsRelStruct").value  = indice
    ctx.field("success").value               = True
  else:
    ctx.field("success").value               = False 
    ctx.field("mc_RelatedStructures").value  = ""  
    ctx.field("mc_csRelatedStructure").value = ""
  pass


#~~~ TRAVERSATTION (Meta-Correlation-Stuff) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

def update_firstOfAllCorrelation():
  global _indice_MetaCorrelation
  
  _indice_MetaCorrelation = 0
  allCorrels              = eval("['" + ctx.field("mc_AllCorrelations").value + "']")
  
  if allCorrels:
    ctx.field("success").value        = True
    ctx.field("mc_Correlation").value = allCorrels[0] 
  else: 
    ctx.field("success").value        = False 
    ctx.field("mc_Correlation").value = ""
  pass 

def update_nextOfAllCorrelation():
  global _indice_MetaCorrelation
  
  _indice_MetaCorrelation += 1
  allCorrels               = eval("['" + ctx.field("mc_AllCorrelations").value + "']")
  
  if _indice_MetaCorrelation < len(allCorrels):
    ctx.field("success").value        = True 
    ctx.field("mc_Correlation").value = allCorrels[_indice_MetaCorrelation]
  else:
    ctx.field("success").value        = False
    ctx.field("mc_Correlation").value = ""
  pass 


def update_firstCorrelType():
  global _indice_MetaCorrelType
  
  _indice_MetaCorrelType = 0
  allCorrelTypes = eval("['" + ctx.field("mc_AllCorrelTypes").value + "']")
  
  if allCorrelTypes:
    ctx.field("success").value       = True 
    ctx.field("mc_CorrelType").value = allCorrelTypes[0]
  else:
    ctx.field("success").value       = False
    ctx.field("mc_CorrelType").value = ""    
  pass 

def update_nextCorrelType():
  global _indice_MetaCorrelType
  
  _indice_MetaCorrelType += 1
  allCorrelTypes = eval("['" + ctx.field("mc_AllCorrelTypes").value + "']")
  
  if _indice_MetaCorrelType < len(allCorrelTypes):
    ctx.field("success").value       = True
    ctx.field("mc_CorrelType").value = allCorrelTypes[_indice_MetaCorrelType]     
  else:
    ctx.field("success").value       = False
    ctx.field("mc_CorrelType").value = ""    
  pass 

def update_previousRelatedStruct():  
  stringOfRelatedStructs = ctx.field("mc_RelatedStructures").value
  
  if stringOfRelatedStructs:
    listOfRelatedStructs = eval("['" + stringOfRelatedStructs + "']")
    indice               = (ctx.field("mc_IndiceCsRelStruct").value - 1) % len(listOfRelatedStructs)
    ctx.field("mc_IndiceCsRelStruct").value  = indice
    ctx.field("mc_csRelatedStructure").value = listOfRelatedStructs[indice]   
  pass 

def update_nextRelatedStruct():
  stringOfRelatedStructs = ctx.field("mc_RelatedStructures").value
  if stringOfRelatedStructs:
    listOfRelatedStructs = eval("['" + stringOfRelatedStructs + "']")
    indice               = (ctx.field("mc_IndiceCsRelStruct").value + 1) % len(listOfRelatedStructs)
    ctx.field("mc_IndiceCsRelStruct").value  = indice
    ctx.field("mc_csRelatedStructure").value = listOfRelatedStructs[indice]   
  pass 


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~ GEOMETRIC-CORRELATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#=== get the procentual degree of Circumvallation by the visibility matrice frome byte-file 'camera_data.dat' ==================#
def build_OcclusionMatrix():
  """Build a nxn occlusion-"matrix" (data = list) up from the existing file 'camera_data.dat', initialy build for automatic camarapositioning).\
     With no existing File 'camera_data.dat' the user will be ask for another file direction and with no file False will be returned. 
     One entry = [procentual occlusion of i by j , procentual Viewpoints with j occluding i]\
     for more infos about the original-matrixes for every viewpoint see the coresponding module METKGeneratingCamData."""

#  ctx.log('get_Circumvallations for all structures from Visibility matrix from byte file camera_data.dat')
  
  filesDir  = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + "/camera_data.dat"  
  
  #--- with no existing files direction ask user for anoter dir, computing the file or to go on with no occlusion-infos --------#
  if not MLABFileManager.exists(filesDir):    
    x = ctx.field("MessageBox.show").touch()
    print x
    returnCode = ctx.field("MessageBox.returnCode").value
        
    #--- open from another direction ---#
    if returnCode == 1:
      filesDir = MLABFileDialog.getOpenFileName(filesDir, "datFile (*.dat)", "Open Camera-Data-File")      
    
    #--- open the METKGenerateCamData window as dialog ---#
    elif returnCode == 2:       
      moduleGenerateCamData = ctx.module("METKGenerateCamData")
      # moduleGenerateCamData.showParameterWindow()      
      hint = ctx.showWindow('generateCamDataDialog') 
      hint.move(0,0)
      moduleGenerateCamData.showModalDialog('generateCamData')
      hint.close()
      
      if not MLABFileManager.exists(filesDir):
        filesDir = MLABFileDialog.getOpenFileName(_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR), "datFile (*.dat)", "Open Camera-Data-File")
      
    #--- flag for no occlusion-computation ---#
    else:
      filesDir = ""
  
  if filesDir:
    
    fileSize = MLABFileManager.getFileSize(filesDir)
    
    with open(filesDir , 'rb') as file:      
      file.seek(12)                              # goes to position 12 (2* sizeOf(int)) in the file (header- numberOfStructs)
#      fileData     = file.read(4)               # reads the next 4 bytes (sizeOf(int))from the current position
#      division     = unpack('<L', fileData)[0]  # 3 --> 640 Viewports 4 --> 2562
      fileData     = file.read(4)                # reads the next 4 bytes from the current position 12(8 wenn division nicht auskommentiert)  
      numOfStructs = unpack('<L', fileData)[0]   # [< = little endian; L=unsigned Long] --> Tuple (number,)-->[0] 
#      versionHint  = file.read(3)               # 3*sizeOf(char) = 3*1
      file.seek(19)                              # skipp the versionHint and go to Position of the first matrix
      
      #--- creats a Matrix of [numStructs X numStructs] with 0 for each entry as preparation -----------------------------------#
      # ---add 3 rows: %# verdeckter Viewports, %# verdeckter Strukturen durch reihenstrukt  --#
      circumMatrix = []
      for i_List_row in range(numOfStructs+1):
        circumMatrix.append([])
        for i_List_column in range(numOfStructs):
          circumMatrix[i_List_row].append([0,0])
        
      numOfViewports = 0
      
      #--- for each viewport ---------------------------------------------------------------------------------------------------#
      while file.tell() < fileSize: #numOfViewports < 1: #
        
        numOfViewports += 1              
        
        #--- for every Matrix-row - respectly for each structure ---------------------------------------------------------------#
        for i_currentStruct in range(numOfStructs):

          #--- every value in one row except the last 5 will be devided by the whole number of visible Pixel of current Struct--
          #    und added to the value in the corresponding MatrixList's position -----------------------------------------------# 
          for i_rowValue in range(0, numOfStructs): # + 5):                     
            currentLineValue = unpack('f', file.read(4))[0]  # f for float -->Tuple (number,)-->[0] 
            if currentLineValue:
              circumMatrix[i_currentStruct][i_rowValue][0]  += currentLineValue # /allPixelOfCurStruct
              if i_currentStruct != i_rowValue:
                circumMatrix[i_currentStruct][i_rowValue][1] += 1 # number of viewports where struct i_currentStruct == i_rowValue occulde strukture i_row 
#                circumMatrix[numOfStructs][i_rowValue][1]    += 1 # number of viewports where struct i_currentStruct == i_rowValue occludes something                              
                circumMatrix[numOfStructs][i_rowValue][0]    += currentLineValue  # absolut number of occulted pixels by structure (numOfStructs == i_rowValue) of struct i_rowValue 

          file.seek(file.tell()+5*4 )     # skip last 5 Entries of Matrix-row (abs.number of occluded Pixel, minZ, maxZ, ? , ? ) ? maby silhouettenlength
      
    #--- travers matrix column by column --------------------# 
    for i_column in range(numOfStructs):  
      for i_row in range(numOfStructs):
        if i_column != i_row:
          procentualOccult                           = round( 100 * circumMatrix[i_row][i_column][0] / circumMatrix[i_row][i_row][0]) # %ual absult occlusion of struct[row] of struct[column]
          circumMatrix[i_row][i_column][0]           = procentualOccult
          circumMatrix[numOfStructs][i_column][0]   += procentualOccult
          procentualViewports                        = round( 100 * circumMatrix[i_row][i_column][1] / numOfViewports)
          circumMatrix[i_row][i_column][1]           = procentualViewports
          circumMatrix[numOfStructs][i_column][1]   += procentualViewports
         
    #--- travers additional rows of matrix column by column ---------------------------------------------#      
    for i_column in range(numOfStructs):
      circumMatrix[numOfStructs][i_column][0] = round(circumMatrix[numOfStructs][i_column][0] / numOfStructs)
      circumMatrix[numOfStructs][i_column][1] = round(circumMatrix[numOfStructs][i_column][1] / numOfViewports)   
        
    return circumMatrix
  
  else:  
    MLAB.logWarning("No files no occlusion-computation1")
    ctx.log("No files no occlusion-computation2")
    return False 
    pass
  pass 


#===  ===#
def build_Intersection_Matrix(listOfStructures):
  
  matrixSize   = len(listOfStructures)
  intersMatrix = []
  fileDir      = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
#  ctx.field("Arithmetic1.function").setStringValue("Divide (Const/Img)") 
#  ctx.field("Arithmetic11.function").setStringValue("Divide (Const/Img)")
#  ctx.field("Intersection.function").setStringValue("Multiply")
#  ctx.field("Intersection.constant").setIntValue(1)

  openGeomFile = open_GeometricCorrelFile()
  
  for index1, structure1 in enumerate(listOfStructures):
    
    ctx.field("Structure1.filename").setStringValue(fileDir + _cls_info.getImageInfo(structure1, INF_FILENAME))
    ctx.field("Arithmetic1.constant").setValue(_cls_info.getImageInfo(structure1, INF_OBJVALUE, INFOTYPE_INT32) * 10)
     
    intersMatrix.append([ctx.field("Struct1Volume.totalVolume").value])    
    
    for index2 in range(index1+1, len(listOfStructures)):
      minDist = get_minimalDistance(openGeomFile, index1, index2)

      if minDist < 1:
        ctx.field("Structure2.filename").setStringValue(fileDir + _cls_info.getImageInfo(listOfStructures[index2], INF_FILENAME))
        ctx.field("Arithmetic11.constant").setValue(_cls_info.getImageInfo(listOfStructures[index2], INF_OBJVALUE, INFOTYPE_INT32) * 10)
        MLAB.processEvents()
        intersMatrix[index1].append(ctx.field("IntersectionVolume.totalVolume").value) 
  #      ctx.field("Structure2.close").touch()
      else:
        intersMatrix[index1].append(0)       
    
    ctx.field("Structure1.close").touch()
  
  openGeomFile.close()  
  return intersMatrix
  pass 


def get_minimalDistance(openFile, i_struct1, i_struct2):  

#  ctx.log("get_minimalDistance(openFile, i_struct1, i_struct2)")
  i_max = max(i_struct1, i_struct2)
  i_min = min(i_struct1, i_struct2)
  
  #--- get startposition of min-distances from header, and from there the wanted position of min-distance entry ---#
  openFile.seek(4)
  start = unpack('<L', openFile.read(4))[0]   
  openFile.seek(start + (factorial(i_max-1)+ i_min)*4)   
  return unpack('f', openFile.read(4))[0]    
  pass


def print_minimalDistanceHalfMatrix():
  """just for testing! could be started from init to have a look at all minimal distances and print the stuff in the control panel"""
  try: 
    allStructs = eval("["+ ctx.field("allStructures").value [3:] + "']")  
    openFile   = open_GeometricCorrelFile()
    numStructs = unpack('<L', openFile.read(4))[0] 
    start      = unpack('<L', openFile.read(4))[0] 
    print numStructs
    print start
    count = 0
    print "all MinDistances"
    openFile.seek(start)
    for i in range(1,numStructs):
      count += 1
      for j in range(count):
        print allStructs[i] + " (" + str(i) + ") & " + allStructs[j] + " (" + str(j) + ") :: " + str(unpack('f', openFile.read(4))[0])  
    openFile.close()
  except ValueError: ctx.log("no case loaded or no structures in case")
  pass


#/// not ordered, yet ///#
def get_Structs_with_minDist_fullfillCriterias(openFile, i_structure, distThresholds = None, searchCriterias = None ):
  """ find all neighbours of a structure (within a certain intervall, if given) and with specified compiled meta-Info-Criterias """
  global _cls_info

  if distThresholds:
    print distThresholds
  else: print 'No distThresh'

  listOfDist_Neighb = []                                                    # result list
  listOfStructures  = eval('['+ ctx.field("allStructures").value[3:]+"']")  # list of all structures to get index of structureID, resp. structureId at index
#  ctx.log("get_Structs_with_minDist_fullfillCriterias(openFile, + str(listOfStructures[i_structure]) + , ...)")
  
  #--- with no demand-structure compute all structurs compile the search criterias with none distance-infos --------------------#
  if i_structure == -1:
    if searchCriterias:
      for structure in listOfStructures:
        for criteria in searchCriterias:
          if criteria[1] == "=="   and _cls_info.get(structure, LAY_DESCRIPTION, criteria[0]) != criteria[2] \
            or criteria[1] == "!=" and _cls_info.get(structure, LAY_DESCRIPTION, criteria[0]) == criteria[2]:                    
            break
        else:
          listOfDist_Neighb.append([None, structure])
  
  #--- compute all structures fullfill the searchcriteras with an min-dist to structure[i] within the distance-threshold -------#
  else:
    #--- get headerInfo like number of structures and the startposition of minimalDistances in file ----------------------------#
    openFile.seek(0)
    numStructs  = unpack('L', openFile.read(4))[0]  
    startMatrix = unpack('L', openFile.read(4))[0] 
    
    #--- start at first position of min-distance-entries of i_structure in file and traverse the "half-matrix-row" i < i_struct --#
    fileIndex = startMatrix + (factorial(i_structure-1)*4)
    openFile.seek(fileIndex)
  
    if i_structure == 0:    #extra handling for index 0 
      index = 1
    
    else:    
      for index in range(i_structure):
        fileData = openFile.read(4) # parallel movement in file to for-loop
        
        #--- if search criterias are given check for their compilation and flag the result -------------------------------------#
        criteriaCompiled = True 
        if searchCriterias:
          for criteria in searchCriterias:
            if criteria[1] == "=="   and _cls_info.get(listOfStructures[index], LAY_DESCRIPTION, criteria[0]) != criteria[2] \
              or criteria[1] == "!=" and _cls_info.get(listOfStructures[index], LAY_DESCRIPTION, criteria[0]) == criteria[2]:          
              criteriaCompiled = False 
              break
        
        #--- if all given search criterias are comiled, check if mindistance is in the contingently given distance-threshold ---#
        if criteriaCompiled: 
          minDist = unpack('f', fileData)[0]      
          #--- if so, append the tuple including the minimal distance and the structure-ID to the result list ------------------#
          if not distThresholds or distThresholds[0] < minDist < distThresholds[1]:          
            listOfDist_Neighb.append([minDist, listOfStructures[index]])
            
      index += 2 # = i_structure + 1
      fileIndex = openFile.tell()
        
    #--- now find all min-distance entries with comparation-partners with an higher index then i_structure and do the same stuff -#  
    for index2 in range(index, numStructs):
      fileIndex += 4 * (index2-1)  # increment in GC-file for min-distances with i_structure as one compartion index 
      criteriaCompiled = True 
      
      if searchCriterias:
        for criteria in searchCriterias:
          if criteria[1] == "=="   and _cls_info.get(listOfStructures[index2], LAY_DESCRIPTION, criteria[0]) != criteria[2] \
            or criteria[1] == "!=" and _cls_info.get(listOfStructures[index2], LAY_DESCRIPTION, criteria[0]) == criteria[2]:
            criteriaCompiled = False 
            break
          
      if criteriaCompiled:       
        openFile.seek(fileIndex)
        minDist = unpack('f', openFile.read(4))[0]
      
        if not distThresholds or distThresholds[0] < minDist < distThresholds[1]:
          listOfDist_Neighb.append([minDist, listOfStructures[index2]]) 
        
  if not listOfDist_Neighb:  return [[None ,None]] # return this with no result (no compiled search criterias and distance-thresh)
  else: return listOfDist_Neighb 
  pass 


def get_OcclusionInfos(openFile, i_struct1, i_struct2):  
  """get_OcclusionInfos(open-Geometric-Correls-File, occluded Structure, occluding Structure) returns the occlusionInfo [procentual Occlusion 1 by 2, procentual CameraViews with occlusion of 1 by 2] from given file"""
  
  #--- read out relevant header infod like absolut number of structures in case and startindex for occlusionInfos --------------#
  openFile.seek(12)
  start   = unpack('L', openFile.read(4))[0]
  
  if start != 0:
    openFile.seek(0)
    numStructs = unpack('L', openFile.read(4))[0]
    
    openFile.seek(start + (i_struct1 * numStructs*2 + i_struct2*2 )*4)   
    occlusion1by2    = unpack('L', openFile.read(4))[0] 
    numViewsOccl1by2 = unpack('L', openFile.read(4))[0]  
    return [occlusion1by2, numViewsOccl1by2]
  
  else:
    return [-1,-1]
  pass
 

#=== returns the concealers (structures occluding demand-structure) and structures occluded by demand-structure to the fields ==#
def get_OccluderOf(openFile, i_structure, lowThresh, highThresh):
#  ctx.log("get_OccluderOf(openFile, " + str(i_structure) + ", " + str(lowThresh) + ", " + str(highThresh) +")")
  
  occluder    = []
  openFile.seek(12)                                      # header: fileIndex of startposition of occlusion-matrix
  start = unpack('L', openFile.read(4))[0]              # if start = 0 --> no occlusionMatriax avaiable
  
  if start:
    allStructs  = eval("[" + ctx.field("allStructures").value[3:] + "']")
    openFile.seek(0)
    numStructs = unpack('L', openFile.read(4))[0]         # header: total number of structures
              # gives startposition of occlusion-matrix  
    openFile.seek(start + (i_structure * numStructs*2)*4)
    
    for index in range(numStructs):
      occlusion = openFile.read(4) 
      if index != i_structure and lowThresh <= unpack('L', occlusion)[0] < highThresh: 
        occluder.append(allStructs[index])
      openFile.read(4) # skip number of viewpoints with occlusion for this occlusionInfo
  
  return occluder
  pass 


def get_OccludedBy(openFile, i_structure, lowThresh, highThresh):
  
#  ctx.log("get_OccludedBy(openFile, i_structure, lowThresh, highThresh)")
  
  occludes    = []
  openFile.seek(12)
  start = unpack('L', openFile.read(4))[0] # header: fileIndex of startposition of occlusion-matrix: 0--> no occlusion-Infos avaiable
  
  if start:
    allStructs  = eval("[" + ctx.field("allStructures").value[3:] + "']")  
    
    openFile.seek(0)
    numStructs = unpack('L', openFile.read(4))[0]               # header: total number of structures
    start     += i_structure*2*4  # gives startposition of occlusion-matrix + index 
    openFile.seek(start)
    
    for index in range(numStructs-1):    
      occlusion = openFile.read(4)
      if index != i_structure and lowThresh <= unpack('L', occlusion)[0] < highThresh: 
        occludes.append(allStructs[index])
      start += numStructs*2*4
      openFile.seek(start)
  
  return occludes
  pass  


def get_IntersectionVolume(openFile, i_struct1, i_struct2):
  openFile.seek(0)
  numStructs = unpack('L', openFile.read(4))[0]
  openFile.seek(8)
  
  start = unpack('L', openFile.read(4))[0]
  i_min = min(i_struct1, i_struct2)
  i_max = max(i_struct1, i_struct2) 
  openFile.seek(start + (i_min*numStructs - factorial(i_min-1) + i_max - i_min)*4)   # first colum includes the volume of Structs therfore (i_min-1 and no -1 at the end (...-1)*4)!
  return unpack('f', openFile.read(4))[0]    
  pass

def get_intersectionPartners(openFile, i_structure):
  pass


 


def update_GeometricCorrelationsOfD_Structure():
  global _epsilon
  
  indexDemandStruct = ctx.field("indice_csdStruct").value
  if indexDemandStruct != -1:        
            
    lowOcclusionThresh  = ctx.field("gc_OcclusThreshLow").value  - _epsilon 
    highOcclusionThresh = ctx.field("gc_OcclusThreshHigh").value + _epsilon     
    
    openFile = open_GeometricCorrelFile()
    
    ctx.field("gc_Neigbours").value      = map(operator.itemgetter(1), get_Structs_with_minDist_fullfillCriterias(openFile,  indexDemandStruct, distThresholds = [ctx.field("gc_NeighbThreshNear").value - _epsilon, ctx.field("gc_NeighbThreshFar").value + _epsilon]))
    ctx.field("gc_Concealer").value      = get_OccluderOf(openFile, indexDemandStruct, lowOcclusionThresh, highOcclusionThresh)
    ctx.field("gc_coveredObjects").value = get_OccludedBy(openFile, indexDemandStruct, lowOcclusionThresh, highOcclusionThresh)
    
    openFile.close()       
  else:
    ctx.field("gc_Neigbours").value      = ''  
    ctx.field("gc_Concealer").value      = ''
    ctx.field("gc_coveredObjects").value = ''
  pass

#=== returns the distance of the two structures in the geometric Tab into the distance field ===================================#
def update_GeometricCorrelsBetweenDemand_Compare():
  """ determine the distance between two selected Structures and publish it in the panel"""
  
#  ctx.log("get_GeometricCorrelsBetweenTwoStructs()")
  indexDemand   = ctx.field("indice_csdStruct").value
  indexCompare  = ctx.field("indice_cscStruct").value
    
  if indexDemand == indexCompare or indexDemand == -1 or indexCompare == -1:
    ctx.field("gc_DC_Distance").value           = -1
    ctx.field("gc_DC_absOcclut_DC").value       = -1
    ctx.field("gc_DC_absOcclut_CD").value       = -1
    ctx.field("gc_DC_amountViews_DC").value     = -1
    ctx.field("gc_DC_amountViews_CD").value     = -1
    ctx.field("gc_DC_IntersectionVolume").value = -1
    ctx.field("gc_DC_Nested").value             = -1
  else:
    openFile = open_GeometricCorrelFile()
   
    ctx.field("gc_DC_Distance").value           = get_minimalDistance(openFile, indexDemand, indexCompare)      
    occlusionInfos                              = get_OcclusionInfos(openFile, indexDemand, indexCompare)
    ctx.field("gc_DC_absOcclut_DC").value       = occlusionInfos[0]
    ctx.field("gc_DC_amountViews_DC").value     = occlusionInfos[1]
    occlusionInfos                              = get_OcclusionInfos(openFile, indexCompare, indexDemand)
    ctx.field("gc_DC_absOcclut_CD").value       = occlusionInfos[0]    
    ctx.field("gc_DC_amountViews_CD").value     = occlusionInfos[1]    
    ctx.field("gc_DC_IntersectionVolume").value = get_IntersectionVolume(openFile, indexDemand, indexCompare)
    ctx.field("gc_DC_Nested").value             = -1 # to DO 

    openFile.close()
  pass


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~ PATHOLOGIC-CORRELATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

#=== delete all Criteria for the selected pathologic Character =================================================================#
def delete_AllCharacterCriteria():
  """ Deletes all criteria for the selected pathologic character. """
  
  if ctx.field("pc_CiteriaCharacter").value == _TYPE_PATHOLOGIC:
    ctx.field("pc_malignantCriteria").value = []
  else:
    ctx.field("pc_doubtfulCriteria").value = []  
  pass 


#=== define a new Criteria (or delete one) for the pathologic Character ========================================================#
def set_CharacterCriteria():
  """ Define a new criteria for the selected pathologic character \n\
      and its dergree of severity with the specific given property. """
  
  ctx.log("set_CharacterCriteria()")
  character      = ctx.field("pc_CiteriaCharacter").value         # malignat, doubtful or benign
  structuretype  = str(ctx.field("pc_CiteriaStructure").value)    # e.g. Tumor or Lymphnode
  strProperty    = str(ctx.field("pc_CriteriaStrProberty").value) # e.g. size in max extension 
  condition      = str(ctx.field("pc_CriteriaCondition").value)   # e.g. <, >= ...
  degreeSeverity = ctx.field("pc_CritDegreeOfSeverity").value
  
  if structuretype:
    
    #--- load the previos Criteria-List for the selected character -------------------------------------------------------------# 
    if character == _TYPE_PATHOLOGIC:
      allCriteria = eval(ctx.field("pc_malignantCriteria").value)
    else:  
      allCriteria = eval(ctx.field("pc_doubtfulCriteria").value)
      
    #--- look if the structuretyp is already stored in the list and insert it accordingly as new element or as a tupel of one --
    #    or delete the whole element or just a property analogously if the dedition-option is delete and not new. --------------# 
    for i_criteriaStruct in range(len(allCriteria)):   
      if structuretype == allCriteria[i_criteriaStruct][0]:   

    #--- if for this structuretype more then one degree of severity are stored (also property =() =empty)-----------------#
#        if len(allCriteria[i_criteriaStruct]) != 2: 
        for degree in allCriteria[i_criteriaStruct][1:]:
          if degree[0] == degreeSeverity:
            degree.append((strProperty, condition, float(ctx.field("pc_CriteriaProbValue").value)))
            break 
        else:
          allCriteria[i_criteriaStruct].append([degreeSeverity, (strProperty, condition, float(ctx.field("pc_CriteriaProbValue").value))]) 
        break 
    
    #--- insert as new Element (1st appearance of that structuretyp) and editing-opion = 'new' ---------------------------------#
    else: # if for-loop falls through
      if ctx.field("pc_CriteriaEditOption").value == "new":
        if strProperty:
          allCriteria.append([structuretype, [degreeSeverity,()]])              
        else:  
          allCriteria.append([structuretype, [degreeSeverity, (strProperty, condition, float(ctx.field("pc_CriteriaProbValue").value))]])
    
    #--- return the list to the associated module's field ----------------------------------------------------------------------#
    if character == _TYPE_PATHOLOGIC:
      ctx.field("pc_malignantCriteria").value = allCriteria 
    else:  
      ctx.field("pc_doubtfulCriteria").value = allCriteria      
  pass


##### noch kucken wie das mit dem Surface_distances zeugs funzt und daraus dann evtl. die gefährdete Fläche berechnen was mit einbezogen wird ##########################
def get_InfiltrationHazard2(openFile, benignStruct):

  global _cls_info
  infiltHazard          = 0 # origin degree of endangered         
  worstSTruct           = None 
  distToWorstCaseStruct = 0
  maxDegreeOfSeverity   = 0  
#  listsOfpatNeighbours  = get_pathologNeighbours(benignStruct)  # not-benign neighbours within infiltrationarea (list[0]) and within not-safe-area (list[1]) 
  listOfallStructs      = eval("[" + ctx.field("allStructures").value[3:] + "']")

  #=== calc. degreeOfEndangered of b-struct: DistanceArea-Influence + degreeOfSeverity of dangerous Struct of the worst case =#
  patholInfiltNeighbours = get_Structs_with_minDist_fullfillCriterias(openFile, listOfallStructs.index(benignStruct), distThresholds = [0, ctx.field("pc_InfiltrationDistance1").value], searchCriterias = [[INF_CHARACTER, "!=", _TYPE_BENIGN]]) 
   
  if patholInfiltNeighbours[0][0]:

    for infNeighbour in patholInfiltNeighbours:
      dOfS = _cls_info.getInt(infNeighbour[1], LAY_DESCRIPTION, INF_DEGREEofSEVERITY) 
      
      if dOfS > maxDegreeOfSeverity:
        maxDegreeOfSeverity   = dOfS
#        worstStruct           = infNeighbour[1]
        distToWorstCaseStruct = infNeighbour[0] 
      
      infiltHazard  = ctx.field("pc_InfiltDist1_Degree").value + maxDegreeOfSeverity     
  
  patholInfiltNeighbours = get_Structs_with_minDist_fullfillCriterias(openFile, listOfallStructs.index(benignStruct), distThresholds = [ctx.field("pc_InfiltrationDistance1").value, ctx.field("pc_InfiltrationDistance2").value], searchCriterias = [[INF_CHARACTER, "!=", _TYPE_BENIGN]])
  if patholInfiltNeighbours[0][0]:  

#   worstStruct2           = none
    distToWorstCaseStruct2 = 0
    for infNeighbour in patholInfiltNeighbours:
      dOfS = _cls_info.getInt(infNeighbour[1], LAY_DESCRIPTION, INF_DEGREEofSEVERITY) 
      
      if dOfS > maxDegreeOfSeverity:
        maxDegreeOfSeverity    = dOfS
#        worstStruct2          = infNeighbour[1]
        distToWorstCaseStruct2 = infNeighbour[0] 
      
    infiltHazard2 = ctx.field("pc_InfiltDist2_Degree").value + maxDegreeOfSeverity 
    if infiltHazard2 > infiltHazard: 
      infiltHazard          = infiltHazard2
      distToWorstCaseStruct = distToWorstCaseStruct2
    
  #--- if no dangerous structures within the not-safe-distace (and infilt-Dist) -> benign struct is safe-> degree = 0 ------#
  return [infiltHazard, distToWorstCaseStruct] 
  pass


#== get infiltration hazard of benign Structures by "be most-endangered" by pathological structures (distance) =================#     
def get_InfiltrationHazard(listOfBenignStructs):
  
  ctx.log("get_InfiltrationHazard(listOfBenignStructs)")
  
  sortedEndangered_B_Structures = {} 
  infiltHazard                  =  0 
      
  openFile = open_GeometricCorrelFile()
  #--- find for each benign Structure its infiltration Hazard ------------------------------------------------------------------#
  for benignStruct in listOfBenignStructs:    
    
    #--- if pathologic Informations (Character + newHazard) should be new calulated or if ther isn't stored a value yet --------#
    if not ctx.field("pc_takeGivenPatholInfos").value or not _cls_info.existInfo(benignStruct, LAY_DESCRIPTION, INF_HAZARDSTATE):      
      infiltInfos  = get_InfiltrationHazard2(openFile, benignStruct)
      infiltDist   = infiltInfos[1]
      infiltHazard = str(infiltInfos[0])   
    else: # take the stored value from the case if the checkbox is thrue
      infiltHazard = _cls_info.existInfo(benignStruct, LAY_DESCRIPTION, INF_HAZARDSTATE)
      
    #--- assign the structure to the dictionary-of-hazards by ordering in to an existing list or creating a new one ------------#   
    if sortedEndangered_B_Structures.has_key(infiltHazard):            
      list = sortedEndangered_B_Structures[infiltHazard] 
      
      #--- sort by distance to worst-case structure ----------------------------------------------------------------------------#
      for i_listItem in range(len(list)):
        if infiltDist < list[i_listItem][1]:
          list.insert(i_listItem, [benignStruct, infiltDist])
          break
      else:
        list.append([benignStruct, infiltDist])
      sortedEndangered_B_Structures[infiltHazard] = list
    else:
      sortedEndangered_B_Structures[infiltHazard] = [[benignStruct, infiltDist]]
      pass
    
    _cls_info.typedSet(benignStruct, LAY_DESCRIPTION, INF_HAZARDSTATE, infiltHazard, INFOTYPE_INT32)
  
 
  #--- return only the sorted structures without the minWorstDistance ----------------------------------------------------------#
  listOfHazards = sortedEndangered_B_Structures.keys()
#  listOfHazards.sort()

  for haz in listOfHazards:
    resultList = []
    for listItem in sortedEndangered_B_Structures[haz]:
      resultList.append(listItem[0])
    sortedEndangered_B_Structures[haz] = resultList
  
  openFile.close()
  return sortedEndangered_B_Structures
  pass


def get_DegreeSeverity(maxExtension, listOfDegrees):
  """compute the Degree of Severity (defined by the criteria for pathol. Structs in the panel) for not-benign Structures """        

#  ctx.log("get_DegreeSeverity("+maxExtension+", "+listOfDegrees+")")
  degree = 0 # =False
  # Suche den Schweregrad für den pathologischen Charater der Struktur
  # durchlaufe alle Schweregradeinträge für die strukturart
  for degreeOfSeverityInfo in listOfDegrees:               # malignantCriteria[i_maligCrit][1:]:
    #--- take degree of current condition --------------------------------------------------------------------------------------#
    degree = degreeOfSeverityInfo[0] #take degree 'of current conditions'

    #--- and test all conditions for this degree of severity for completeness ; all met -> correct degree is found -------------#
    for condition in degreeOfSeverityInfo [1:]:
      if condition:
        if condition[0] == "sizeInMaxExtension":        
          if condition[1] == '<=' and maxExtension > condition[2]:                            
              degree = 0
              break
          elif condition[1] == '>' and maxExtension <= condition[2]:                           
              degree = 0 
              break
    if degree:
      return degree 
    
  return degree 
  pass     


#=== compute the pahtologic character of a structure via the Charater-Criteria =================================================#
def get_pathologCharacter(structure, dict_pathologicCharacter):
  
#  ctx.log("get_pathologCharacter(" + structure + ")")
  global _cls_info
  
  #--- if pathologic Informations (Character + newHazard) should be new calulated or if ther isn't stored a value yet --------#
  if not ctx.field("pc_takeGivenPatholInfos").value or not _cls_info.existInfo(structure, LAY_DESCRIPTION, INF_CHARACTER):
  
    maxExtension     = 0
    character        = _TYPE_BENIGN
    degreeSeverity   = 1
    kindOfStructure  = _cls_info.get(structure, LAY_DESCRIPTION, INF_STRUCTURE)
  
    #--- check for malignant character -----------------------------------------------------------------------------------------#
    malignantCriteria = eval(ctx.field("pc_malignantCriteria").value)   
    # durchsuche die malignant Kriterien nach der Structurart
  
    for i_maligCrit in range(len(malignantCriteria)):
    
      #- if kind of Structure included, (means) Structure has malignant Character -#
      if malignantCriteria[i_maligCrit][0] == kindOfStructure:
        character      = _TYPE_PATHOLOGIC                
        maxExtension   = float(_cls_info.get(structure, LAY_MEASURES, INF_MAX_DIAMETER))     
        degreeSeverity = str(get_DegreeSeverity(maxExtension, malignantCriteria[i_maligCrit][1:]))
  
        if dict_pathologicCharacter[_TYPE_PATHOLOGIC].has_key(degreeSeverity):
          currenList = dict_pathologicCharacter[_TYPE_PATHOLOGIC][degreeSeverity]
          for i_structSize in range(len(currenList)):
            if maxExtension > _cls_info.get(currenList[i_structSize], LAY_MEASURES, INF_MAX_DIAMETER):
              dict_pathologicCharacter[_TYPE_PATHOLOGIC][degreeSeverity][i_structSize:i_structSize]= [structure]
              break 
          else:
            dict_pathologicCharacter[_TYPE_PATHOLOGIC][degreeSeverity].append(structure)
        else:
          dict_pathologicCharacter[_TYPE_PATHOLOGIC][degreeSeverity] = [structure]
        break 
      
    #--- check for doubtful character --------------------------------------------------------------------------------------------#   
    else:
      doubtfulCriteria = eval(ctx.field("pc_doubtfulCriteria").value)
  
      for i_doubtCrit in range(len(doubtfulCriteria)):
  
        if doubtfulCriteria[i_doubtCrit][0] == kindOfStructure:
          character      = _TYPE_DOUBTFUL
          maxExtension   = float(_cls_info.get(structure, LAY_MEASURES, INF_MAX_DIAMETER))       
          degreeSeverity = str(get_DegreeSeverity(maxExtension, doubtfulCriteria[i_doubtCrit][1:]) )

          if dict_pathologicCharacter[_TYPE_DOUBTFUL].has_key(degreeSeverity):
            currenList = dict_pathologicCharacter[_TYPE_DOUBTFUL][degreeSeverity]
            for i_structSize in range(len(currenList)):
              if maxExtension > _cls_info.get(currenList[i_structSize], LAY_MEASURES, INF_MAX_DIAMETER):
                dict_pathologicCharacter[_TYPE_DOUBTFUL][degreeSeverity][i_structSize:i_structSize]= [structure]
                break
            else:
              dict_pathologicCharacter[_TYPE_DOUBTFUL][degreeSeverity].append(structure)
          else:
            dict_pathologicCharacter[_TYPE_DOUBTFUL][degreeSeverity] = [structure]
          break  
     
     #--- Benign character -----------------------------------------------------------------------------------------------------#
      else:
        dict_pathologicCharacter[_TYPE_BENIGN].append(structure)
    
    _cls_info.typedSet(structure,  LAY_DESCRIPTION, INF_CHARACTER, character, INFOTYPE_STRING) 
    _cls_info.markInfoPrivate(INF_CHARACTER)
    if character != _TYPE_BENIGN:
      _cls_info.typedSet(structure,  LAY_DESCRIPTION, INF_DEGREEofSEVERITY, degreeSeverity, INFOTYPE_INT32)
    else:
      _cls_info.typedSet(structure, LAY_DESCRIPTION, INF_HAZARDSTATE, 0, INFOTYPE_INT32)
  
  return dict_pathologicCharacter           
  pass

def set_DemandCharachter_Hazard():
  """change the character and/or hazard (InfiltrationRisk/ DegreeOfSeverity) of the demand Structure,\n
     handle the possible changes of benigns structures infiltration hazard 
     and update the relevant dictionarys (Meta-Correl and the pathologic dicts (patholog, doubtf. and benign structs.))"""
  
  global _cls_info
 
  demandStruct = ctx.field("demandStructure").value  
  newCharacter = ctx.field("pc_characterOfD").value
  oldCharacter = _cls_info.getString(demandStruct, LAY_DESCRIPTION, INF_CHARACTER)
    
  #---  check for valid entries ------------------------------------------------------------------------------------------------#
  if demandStruct != "" and (newCharacter == _TYPE_PATHOLOGIC or newCharacter == _TYPE_BENIGN or newCharacter == _TYPE_DOUBTFUL):
 
    #--- get the new and old (infiltration) hazard (Degree of Severity for non beningns and Degree of Hazard for benigns -------# 
    newHazard        = str(ctx.field("pc_hazardState").value)
    oldHazard        = '0'                    # default     
    kindOfHazard_old = INF_DEGREEofSEVERITY   # kind of hazard: infiltrationHazard (for benign) or degree of severity (the other)
    if oldCharacter == _TYPE_BENIGN:
      oldHazard    = str(_cls_info.get(demandStruct, LAY_DESCRIPTION, INF_HAZARDSTATE))            
      kindOfHazard_old = INF_HAZARDSTATE 
    else:
      oldHazard = str(_cls_info.get(demandStruct, LAY_DESCRIPTION, INF_DEGREEofSEVERITY))   
  
    #--- compute the changes only when there are changes------------------------------------------------------------------------#
    if newCharacter == oldCharacter and newHazard == oldHazard:
      return 
    else: 
      
      dictMetaCorrels       = eval(ctx.field("mc_DictContainer").value)   # contains all Metacorrelations like the character-correlation with ists Typs pathol, benign and doubtful  
      dictOfpathologicDicts = {}                                          # contains the relevant pathologic Dictionarys ([old/newCharacter]{hazardState1: [member1-structures], ..., hazardState2: [member2-structures]}
      
      if newCharacter != oldCharacter:
  
        #--- commit new character to the dictOfMetacorrels (ordered) and to the ObjInfos and clear old directory ---------------#
        #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#       
        dictMetaCorrels[INF_CHARACTER][oldCharacter].remove(demandStruct)
        if dictMetaCorrels[INF_CHARACTER][oldCharacter]:
          dictMetaCorrels[INF_CHARACTER][oldCharacter] = sort_structures_TSP_Insert(dictMetaCorrels[INF_CHARACTER][oldCharacter])
        else:
          del dictMetaCorrels[INF_CHARACTER][oldCharacter]
        if dictMetaCorrels[INF_CHARACTER].has_key(newCharacter) : 
          dictMetaCorrels[INF_CHARACTER][newCharacter].append(demandStruct)
          dictMetaCorrels[INF_CHARACTER][newCharacter] = sort_structures_TSP_Insert(dictMetaCorrels[INF_CHARACTER][newCharacter])
        else:
          dictMetaCorrels[INF_CHARACTER][newCharacter] = [demandStruct]
        _cls_info.typedSet(demandStruct, LAY_DESCRIPTION, INF_CHARACTER, newCharacter, INFOTYPE_STRING)   
        
      #=== update the pathologic lists (malignat, doubtful and benign) and the metaCorrelDict in the coresponding fields -------#
      #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
      
      #--- bild a dictionary with the contens of the pathologic fields of the old and new character in pathologic Correls ------#
      if oldCharacter == _TYPE_BENIGN or newCharacter == _TYPE_BENIGN:
        dictOfpathologicDicts[_TYPE_BENIGN]     = eval("{" + ctx.field("pc_benignStructures").value + "}")
      if oldCharacter == _TYPE_DOUBTFUL or newCharacter == _TYPE_DOUBTFUL:
        dictOfpathologicDicts[_TYPE_DOUBTFUL]   = eval("{" + ctx.field("pc_doubtfulStructures").value + "}") 
      if oldCharacter == _TYPE_PATHOLOGIC or newCharacter == _TYPE_PATHOLOGIC:
        dictOfpathologicDicts[_TYPE_PATHOLOGIC] = eval("{" + ctx.field("pc_malignantStructures").value + "}")                                  
                           
      #--- remove d-structure from old places - with no more structures for this hazard-key remove also the directory (key)-----#  
      dictOfpathologicDicts[oldCharacter][oldHazard].remove(demandStruct)
      dictMetaCorrels[kindOfHazard_old][oldHazard].remove(demandStruct) 
      if not dictOfpathologicDicts[oldCharacter][oldHazard]:
        del dictOfpathologicDicts[oldCharacter][oldHazard] 
        del dictMetaCorrels[kindOfHazard_old][oldHazard] 
      
      #--- and insert it to the new directory (hazard-Dictionary of new character and dictMetaCorrels) on its correct index  ---#    
      #--- with an existing direction include and (handle the changes in the hazards-lists in the Dicts) -----------------------#
  
      #--- with an benign old character and another new one the maxDiameter has to be calculatet before ------------------------#
      if oldCharacter == _TYPE_BENIGN and newCharacter != _TYPE_BENIGN:
        ctx.field("METKMeasures.descriptionTag").value = INF_NAME
        ctx.field("METKMeasures.structure").value      = _cls_info.get(demandStruct, LAY_DESCRIPTION, INF_NAME)
        ctx.field("METKMeasures.start").touch()
        MLAB.processEvents()
            
      listOfChangedHazards = [] # memory-list for changed benign hazards-lists wich have to be sorted later by TSP
      
      if dictOfpathologicDicts[newCharacter].has_key(newHazard):                          
        
        if newCharacter != _TYPE_BENIGN:
          
          # with not-benig new character of demand find the correct position of it by the maxDiameter of the structs -----------#        
          demand_maxDiameter = _cls_info.get( demandStruct, LAY_MEASURES, INF_MAX_DIAMETER)
          for i_struct in range(len(dictOfpathologicDicts[newCharacter][newHazard])):
            if demand_maxDiameter > _cls_info.get(dictOfpathologicDicts[newCharacter][newHazard][i_struct], LAY_MEASURES, INF_MAX_DIAMETER):
              dictOfpathologicDicts[newCharacter][newHazard][i_struct:i_struct] = [demandStruct]
              break 
          else:
            dictOfpathologicDicts[newCharacter][newHazard].append(demandStruct)
          
          #--- update the metaCorrel-Dictionary ---#
          dictMetaCorrels[INF_DEGREEofSEVERITY][newHazard].append(demandStruct)
          dictMetaCorrels[INF_DEGREEofSEVERITY][newHazard] = sort_structures_TSP_Insert(dictMetaCorrels[INF_DEGREEofSEVERITY][newHazard])
          _cls_info.typedSet(demandStruct, LAY_DESCRIPTION, INF_DEGREEofSEVERITY, int(newHazard), INFOTYPE_INT32)
                
        else:  # with benign new character just append it on the benign-list of the new hazard (sortig follows later) ----------#
          dictOfpathologicDicts[newCharacter][newHazard].append(demandStruct)
          dictMetaCorrels[INF_HAZARDSTATE][newHazard].append(demandStruct) 
          listOfChangedHazards.append(newHazard) # memory for changed benign hazards-lists wich have to be sorted later by TSP 
          _cls_info.typedSet(demandStruct, LAY_DESCRIPTION, INF_HAZARDSTATE, int(newHazard), INFOTYPE_INT32)
          
      else:
        #--- with no existing directory create a new list with newHazard as key ------------------------------------------------#
        dictOfpathologicDicts[newCharacter][newHazard] = [demandStruct] 
        if newCharacter == _TYPE_BENIGN:
          dictMetaCorrels[INF_HAZARDSTATE][newHazard] = [demandStruct] 
          listOfChangedHazards.append(newHazard)
          _cls_info.typedSet(demandStruct, LAY_DESCRIPTION, INF_HAZARDSTATE, int(newHazard), INFOTYPE_INT32)
        else:
          dictMetaCorrels[INF_DEGREEofSEVERITY][newHazard] = [demandStruct] 
          _cls_info.typedSet(demandStruct, LAY_DESCRIPTION, INF_DEGREEofSEVERITY, int(newHazard), INFOTYPE_INT32)
      
      
      #- with different kindOfhazards raises, clear old Infolayer to maintain only one kind of hazard --------------------------#
      if _cls_info.existInfo(demandStruct, LAY_DESCRIPTION, INF_HAZARDSTATE) and \
        _cls_info.existInfo(demandStruct, LAY_DESCRIPTION, INF_DEGREEofSEVERITY):
        _cls_info.activateObject(demandStruct)         # this 4 rows are here because ... 
        _cls_info.activateLayer(LAY_DESCRIPTION)       # ...  _cls_obj has no .removeInf() yet
        _cls_info.activateInfo(kindOfHazard_old)           # ...  _cls_obj has no .removeInf() yet
        _cls_info.fld_removeInfo.touch()               # ...  _cls_obj has no .removeInf() yet
        
      #--- handle changes of the infiltration-hazard for possible endangered Structs (within the not-safe- and infiltrated-aerea)
      if not  oldCharacter == newCharacter == _TYPE_BENIGN:
        
        listsOfpatNeighb     = get_pathologNeighbours(demandStruct)
        listOfRelevants      = listsOfpatNeighb[0] + listsOfpatNeighb[1]
#        listOfChangedHazards = [newHazard] #(steht jetzt weiter oben)  memory for changed hazards-lists wich have to be sorted later by TSP like the new Hazard state of demand
        
        for relevant in listOfRelevants:
          relevant_hazard_old = str(_cls_info.get(relevant, LAY_DESCRIPTION, INF_HAZARDSTATE))   
          
          if relevant_hazard_old:  #--> check only benign Structs with an hazard-entry in objInfo (only they are in the benign lists)
            hazardInfos             = get_InfiltrationHazard2(relevant)
            relevant_hazard_new     = str(hazardInfos[0])
            relevant_endangeredDist = hazardInfos[1]
            
            #--- if the hazard of relevant structs changes handle the changes in the affected lists ----------------------------#
            if relevant_hazard_new != relevant_hazard_old:   
      
              #--- remove relevant struct from old List and clear key (directory)if there are no more elements for this key -#
              dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_old].remove(relevant)
              dictMetaCorrels[INF_HAZARDSTATE][relevant_hazard_old].remove(relevant)
              if not dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_old]:  
                del dictOfpathologicDicts[_TYPE_BENIGN][changedListKey]               
                del dictMetaCorrels[INF_HAZARDSTATE][relevant_hazard_old]
              
              #--- include relevant struct into the existing list or include a new one into dict if there isn't already one -#
              if dictOfpathologicDicts[_TYPE_BENIGN].has_key(relevant_hazard_new): 
                dictMetaCorrels[INF_HAZARDSTATE][relevant_hazard_new].append(relevant)
            
                #--- order relevant into the list of new benign-hazard-list ---#
                for i_hazardMember in range(len(dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_new])):
                  if relevant_endangeredDist < get_InfiltrationHazard2(dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_new][i_hazardMember])[1]:
                    dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_new].insert(i_hazardMember, relevant)
                    break 
                else:
                  dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_new].append(relevant) 
                    
              else:
                dictOfpathologicDicts[_TYPE_BENIGN][relevant_hazard_new] = [relevant]
                dictMetaCorrels[INF_HAZARDSTATE][relevant_hazard_new]    = [relevant]                
              _cls_info.typedSet(relevant, LAY_DESCRIPTION, INF_HAZARDSTATE, int(relevant_hazard_new), INFOTYPE_INT32)
                          
              #--- remember the changed Lists for following (TSP)-sorting ---------------------------------------------------# 
              listOfChangedHazards.extend([relevant_hazard_old, relevant_hazard_new])    
            
      #--- with Changes sort the new/changed lists ------------------------------------------------------------------------#
      for changedListKey in set(listOfChangedHazards):                             # set() cause of no duplicate elements
        if dictOfpathologicDicts[_TYPE_BENIGN].has_key(changedListKey):
#          dictOfpathologicDicts[newCharacter][changedListKey] = sort_structures_TSP_Insert(dictOfpathologicDicts[newCharacter][changedListKey])
          dictMetaCorrels[INF_HAZARDSTATE][changedListKey]    = sort_structures_TSP_Insert(dictMetaCorrels[INF_HAZARDSTATE][changedListKey]) 
          _cls_info.typedSet(demandStruct, LAY_DESCRIPTION, INF_HAZARDSTATE, int(changedListKey), INFOTYPE_INT32)
                
      
      #--- commit the patholDicts to the corresponding fields of the pathologic Correlation-Panel ------------------------------#
      if oldCharacter == _TYPE_BENIGN     or newCharacter == _TYPE_BENIGN:
        ctx.field("pc_benignStructures").value    = str(dictOfpathologicDicts[_TYPE_BENIGN])[1:-1] 
      if oldCharacter == _TYPE_DOUBTFUL   or newCharacter == _TYPE_DOUBTFUL:
        ctx.field("pc_doubtfulStructures").value  = str(dictOfpathologicDicts[_TYPE_DOUBTFUL])[1:-1] 
      if oldCharacter == _TYPE_PATHOLOGIC or newCharacter == _TYPE_PATHOLOGIC:
        ctx.field("pc_malignantStructures").value = str(dictOfpathologicDicts[_TYPE_PATHOLOGIC])[1:-1] 
     
      #--- commit the meta-correl-dictionary to the corresponding field of the "Correls in Meta_Infos"-Panel -------------------#      
      ctx.field("mc_DictContainer").value = str(dictMetaCorrels) 
      
        
    update_memberOfCharcter_Hazard()
      
  else:
    ctx.log("-set_DemandCharachter_Hazard(): NameError: no valid enum-name for character valids: '"+ _TYPE_BENIGN +"', '" + _TYPE_DOUBTFUL + "' or '"+ _TYPE_PATHOLOGIC + "' or no Structure selected.")
  pass 
  

def update_pathologicCorrelations():
  global _cls_info
  demandStruct = ctx.field("demandStructure").value
  
  if demandStruct:
    character                          = _cls_info.get(demandStruct, LAY_DESCRIPTION, INF_CHARACTER)
    ctx.field("pc_characterOfD").value = character
    patholSearchCriteria = []
    if character == _TYPE_BENIGN:
      ctx.field("pc_hazardState").value = _cls_info.getInt(demandStruct, LAY_DESCRIPTION, INF_HAZARDSTATE)
      patholSearchCriteria = [[INF_CHARACTER, "!=", _TYPE_BENIGN]]
    elif character == _TYPE_DOUBTFUL:
      ctx.field("pc_hazardState").value = _cls_info.getInt(demandStruct, LAY_DESCRIPTION, INF_DEGREEofSEVERITY)
      patholSearchCriteria = []
    else: # character == _TYPE_PATHOLOGIC:
      ctx.field("pc_hazardState").value = _cls_info.getInt(demandStruct, LAY_DESCRIPTION, INF_DEGREEofSEVERITY)
      patholSearchCriteria = [[INF_CHARACTER, "!=", _TYPE_PATHOLOGIC]]
    
    openGCFile = open_GeometricCorrelFile()    
    ctx.field("pc_InfiltDist1_Structures").value = str(map(operator.itemgetter(1), get_Structs_with_minDist_fullfillCriterias(openGCFile, ctx.field("indice_csdStruct").value, distThresholds = [0, ctx.field("pc_InfiltrationDistance1").value], searchCriterias = patholSearchCriteria)))
    ctx.field("pc_InfiltDist2_Structures").value = str(map(operator.itemgetter(1), get_Structs_with_minDist_fullfillCriterias(openGCFile, ctx.field("indice_csdStruct").value, distThresholds = [ctx.field("pc_InfiltrationDistance1").value, ctx.field("pc_InfiltrationDistance2").value], searchCriterias = patholSearchCriteria)))
    openGCFile.close()
  else:
    ctx.field("pc_characterOfD").value           = ""
    ctx.field("pc_hazardState").value            = -1
    ctx.field("pc_InfiltDist1_Structures").value = ""
    ctx.field("pc_InfiltDist2_Structures").value = ""
  pass


def update_memberOfCharcter_Hazard():
  
  character = ctx.field("pc_characterOfD").value
  hazard    = str(ctx.field("pc_hazardState").value)
  
  if character == _TYPE_BENIGN:
    try:    
      ctx.field("pc_memberOf_Char_Haz").value =  str(eval( "{" + ctx.field("pc_benignStructures").value + "}")[hazard])[1:-1]
      return
    except: raise ValueError, "could not find the value for infiltration-hazard " + str(hazard) + " for character " + character + "."
  elif character == _TYPE_DOUBTFUL:
    try:    
      ctx.field("pc_memberOf_Char_Haz").value =  str(eval( "{" + ctx.field("pc_doubtfulStructures").value + "}")[hazard])[1:-1]
      return
    except: raise ValueError, "could not find the value for infiltration-hazard " + str(hazard) + " for character " + character + "."
  elif character == _TYPE_PATHOLOGIC:
    try:    
      ctx.field("pc_memberOf_Char_Haz").value =  str(eval( "{" + ctx.field("pc_malignantStructures").value + "}")[hazard])[1:-1]
      return
    except: raise ValueError, "could not find the value for infiltration-hazard " + str(hazard) + " for character " + character + "."
  else:
    ctx.field("pc_memberOf_Char_Haz").value = ""
  pass


def update_combinedSearchCriteriaCompileds():  
  
  distanceInterval = ctx.field("cc_minDistanceInterval").value.replace(" ", "")
  distI            = None
  if distanceInterval:
    distanceInterval = distanceInterval.replace(";", ",").split(",")
    if len(distanceInterval) == 2:
      try: distI = [int(distanceInterval[0]), int(distanceInterval[1])]
      except ValueError: 
        ctx.log("No valid distance -thresh: 2 numbers sperated with a comma are required (e.g.: 0,5). no char" )
        return
    else:
      ctx.log("wrong number of Distance-Threshold-borders; 2 are required; for near- and far- threshold-border! " )
      ctx.field("cc_relatedStructures").value = ""
      return
    
  searchCriteriasTransform = ctx.field("cc_meta_searchCriterias").value.strip()
  searchCriterias = None   
  
  if searchCriteriasTransform :
    searchCriteriasTransform = searchCriteriasTransform.replace(",", ";").split(";")
    searchCriterias          = []
   
    for criteria in searchCriteriasTransform :
      try:
        criteria    = criteria.strip().split(" ") # build a list with space as seperator from sthe string with no space at start and end
        criteria[0] = str(criteria[0]).replace(" ", "")
        criteria[1] = str(criteria[1]).replace(" ", "")
        criteria[2] = str(criteria[2]).replace(" ", "")
        searchCriterias.append(criteria)
      except IndexError: 
        ctx.log("Sorry!, but '" + str(criteria)[1:-1].replace(",", " ").replace("'", "")  + "' is no valid search criteria!  -> 'Correlation == CorrelType' or 'Correlation != CorrelType' seperate more criterias with a simicolon or comma" )
        ctx.field("cc_relatedStructures").value = ""
        return
  try:
    openGCFile = open_GeometricCorrelFile()
    ctx.field("cc_relatedStructures").value = map(operator.itemgetter(1), get_Structs_with_minDist_fullfillCriterias(openGCFile, ctx.field("indice_csdStruct").value, distI, searchCriterias))
  
  #--- Error-exploration -------------------------------------------------------------------------------------------------------#
  except ValueError: 
    ctx.field("cc_relatedStructures").value = ""
    
    if distI and distI[0] > distI[1]:
        ctx.log("The first number has to be lower than or equal the second number! Try again with automatic changed entry.")
        ctx.field("cc_minDistanceInterval").value = str(distI[1]) + "," + str(distI[0])
          
    if searchCriterias:
      dictOfMetaCorrels = eval(ctx.field("mc_DictContainer").value)
      for criteria in searchCriterias:
        if dictOfMetaCorrels.has_key(criteria[0]):           
          if not dictOfMetaCorrels[criteria[0]].has_key(criteria[2]):
            ctx.log(criteria[2] + ' is no valid correlation Typ of correlation ' + criteria[0] + ".")
        else: ctx.log(criteria[0] + " is not a valid correlation for this case")
        if criteria[1] != "==" and criteria[1] != "!=":   
          ctx.log(criteria[1] + " is not a valid comparsion use == for 'equal' and != for 'not equal'")
    else:
      ctx.log("Hmmm, there is something wrong with combined correlation computation! Check the distance-threshes and search criterias")
  pass


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~ clean- Up and further handle-events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#

def cleanUp_DemandFields():
  """cleans up all demand-structure specific fields in the panel --> '' """
  global _indice_MetaCorrelType, _indice_MetaCorrelation
    
  ctx.field("allStructures").value    = ""
  ctx.field("demandStructure").value  = ""
  ctx.field("compareStructure").value = ""
  ctx.field("indice_csdStruct").value = 0
  ctx.field("indice_cscStruct").value = 0
  
  ctx.field("mc_AllCorrelations").value    = ""
  ctx.field("mc_Correlation").value        = ""
  ctx.field("mc_AllCorrelTypes").value     = ""
  ctx.field("mc_CorrelType").value         = ""
  ctx.field("mc_RelatedStructures").value  = ""
  ctx.field("mc_csRelatedStructure").value = ""
  ctx.field("mc_IndiceCsRelStruct").value  = 0
  
  ctx.field("gc_Neigbours").value      = "" 
  ctx.field("gc_NeigbMalignant").value = ""
  ctx.field("gc_NeigbDoubtful").value  = ""
  ctx.field("gc_NeigbBenign").value    = ""
  ctx.field("gc_Concealer").value      = ""
  ctx.field("gc_coveredObjects").value = ""
  
  ctx.field("pc_characterOfD").value           = ""
  ctx.field("pc_hazardState").value            = 0
  ctx.field("pc_InfiltDist1_Structures").value = ""
  ctx.field("pc_InfiltDist2_Structures").value = ""
  
  _indice_MetaCorrelType  = 0 
  _indice_MetaCorrelation = 0 
  
  cleanUp_CompareFields()  
  pass


def cleanUp_CompareFields():
  """cleans up all comparing (demand-structure vs. compare-structure) specific fields in the panel --> '' """
  ctx.field("mc_CorrelationsD_C").value       = ""  
  ctx.field("gc_DC_Distance").value           = -1 
  ctx.field("gc_DC_absOcclut_DC").value       = -1 
  ctx.field("gc_DC_absOcclut_CD").value       = -1 
  ctx.field("gc_DC_amountViews_DC").value     = -1 
  ctx.field("gc_DC_amountViews_CD").value     = -1
  ctx.field("gc_DC_IntersectionVolume").value = -1
  ctx.field("gc_DC_Nested").value             = -1 
  pass 


#*******************************************************************************************************************************#
#*** Handle further CaseEvents *************************************************************************************************#
#*******************************************************************************************************************************#
    
def handleSaveEvent():
  ctx.log("handleSaveEvent()") 
  pass
 

def handleCleanupEvent():
 
  global _cls_info, _cls_iter
  global _dict_minDistance
  
 # handleSaveEvent()
  ctx.field("caseLoaded").value = False 

  if ctx.field("calcEverythingWithSelection").value:
    ctx.field("calcEverythingWithSelection").value = False
    cleanUp_DemandFields()
    ctx.field("calcEverythingWithSelection").value = True 
  else:
    cleanUp_DemandFields()
  
  ctx.field("pc_CiteriaStructure").value     = ""
  ctx.field("pc_CriteriaStrProberty").value  = ""
  ctx.field("pc_CriteriaProbValue").value    = 0
  ctx.field("pc_CritDegreeOfSeverity").value = 0
  
  ctx.field("pc_malignantStructures").value  = ""
  ctx.field("pc_doubtfulStructures").value   = ""
  ctx.field("pc_benignStructures").value     = ""
  
  ctx.field("cc_relatedStructures").value    = ""
  ctx.log("METKCorrelations is cleared")  
  pass
   

def handleObjectCreatedEvent(field = 0):
  # To Do  
  ctx.log( 'handleObjectCreatedEvent()')
  pass


def handleObjectRemovedEvent(field = 0):
  # To Do  
  ctx.log('handleObjectRemovedEvent()')
  pass


def handleAttributeCreatedEvent(field = 0):
  # To Do  
  ctx.log('handleAttributeCreatedEvent()')
  pass

def handleAttributeRemovedEvent(field = 0):
  # To Do    
  ctx.log( "handleAttributeRemovedEvent()")
  pass


def handleAttributeModifiedEvent(field = 0):
  # To Do  
  ctx.log( "handleAttributeModifiedEvent()")
  pass
