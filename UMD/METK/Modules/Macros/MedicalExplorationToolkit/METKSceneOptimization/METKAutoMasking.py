# Python module import
from ObjMgr import *
from VisDefinitions import *  #Konstanten werden definiert statt String z.B. damit das gleich beim compilieren erkannt wird wenn ich mich verschreibe
from CorrlNet_Definitions import * # enthält die Netzwerk-Klasse, die das Correlation-Network darstellt
import pickle, pprint

#=== Global class instances ====================================================================================================#
_cls_info   = None  #cls = class
_cls_correl = None

#--- Global Variables --------#
_dictOfPrioritys       = {}
_dict_StructAppearance = {}

INF_CHARACTER      = "Character"
_STRUCTURE         = "Structure"
_CASE              = "Case"
_CORREL_NEIGHBOURS = "Neighbours"
_TYPE_MALIGNANT    = "Malignant"  # bösartig
_TYPE_BENIGN       = "Benign"     # gutartig
_TYPE_DOUBTFUL     = "Doubtful"   # verdächtig
#============================================================================================================ END GLOBAL =======#
  

def init():
  
   global _cls_info, _cls_correl
   ctx.log("init")
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED)   
   _cls_correl = METKObjCorrelation(ctx.module("METKCorrelationNetwork"))
   ctx.field("enableAnimation").value = False
   pass
 
  
def handleLoadedEvent():
   pass 

#*** update with new structures; after case-loading-event upon completion of correlation calculations **************************# 
def handleNewCaseCorrelation():
  
  global  _dictOfPrioritys, _dict_StructAppearance, _cls_info, _cls_correl
  
  #--- damit es nicht beim cleanen aufgerufen wird, wenn sich das Feld leertund das priofile erstellt wird
  if  _cls_info.get(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG) != "XML file loaded.":
    return 
  
#  ctx.log( 'newCaseLoadedMasking()')
  listOfStructures = _cls_correl.getAllStructures()

  for structure in listOfStructures:
    _dictOfPrioritys[structure] = [0, {}]
    if not _cls_info.existInfo(structure, LAY_APPEARANCE, INF_SILHOUETTE):
      _cls_info.typedSet(structure, LAY_APPEARANCE, INF_SILHOUETTE, False, INFOTYPE_BOOL)     
    if not _cls_info.existInfo(structure, LAY_APPEARANCE, INF_SILHOUETTECOLOR):
      _cls_info.typedSet(structure, LAY_APPEARANCE, INF_SILHOUETTECOLOR, '0 0 0', INFOTYPE_VEC3) 
    if not _cls_info.existInfo(structure, LAY_APPEARANCE, INF_SILHOUETTEWIDTH):
      _cls_info.typedSet(structure, LAY_APPEARANCE, INF_SILHOUETTEWIDTH, 2, INFOTYPE_DOUBLE) 
    
    if  _cls_info.get(structure, LAY_APPEARANCE, INF_SILHOUETTE) == True:   
      _dict_StructAppearance[structure]= {\
          INF_TRANSPARENCY    : _cls_info.get(structure, LAY_APPEARANCE, INF_TRANSPARENCY),\
          INF_SILHOUETTE      : True,\
          INF_SILHOUETTECOLOR : _cls_info.get(structure, LAY_APPEARANCE, INF_SILHOUETTECOLOR),\
          INF_SILHOUETTEWIDTH : _cls_info.get(structure, LAY_APPEARANCE, INF_SILHOUETTEWIDTH)}
    else:
      _dict_StructAppearance[structure]= {\
          INF_TRANSPARENCY : _cls_info.get(structure, LAY_APPEARANCE, INF_TRANSPARENCY),\
          INF_SILHOUETTE   : False }
  
  ctx.field("targetStructures").value = str([])
  savePriorityDictToFile() 
  saveStructAppearanceDictToFile()
  pass 

def savePriorityDictToFile():
  # save Prioritys  in  Files
  global _dictOfPrioritys
#  ctx.log("savePriorityDictToFile()'")
  MLABFileManager.remove(ctx.localPath()+ "/PriorityDist")
  openFile = open(ctx.localPath()+ "/PriorityDist", 'wb')
  # Pickle dictionary using protocol 0.  w writable b binär
  pickle.dump(_dictOfPrioritys, openFile) 
  openFile.close()
  pass
 
def loadPriorityDictFromFile():
  global _dictOfPrioritys
#  ctx.log("loadPriorityDictFromFile()'")
  pkl_file = open(ctx.localPath()+ "/PriorityDist", 'rb')
  _dictOfPrioritys       = pickle.load(pkl_file) 
  pkl_file.close()
  pass 

def saveStructAppearanceDictToFile():
  global _dict_StructAppearance
#  ctx.log("saveStructAppearanceDictToFile()'")
  MLABFileManager.remove(ctx.localPath()+ "/StructAppearance")
  openFile = open(ctx.localPath()+ "/StructAppearance", 'wb')
  pickle.dump(_dict_StructAppearance, openFile)
  openFile.close()
  pass
 
def loadStructAppearanceDictFromFile():
  global _dict_StructAppearance
#  ctx.log("loadStructAppearanceDictFromFile()'")
  pkl_file = open(ctx.localPath()+ "/StructAppearance", 'rb')
  _dict_StructAppearance = pickle.load(pkl_file)
  pkl_file.close()
  pass 


def setNeighbourDist():
  ctx.field("METKCorrelationNetwork.maxNeighbourDist").value = ctx.field("dist_close").value
  ctx.field("METKCorrelationNetwork.applyMaxDist").touch()
  pass

def selectionOfStructure():
  """generates a set of picked/selected structures und highlights those"""
 
  global _cls_info
  
  if  _cls_info.get(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG) != "XML file loaded.":
    return 
  
  structure = ctx.field("METKCSO.cso").value
#  ctx.log( "selectionOfStructure()" + structure)
  
  _cls_correl.setDemandStructure(structure)

  if structure == "":   
    demasking()
    ctx.field("targetStructures").value = []
    return
  
  elif ctx.field("single_groupSelection").value == 'single': 
   
    #--- add picked Structure to the empty Set of picked Structures - single selection ------------------------------#
    if not ctx.field("SoKeyGrabber.altPressed").value: 
      
      #>>> dehighlighting of previously highlighted and restart picking
      for pickedStruct in eval(ctx.field("targetStructures").value):
        setSilhouette_DehighlightAttr(pickedStruct)
        pass  
      ctx.field("targetStructures").value = [structure]
      setSilhouette_HighlightAttr(structure, color = "CSO_SL")
      ctx.field("lastSelectedStructure").value = structure 
      
      
      if ctx.field("enableAutomasking").value:
        set_Prioritys(structure)
        automasking()
      return
      
    #=== multiple selection of structures via 'Alt -Key' ============================================================# 
    #    - deselction of Structure if already selected or adding a new picked Structures ----------------------------#   
    else:
      # zuvor ausgewählte Struktur in der Farbe der picked Structure markieren bevor mit der neu gepickten Struktur weiter gemacht wird
      if ctx.field("lastSelectedStructure").value != "":
        setSilhouette_HighlightAttr(ctx.field("lastSelectedStructure").value, color = "selected_SL")
      listOfFocusStructs = eval(ctx.field("targetStructures").value)
      if listOfFocusStructs.__contains__(structure):
         listOfFocusStructs.remove(structure)
         setSilhouette_DehighlightAttr(structure)
         ctx.field("lastSelectedStructure").value = ""
         ctx.field("targetStructures").value = listOfFocusStructs
         set_Prioritys_removeFocus(structure)
      else:
         listOfFocusStructs.append(structure)
         setSilhouette_HighlightAttr(structure, color = "CSO_SL")
         ctx.field("targetStructures").value      = listOfFocusStructs
         ctx.field("lastSelectedStructure").value = structure
         if ctx.field("enableAutomasking").value:
           set_Prioritys(structure)
      automasking()
      return      
  
  else: # group-mode 

    _cls_correl.setDemandStructure(structure)
#    _cls_correl.enableColering()
    _cls_correl.getFirstCorrel()
    set_Prioritys_Correlgroup(structure)
    #'enableIrgendwas durchlaufen und auswählen der Strukturn'
    automasking()
  pass # // build_SetOfFocusStructures()

def set_Prioritys_Correlgroup(focus_struct):
  global _dictOfPrioritys, _cls_correl
#  ctx.log('set_Prioritys_Correlgroup(focus_struct)')
  loadPriorityDictFromFile()
  
  #=== Priority for frequency of correlations =================================================================#      
  prio_context       = 0.08
  normPrioCorrel     = 0.1
  listOfCorrelations = _cls_correl.getCorrelations()
  listLength         = len(listOfCorrelations)
  
  #outlier ohne Nachbarn und Importance -2 für maxPrio für Structure und Structuregroup
  outlier = 0
  if INF_IMPORTANCE in listOfCorrelations: outlier += 1      
  maxPrioCorrel = (1-prio_context-((listLength-outlier-2)*normPrioCorrel)) / 2
#  ctx.log(str(listLength)+": maxPrio = 1- "+ str(prio_context)+ " - " + str(normPrioCorrel)+ " * " + str(listLength-outlier-2)+ " / 2 = " + str(maxPrioCorrel)+ " = "+ str( (1 - prio_context - (normPrioCorrel * (listLength-outlier-2)))/2))  
  ctx.field("prio_correl").value = maxPrioCorrel
  for structure in _dictOfPrioritys.iterkeys():
    _dictOfPrioritys[structure][0]= prio_context
  for correl in listOfCorrelations:  
    if correl != INF_IMPORTANCE:
      _cls_correl.setCorrelation(correl)
      if correl == INF_STRUCTURE or correl == INF_STRUCTUREGROUP:
        for struct in _cls_correl.getRelatedStructs():          
          _dictOfPrioritys[struct][0] += maxPrioCorrel 
#          ctx.log(str(struct) + ": " + str(_dictOfPrioritys[struct][0]))
      else:
        relatedStructures = []
        if correl == _CORREL_NEIGHBOURS:
          _cls_correl.getFirstType()
          i = 0
          while _cls_correl.success():
            if _cls_correl.getRelatedStructs() != False:
              relatedStructures += _cls_correl.getRelatedStructs()  
            _cls_correl.getNextType()
            i += 1
        else:
          relatedStructures = _cls_correl.getRelatedStructs()
        for struct2 in relatedStructures:          
          _dictOfPrioritys[struct2][0] += normPrioCorrel    
 #       
  #=== compute the normalized priority of each structure ============================================================#   
  normalize_transform_Prioritys(1)         
  savePriorityDictToFile()
  pass

def set_Prioritys_removeFocus(removedFocusStruct):
  global _dictOfPrioritys  
  ctx.log("set_Prioritys_removeFocus(removedFocusStruct)")
  
  loadPriorityDictFromFile()
  max_prio_res = 0
  for struct, prioList in _dictOfPrioritys.iteritems():
    if prioList[1].has_key(removedFocusStruct):
       prioList[1].pop(removedFocusStruct)
    prioDist         = 0
    prioCharact      = 0
    prioCorrelAmount = 0
    for prios in prioList[1].itervalues():
      if prios[0] > prioDist:    prioDist   = prios[0]
      if prios[1] > prioCharact: prioCharact = prios[1]
      prioCorrelAmount += prios[2]
    prio_res = (ctx.field("weight_dist").value * prioDist)\
             * (ctx.field("weight_charact").value * prioCharact)\
             + (ctx.field("weight_correl").value * prioCorrelAmount) 
    if prio_res > max_prio_res: max_prio_res = prio_res               
    _dictOfPrioritys[struct][0]= prio_res        
  normalize_transform_Prioritys(max_prio_res)
  pass

def set_Prioritys(focus_struct):
  global _dictOfPrioritys, _cls_correl, _cls_info
  
  if focus_struct == "":
    return 
  else:
#    ctx.log( 'set_Prioritys()')
    _cls_correl.setCorrelMode()
    listOfFocusStructs = eval(ctx.field("targetStructures").value)
    loadPriorityDictFromFile()  
    
    # handle single- resp. multy-selection
    if not ctx.field("SoKeyGrabber.altPressed").value:
      _dictOfPrioritys[focus_struct] = [1 - ctx.field("maxPriority").value, {}] 
    else: 
      _dictOfPrioritys[focus_struct][0] = 1 - ctx.field("maxPriority").value
          
    #=== Priority for Distance to Focus-Structure =====================================================================#
    distDict = _cls_correl.getDistDict()
    for structPair, dist in distDict.iteritems():
      if focus_struct in structPair:
        if structPair[0]== focus_struct:
          struct0 = structPair[1]
        elif structPair[1]== focus_struct:
          struct0 = structPair[0]
        
        #=== single or multiselection =================================================================================#
        if not ctx.field("SoKeyGrabber.altPressed").value:
          _dictOfPrioritys[struct0]=[0, {focus_struct : [0,0,0]}]
        elif not _dictOfPrioritys[struct0][1].has_key(focus_struct):
          _dictOfPrioritys[struct0][1][focus_struct] = [0,0,0]  
        
        prio_dist = 0
        close = ctx.field("dist_close").value
        far   = ctx.field("dist_far").value
        if dist < close:
          prio_dist = ctx.field("prio_dist_close").value
        elif dist > far: 
          prio_dist = ctx.field("prio_dist_far").value
        else:
          maxP = ctx.field("prio_dist_close").value
          minP = ctx.field("prio_dist_far").value
          prio_dist = maxP - (((dist-close)/(far-close))*(maxP-minP))
  
        _dictOfPrioritys[struct0][1][focus_struct] = [prio_dist,0,0]   

    #=== Priority for Character conformity ============================================================================#
    prioCorrel  = 0
    prioCharact = 0
    _cls_correl.setDemandStructure(focus_struct)
    listOfCorrelations = _cls_correl.getCorrelations(focus_struct)
    
    for correl in listOfCorrelations:  
      
      if correl != _CORREL_NEIGHBOURS:          
        
        #=== Priority for Character conformity ======================================================================#
        if correl == INF_CHARACTER:  
          _cls_correl.setCorrelation(correl)
          charFokus = _cls_info.get( focus_struct, LAY_DESCRIPTION, INF_CHARACTER)
          for struct0 in _dictOfPrioritys.iterkeys(): 
            if struct0 != focus_struct:
              charStruct0 = _cls_info.get( struct0, LAY_DESCRIPTION, INF_CHARACTER)
              prioCh      = 0
              if charStruct0 == charFokus :
                prioCh = ctx.field("prio_charact_same").value
              elif charFokus == _TYPE_BENIGN:
                if charStruct0 == _TYPE_DOUBTFUL:
                  prioCh = ctx.field("prio_charact_other").value
                else: prioCh = ctx.field("prio_charact_inverse").value
              elif charFokus == _TYPE_DOUBTFUL:
                if charStruct0 == _TYPE_MALIGNANT:
                  prioCh = ctx.field("prio_charact_other").value
                else: prioCh = ctx.field("prio_charact_inverse").value 
              else:
                if charStruct0 == _TYPE_DOUBTFUL:
                  prioCh = ctx.field("prio_charact_other").value
                else: prioCh = ctx.field("prio_charact_inverse").value 
                
              _dictOfPrioritys[struct0][1][focus_struct][1] = prioCh
        
        elif correl == INF_STRUCTURE:
          _cls_correl.setCorrelation(INF_STRUCTURE)
          prioRelatedStruct = ctx.field("maxPriority").value 
          prioRelatedNeighb = ctx.field("prio_dist_close").value*ctx.field("weight_dist").value * ctx.field("weight_charact").value*ctx.field("prio_charact_inverse").value
          for related in _cls_correl.getRelatedStructs()[1:]:
            _cls_correl.setDemandStructure(related)
            _cls_correl.setCorrelation(_CORREL_NEIGHBOURS)            
            if _cls_correl.getRelatedStructs(_TYPE_MALIGNANT) != False:
              _dictOfPrioritys[related][1][focus_struct][2] = prioRelatedStruct
              for relatMaligNeighbour in _cls_correl.getRelatedStructs():
                _dictOfPrioritys[relatMaligNeighbour][1][focus_struct][2] = prioRelatedNeighb
            if _cls_correl.getRelatedStructs(_TYPE_DOUBTFUL) != False:
              _dictOfPrioritys[related][1][focus_struct][2] = prioRelatedStruct
              for relatDoubtNeighbour in _cls_correl.getRelatedStructs():
                if relatDoubtNeighbour != focus_struct: 
                  _dictOfPrioritys[relatDoubtNeighbour][1][focus_struct][2] = prioRelatedNeighb
#          _cls_correl.setDemandStructure(focus_struct)
           
#        else:
#          #=== Priority for frequency of correlations =================================================================#    
#          if len(listOfCorrelations) != 0:
#            maxPrioCorrel = ctx.field("maxPriority").value /len(listOfCorrelations)
#            ctx.field("prio_correl").value = maxPrioCorrel
#          for struct1 in _cls_correl.getRelatedStructs1()[1:]:          
#            if struct1 != focus_struct: 
#              _dictOfPrioritys[struct1][1][focus_struct][2] += ctx.field("prio_correl").value 
      
    #=== compute the resulting priority of each structures ============================================================#
    max_prio_res = 0
    for structure, prioList in _dictOfPrioritys.iteritems():
      if structure not in listOfFocusStructs:
        prioDist         = 0
        prioCharact      = 0
        prioCorrelAmount = 0
        for prios in prioList[1].itervalues():
          if prios[0] > prioDist:   prioDist     = prios[0]               # max PrioDist
          if prios[1] > prioCharact: prioCharact = prios[1]               # max CharactPrio
          prioCorrelAmount += prios[2]                                    # frequenzy of corelations of all focusstructures   
        
        prio_res = (ctx.field("weight_dist").value    * prioDist)\
                 * (ctx.field("weight_charact").value * prioCharact)\
                 + (ctx.field("weight_correl").value  * prioCorrelAmount) # resulting priority (not normed)
        
        if prio_res > max_prio_res: max_prio_res = prio_res               
        _dictOfPrioritys[structure][0]= prio_res      
#    print '--------------------------------------------------------------------------------------------------------------------'
#    pprint.pprint(_dictOfPrioritys)
    #=== compute the normalized priority of each structure ============================================================#   
    normalize_transform_Prioritys(max_prio_res)
  pass

def normalize_transform_Prioritys(max_prio_res):
  ''' normalizes the priority of each structure and transform all of them to transparent-values'''
  global _dictOfPrioritys
 
  norm = 1
  listOfFocusStructs = eval(ctx.field("targetStructures").value)
  if max_prio_res > ctx.field("maxPriority").value: 
    norm = (ctx.field("maxPriority").value) / max_prio_res
  for structure in _dictOfPrioritys.iterkeys():
    if not structure in listOfFocusStructs:  
      _dictOfPrioritys[structure][0] = 1 -  norm * _dictOfPrioritys[structure][0]
  savePriorityDictToFile()  
  pass 


def automasking():  
  '''define renderingstile of each struckture (trancparenzy and silhouette)'''
  global _cls_info, _dictOfPrioritys
  
  if ctx.field("enableAutomasking").value:
#    ctx.log( 'automasking()')
    if ctx.field("enableAnimation").value:
      cso_color = str(ctx.field("color_CSO").value)[1:-1].replace(', ',' ')
      select_color = str(ctx.field("color_selStructuresSilhouet").value)[1:-1].replace(', ',' ')
      ctx.field("UMDAnimation2.ScriptArea").value = "[Ini]\nLengthTimeUnit=5\nLengthSeconds=2\nRealTime=yes\n\n[Script]\n[0] 'System' setBackground white"
      
      for structure, list_prio in _dictOfPrioritys.iteritems():
        sil_color = _cls_info.get(structure, LAY_APPEARANCE, INF_SILHOUETTECOLOR)

        if list_prio[0] > (1 - ctx.field("maxPriority").value):
          _cls_info.typedSet(structure, LAY_APPEARANCE, INF_SILHOUETTE, False, INFOTYPE_BOOL) 
          #ctx.field("UMDAnimation2.ScriptArea").value = ctx.field("UMDAnimation2.ScriptArea").value + "\n" + "[0,5] '" + structure + "' setSilouhette False"  
        ctx.field("UMDAnimation2.ScriptArea").value = ctx.field("UMDAnimation2.ScriptArea").value + "\n" + "[0,5] '" + structure + "' setTransparency " + str(list_prio[0]) 
      ctx.field("UMDAnimation2.ScriptAreaExecute").touch() 
    
    else:
      for structure, list_prio in _dictOfPrioritys.iteritems():
        if list_prio[0] > (1 - ctx.field("maxPriority").value): 
          _cls_info.typedSet(structure, LAY_APPEARANCE, INF_SILHOUETTE, False, INFOTYPE_BOOL)
        _cls_info.typedSet(structure, LAY_APPEARANCE, INF_TRANSPARENCY, list_prio[0], INFOTYPE_DOUBLE) #)typeT)
      _cls_info.notify()    
  pass

def demasking():
  '''ausgangsdarstellung wieder anzeigen'''
  global _cls_info, _dictOfPrioritys, _dict_StructAppearance
#  ctx.log( 'demasking()')
  loadStructAppearanceDictFromFile() #lade appearenceDict
  if ctx.field("enableAnimation").value:
    ctx.field("UMDAnimation2.ScriptArea").value = "[Ini]\nLengthTimeUnit=5\nLengthSeconds=2\nRealTime=yes\n\n[Script]\n[0] 'System' setBackground white"
    for structure, appearances in _dict_StructAppearance.iteritems():
      for inf_appear, info in appearances.iteritems(): 
        ctx.field("UMDAnimation2.ScriptArea").value = ctx.field("UMDAnimation2.ScriptArea").value + "\n" + "[0,5] '" + structure + "' set" + inf_appear.capitalize()+ " " + str(info) 
    ctx.field("UMDAnimation2.ScriptAreaExecute").touch()     
  else:
#    pprint.pprint(_dict_StructAppearance)
    for structure, appearances in _dict_StructAppearance.iteritems():
#      print structure 
      for inf_appear, info in appearances.iteritems():      
        _cls_info.set(structure, LAY_APPEARANCE, inf_appear, info)
#        ctx.log( inf_appear + " " + str(info) )
      setSilhouette_DehighlightAttr(structure) # temp wegen roter linien wenn demaskiert  
    _cls_info.notify()        
  pass 


def switchAutomaskingMode(field = 0):
#  ctx.log('switchAutomaskingMode(field = 0)')
  listOfFocusStr =  eval(ctx.field("targetStructures").value)
  if ctx.field("enableAutomasking").value and listOfFocusStr != []:
    ctx.field("SoKeyGrabber.altPressed").value = True
    for structure in listOfFocusStr:
      set_Prioritys(structure)
    automasking() 
    ctx.field("SoKeyGrabber.altPressed").value = False
  else:
    demasking()
  pass


#*** highlighting by silhouetts coloring ***************************************************************************************#
def setSilhouette_HighlightAttr(structure, color = "selected_SL", width = 5):

  global _cls_info, _dict_StructAppearance
#  ctx.log('setSilhouette_HighlightAttr(str, col, w)')
  if color == "CSO_SL" :
    color = str(ctx.field("color_CSO").value)[1:-1].replace(', ',' ')
  else: 
    color = str(ctx.field("color_selStructuresSilhouet").value)[1:-1].replace(', ',' ')
    
  _cls_info.set(structure, LAY_APPEARANCE, INF_SILHOUETTE,      True)
  _cls_info.set(structure, LAY_APPEARANCE, INF_SILHOUETTECOLOR, color)
  _cls_info.set(structure, LAY_APPEARANCE, INF_SILHOUETTEWIDTH, width)
  _cls_info.notify()
  pass

#*** convert back silhouette highlighting (into original appearance) ***********************************************************#
def setSilhouette_DehighlightAttr(structure ):

  global _cls_info, _dict_StructAppearance
#  ctx.log('setSilhouette_DeHighlightAttr(str)')
  loadStructAppearanceDictFromFile() 
  _cls_info.set(structure, LAY_APPEARANCE, INF_SILHOUETTE, _dict_StructAppearance[structure][INF_SILHOUETTE])
  if _dict_StructAppearance[structure][INF_SILHOUETTE] == True:
    _cls_info.set(structure, LAY_APPEARANCE, INF_SILHOUETTECOLOR, _dict_StructAppearance[structure][INF_SILHOUETTECOLOR])
    _cls_info.set(structure, LAY_APPEARANCE, INF_SILHOUETTEWIDTH, _dict_StructAppearance[structure][INF_SILHOUETTEWIDTH])
  _cls_info.notify()      
  pass 


#*** trigger the pressed key's responding event if existing ********************************************************************#
def handleKeyboardEvent():
  
  lastKey = ctx.field("SoKeyGrabber.lastKey").value
  
  #--- trigger MASKING-EVENT when enabled --------------------------------------------------------------------------------------#
  if lastKey == ctx.field("autoMasking_Shortcut").value and ctx.field("enableAutomasking").value:
#    masking()
    return 
  
  #--- trigger Group-Mode ------------------------------------------------------------------------------------------------------#
  elif lastKey == ctx.field("key_groupmode").value:
    if ctx.field("activeCorrelationMode").value: ctx.field("activeCorrelationMode").value = False 
    else: ctx.field("activeCorrelationMode").value = True 
    return 
  elif ctx.field("activeCorrelationMode").value \
    and lastKey == 'RETURN' or lastKey == 'PAD_ENTER'\
    or  lastKey == ctx.field("key_group_run_forwards").value\
    or  lastKey == ctx.field("key_group_run_backwards").value:   
    handleGroupMode(lastKey)
  pass # // handle keybordevents


def handleCleanupEvent():
 
   global _cls_correl, _dictOfPrioritys, _dict_StructAppearance  
   
   ctx.log("clean METKAutomasking")
   ctx.field("METKCSO.cso").value = ""
   
   demasking()
   
   ctx.field("targetStructures").value = ""
   ctx.field("cSM").value = ""
   ctx.field("UMDAnimation2.ScriptArea").value = ""
   _dict_StructAppearance.clear()
   _dictOfPrioritys.clear()

   MLABFileManager.remove(ctx.localPath()+ "/PriorityDist")
   MLABFileManager.remove(ctx.localPath()+ "/StructAppearance") 
    
   ctx.log("AutoMasking clean")
   pass